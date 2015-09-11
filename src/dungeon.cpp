/*
 * Ian Milligan 
 * CS 488 Assignment 5
 */
#include "dungeon.hpp"
#include "image.hpp"
#include <iostream>
#include <sys/time.h>
#include <climits>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdlib>
#include <cmath>
#include <pthread.h> 
#include "settings.hpp"
#include "scene_lua.hpp"

using namespace std;

// Simple structure to backtrack while pathfinding
class PathNode {
public:
  PathNode() {}
  PathNode(Point2D point, PathNode* parent) {
    this->point = point;
    this->parent = parent;
  }
  Point2D point;
  PathNode* parent;
};

// Comparer (the default isn't enough)
struct Point2DCompare {
  bool operator() (const Point2D& lhs, const Point2D& rhs) const{
    if (lhs[0] == rhs[0]) {
      return lhs[1] < rhs[1];
    } else {
      return lhs[0] < rhs[0];
    }
  }
};

// A simple struct to store information about pathfinding.
// Used to make pathfinding multithreaded
struct PathfindingInstructions {
  Point2D current;
  Point2D destination;
  Being *reciever;
  Dungeon *dungeon;
};

// Just draw the floor as a quad, rather than a full cube
void draw_floor(double x, double y, double z, double size) {
  glBegin(GL_QUADS);  
  glNormal3d(0, 0, 1);
  glTexCoord2f (0.0, 0.0);
  glVertex3d(x, y, z);
  glTexCoord2f (1.0, 0.0);
  glVertex3d(x + size, y, z);
  glTexCoord2f (1.0, 1.0);
  glVertex3d(x + size, y + size, z);
  glTexCoord2f (0.0, 1.0);
  glVertex3d(x, y + size, z);
  glEnd();
}

// Get a random in a given range
double random_range(double min, double max) {
  double f = (double)rand() / RAND_MAX;
  double ret =  min + f * (max - min);
  return ret;
}

// Display a face (from a doubly-linked list of points)
// Rotate the other direction if invert_normal is flipped
void display_face(std::vector<std::vector<Point3D> > face, bool invert_normal) {
  int points = face.size();
  float increment = (float)1 / (points - 1);

  for (float i = 0; i < points - 1; i++) {
    for (float j = 1; j < points; j++) {
      // The resulting triangles look something like this:
      /*
       * 3--2
       * |\ |
       * | \|
       * 4--1
       */
      Point3D point1;
      Point3D point2;
      Point3D point3;
      Point3D point4;

      if (invert_normal) {
        point1 = face[i + 1][j - 1];      
        point2 = face[i + 1][j];
        point3 = face[i][j];
        point4 = face[i][j - 1];
      } else {
        point1 = face[i][j - 1];
        point2 = face[i][j];
        point3 = face[i + 1][j];
        point4 = face[i + 1][j - 1];
      }

      Vector3D normal1 = (point3 - point1).cross(point3 - point2);
      normal1.normalize();
      glNormal3d(normal1[0], normal1[1], normal1[2]);
      glTexCoord2f(i * increment, ((j - 1) * increment)); glVertex3d(point1[0], point1[1], point1[2]);
      glTexCoord2f(i * increment, (j * increment)); glVertex3d(point2[0], point2[1], point2[2]);
      glTexCoord2f((i + 1) * increment, (j * increment)); glVertex3d(point3[0], point3[1], point3[2]);
      
      Vector3D normal2 = (point3 - point1).cross(point4 - point1);
      normal2.normalize();
      glNormal3d(normal2[0], normal2[1], normal2[2]);
      glTexCoord2f(i * increment, ((j - 1) * increment)); glVertex3d(point1[0], point1[1], point1[2]);
      glTexCoord2f((i + 1) * increment, (j * increment)); glVertex3d(point3[0], point3[1], point3[2]);
      glTexCoord2f((i + 1) * increment, ((j - 1) * increment)); glVertex3d(point4[0], point4[1], point4[2]);
    }
  }
}


Dungeon::Dungeon() 
  : has_display_list(false),
    has_texture(false),
    has_lights(false) { }

Dungeon::~Dungeon() { }

void Dungeon::draw_wall(double x, double y, double z, Piece description, double size) {
  // Use a display list unique to each wall peice
  if (description.display_list == -1) {
    GLuint index = glGenLists(1);
    description.display_list = index;
    glNewList(index, GL_COMPILE);
    glBegin(GL_QUADS);

    // First face in xy plane 
    glNormal3d(0, 0, -1);
    glTexCoord2f (0.0, 0.0);
    glVertex3d(x, y, z);
    glTexCoord2f (1.0, 0.0);
    glVertex3d(x, y + size, z);
    glTexCoord2f (1.0, 1.0);
    glVertex3d(x + size, y + size, z);
    glTexCoord2f (0.0, 1.0);
    glVertex3d(x + size, y, z);


    glEnd();
    glBegin(GL_TRIANGLES);

    // Draw each of our fractal walls
    display_face(description.face1, true);
    display_face(description.face2, true);
    display_face(description.face3, false);
    display_face(description.face4, false);

    glEnd();
    glBegin(GL_QUADS);

    // Second face in xy plane
    glNormal3d(0, 0, 1);
    glTexCoord2f (0.0, 0.0);
    glVertex3d(x, y, z + size);
    glTexCoord2f (1.0, 0.0);
    glVertex3d(x + size, y, z + size);
    glTexCoord2f (1.0, 1.0);
    glVertex3d(x + size, y + size, z + size);
    glTexCoord2f (0.0, 1.0);
    glVertex3d(x, y + size, z + size);

    glEnd();
    glEndList();
  }

  glCallList(description.display_list);
}

bool Dungeon::loadFromFile(char * file) {
  // Load in the png
	Image dungeon_image = Image();
  if (!dungeon_image.loadPng(file)) {
    cerr << "Image loading failed" << endl;
    return false;
  }

  // Keep track of important things
  width = dungeon_image.width();
  height = dungeon_image.height();
  int depth = dungeon_image.elements();

  // First pass: build the grid
  dungeon = new Piece*[width];
  for (int i = 0; i < width; i++) {
    dungeon[i] = new Piece[height];
    for (int j = 0; j < height; j++) {
      // Black: wall
      bool black = (dungeon_image(i, j, 0) == 0.0 &&
                    dungeon_image(i, j, 1) == 0.0 && 
                    dungeon_image(i, j, 2) == 0.0);
      // Red: enemy
      bool red = (dungeon_image(i, j, 0) != 0.0 &&
                  dungeon_image(i, j, 1) == 0.0 && 
                  dungeon_image(i, j, 2) == 0.0);
      // Green: player
      bool green = (dungeon_image(i, j, 0) == 0.0 &&
                  dungeon_image(i, j, 1) != 0.0 && 
                  dungeon_image(i, j, 2) == 0.0);
      if (green) {
        player->setPosition(Point3D(i, j, 0));
      }
      if (red) {
        addEnemy(i, j);
      }
      Piece piece;
      piece.is_wall = black;
      piece.display_list = -1;
      dungeon[i][j] = piece;
    }
  }

  // Do a second pass and generate all the walls.
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      createPeice(i, j);
    }
  }
}

void Dungeon::addPlayerModel(SceneNode* player, int sword_sound) {
  this->playerModel = player;
  // Actually add a new being
  this->player = new Being(playerModel, sword_sound);
}

void Dungeon::addEnemyModel(char* enemy_file) {
  this->enemyFile = enemy_file;
}
void Dungeon::addEnemy(double x, double y) {
  // Load in a new structure every time
  // If we were to share nodes the joints would all move
  // in tandem
  SceneNode *enemy_structure = import_lua(enemyFile);
  Being * new_enemy = new Being(enemy_structure);
  new_enemy->setPosition(Point3D(x, y, 0));
  this->enemies.push_back(new_enemy);
}

void Dungeon::runGL(bool picking) {
  // Setup display lists if we haven't already
  if (!has_display_list) {
    createDisplayLists();
  }

  // Setup textures if we haven't already
  if (!has_texture) {
    initTexture();
  }

  if (!has_lights) {
    GLfloat light_position[] = {0, -1, 1.5, 1};
    GLfloat light_color[]={1.0, 1.0, 1.0, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
    glEnable(GL_LIGHT0);
  }

  // Draw the player
  glPushMatrix();
  glRotated(90, 1, 0, 0);
  glTranslated(PIECE_SIZE / 2, 1.5, - PIECE_SIZE / 2);
  glScaled(0.5, 0.5, 0.5);
  player->runGL(picking);  
  glPopMatrix();
  
  // Move the whole board so it is centered on the player
  Point3D displacement = player->getPosition();
  glTranslated(-(displacement[0] + 0.5), -(displacement[1] + 0.5), 0);

  // We are now drawing enemies
  if (picking) {
    glLoadName(NAME_ENEMY);
  }

  for (int i = 0; i < enemies.size(); i++) {
    // Push the name so we know which one is being displayed
    if (picking) {
      glPushName(i);
    }
    glPushMatrix();
    Point3D location = enemies[i]->getPosition();
    glTranslated(location[0], location[1], 0);
    glRotated(90, 1, 0, 0);
    if (picking) {
      // If we are picking, just draw the enemies as big cubes
      glScaled(0.5, 1.0, 0.5);
      glTranslated(0, 1.0, -PIECE_SIZE);
      draw_cube(0, 0, 0, PIECE_SIZE);
    } else {
      // Else actually draw them
      glScaled(0.5, 0.5, 0.5);
      glTranslated(PIECE_SIZE / 2, 3.0, - PIECE_SIZE / 2);
      enemies[i]->runGL(picking);
    }
    glPopMatrix();
    if (picking) {
      glPopName();
    }
  }

  // The actual dungeon
  if (picking) {
    glLoadName(NAME_DUNGEON);
  }

  if (picking) {
    // Call the picking list: free of textures and fractals
    glCallList(DUNGEON_PICKING);
  } else {
    wall_material->apply_gl();
    // A bit of complicated work here.
    // First draw the border (out of cubes)
    for (int j = -1; j < height + 1; j++) {
      draw_cube(-1, j, 1, 1);
      draw_cube(width, j, 1, 1);
    }
    for (int i = 0; i < width; i++) {
      draw_cube(i, -1, 1, 1);
      draw_cube(i, height, 1, 1);
    }

    int player_x = (int)player->getPosition()[0];
    int player_y = (int)player->getPosition()[1];
    for (int i = 0 ; i < width; i++) {
      // If the row is too far away, don't draw it
      if (i < player_x - DRAW_DISTANCE || i > player_x + DRAW_DISTANCE) {
          continue;
      }
      for (int j = 0; j < height; j++) {
        // If the column is too far away, don't draw it
        if (j < player_y - DRAW_DISTANCE || j > player_y + DRAW_DISTANCE) {
          continue;
        }

        // If it is a wall, draw the fractal wall
        if (dungeon[i][j].is_wall) {
          glPushMatrix();
          glTranslated(0, 0, 1);
          if (settings.fractals) {
            draw_wall(i, j, 0, dungeon[i][j], 1);
          } else {
            draw_cube(i, j, 0, 1);
          }                        
          glPopMatrix();
        }
        // Draw the floor
        draw_floor(i, j, 1, 1);
      }
    }
    wall_material->end_gl();
  }
}

// For use with threads: pathfind and add the result to the being when done
// This doesn't block the ui thread
void *being_pathfind(void * instructions) {
  PathfindingInstructions * find = (PathfindingInstructions *)instructions;
  vector<Point3D> new_path = find->dungeon->pathfind((int)find->current[0], (int)find->current[1], (int)find->destination[0], (int)find->destination[1]);
  // No path was found
  if (new_path.size() > 0) {
    find->reciever->setTarget(NULL);
    find->reciever->setPath(new_path);
  }
  delete instructions;
  return NULL;
}

void Dungeon::moveTo(double x, double y) {
  // Some c++ lovelyness
  void *being_pathfind(void *);

  // Setup the instructions for pathfinding
  Point3D displacement = player->getPosition();
  PathfindingInstructions *instructions = (PathfindingInstructions *)malloc(sizeof(PathfindingInstructions));
  instructions->current = Point2D(displacement[0], displacement[1]);
  instructions->destination = Point2D(x, y);
  instructions->reciever = player;
  instructions->dungeon = this;
  pthread_t thread;
  pthread_create(&thread, NULL, being_pathfind, (void *)instructions);
}

void Dungeon::target(int enemy_number) {
  // If the enemy actually exists
  if (enemies.size() > enemy_number) {
    // The user has clicked on an enemy.
    // Actually target him
    player->setTarget(enemies[enemy_number]);
    Point3D player_location = player->getPosition();
    Point3D target_location = enemies[enemy_number]->getPosition();
    vector<Point3D> target_path = pathfind((int)player_location[0], (int)player_location[1], (int)target_location[0], (int)target_location[1]);
    player->setPath(target_path);
  }
}

// Return whether or not to repaint
bool Dungeon::tick() {

  // Tick the player
  bool repaint = player->tick();
  for (std::vector<Being *>::iterator it = enemies.begin() ; it != enemies.end(); ++it) {
    repaint |= (*it)->tick();

    // If we are fleeing, pathfind to a new location away from the player
    if ((*it)->getFleeing() != NULL) {
      Vector3D away = (*it)->getPosition() - (*it)->getFleeing()->getPosition();
      away.normalize();
      Point3D current_position = (*it)->getPosition();
      Point3D new_position = current_position + (FLEE_DISTANCE * away);
      if ((int)new_position[0] >= 0 && (int)new_position[0] <= width - 1 &&
          (int)new_position[1] >= 0 && (int)new_position[1] <= height - 1 &&
          !dungeon[(int)new_position[0]][(int)new_position[1]].is_wall) {
        // Setup pathfinding
        PathfindingInstructions *instructions = (PathfindingInstructions *)malloc(sizeof(PathfindingInstructions));
        instructions->current = Point2D(current_position[0], current_position[1]);
        instructions->destination = Point2D(new_position[0], new_position[1]);
        instructions->reciever = (*it);
        instructions->dungeon = this;
        pthread_t thread;
        pthread_create(&thread, NULL, being_pathfind, (void *)instructions);
      }
    } else if (player->getTarget() == NULL) {
      // See if the player is close enough to attack this enemy
      double distance = ((*it)->getPosition() - player->getPosition()).length();
      if (distance < ATTACK_DISTANCE) {
        // A little to close for comfort
        player->setTarget(*it);
        player->setPath(vector<Point3D>());
      }
    }
  }
  return repaint;
}

// Given a grid of points (of equal dimensions) find the midpoint and displace it in the 'axis' direction
// by some amount.  Then, repeat the process on all sub-quadrants
/*
 * .---.
 * | | |
 * |-*-|
 * | | |
 * .---.
 */
void fractal(vector<vector<Point3D> >* points, int left_index, int right_index, int bottom_index, int top_index, int axis) {
  if (right_index - left_index <= 1) {
    return;
  }

  // We should always have some power of two
  int horizontal_midpoint = (left_index + right_index) / 2;
  int vertical_midpoint = (bottom_index + top_index) / 2;

  double expected = ((*points)[left_index][right_index][axis] + (*points)[bottom_index][top_index][axis]) / 2;
  (*points)[horizontal_midpoint][vertical_midpoint][axis] = expected + random_range(-FRACTAL_EPSILON, FRACTAL_EPSILON);
  
  // Work on the children
  fractal(points, left_index, horizontal_midpoint, bottom_index, vertical_midpoint, axis);
  fractal(points, horizontal_midpoint, right_index, bottom_index, vertical_midpoint, axis);
  fractal(points, horizontal_midpoint, right_index, vertical_midpoint, top_index, axis);
  fractal(points, left_index, horizontal_midpoint, vertical_midpoint, top_index, axis);
}
 
void Dungeon::createPeice(int x, int y) {
  float increment = PIECE_SIZE / FRACTAL_COMPLEXITY;

  if ((y > 0 && !dungeon[x][y - 1].is_wall) || (y < (height - 1) && !dungeon[x][y + 1].is_wall)) {
    // The xz planes: only create the fractal if they are visible  (there isn't a wall in both directions)
    vector<vector<Point3D> > face1 = vector<vector<Point3D> >(FRACTAL_COMPLEXITY + 1);

    // Initialize a flat surface (a grid with all the y values the same)
    for (int i = 0; i < FRACTAL_COMPLEXITY + 1; i++) {
      face1[i] = vector<Point3D>(FRACTAL_COMPLEXITY + 1);
      for (int j = 0; j < FRACTAL_COMPLEXITY + 1; j++) {
        face1[i][j] = Point3D(x + (increment * i), y, j * increment);
      }
    }

    // Use the first face to create the opposite face, just adding PIECE_SIZE to each of the y 
    // values
    if (y < (height - 1) && !dungeon[x][y + 1].is_wall) {
      vector<vector<Point3D> > opposite_face1 = vector<vector<Point3D> >(FRACTAL_COMPLEXITY + 1);
      for (int i = 0; i < FRACTAL_COMPLEXITY + 1; i++) {
        opposite_face1[i] = vector<Point3D>(FRACTAL_COMPLEXITY + 1);
        for (int j = 0; j < FRACTAL_COMPLEXITY + 1; j++) {
          opposite_face1[i][j] = Point3D(face1[i][j][0], face1[i][j][1] + PIECE_SIZE, face1[i][j][2]);
        }
      }

      // Introduce randomness
      fractal(&opposite_face1, 0, FRACTAL_COMPLEXITY, 0, FRACTAL_COMPLEXITY, 1);
      dungeon[x][y].face3 = opposite_face1;
    }

    if (y > 0 && !dungeon[x][y - 1].is_wall) {
      // Introduce randomness into the original wall
      fractal(&face1, 0, FRACTAL_COMPLEXITY, 0, FRACTAL_COMPLEXITY, 1);
      dungeon[x][y].face1 = face1;
    }    
  }

  // The yz planes
  if ((x > 0 && !dungeon[x - 1][y].is_wall) || (x < (width - 1) && !dungeon[x + 1][y].is_wall)) {
    vector<vector<Point3D> > face2 = vector<vector<Point3D> >(FRACTAL_COMPLEXITY + 1);

    // Initialize a flat surface (a grid with all the y values the same)
    for (int i = 0; i < FRACTAL_COMPLEXITY + 1; i++) {
      face2[i] = vector<Point3D>(FRACTAL_COMPLEXITY + 1);
      for (int j = 0; j < FRACTAL_COMPLEXITY + 1; j++) {
        face2[i][j] = Point3D(x, y + (increment * i), j * increment);
      }
    }

    // Use the first face to create the opposite face, just adding PIECE_SIZE to each of the x 
    // values
    if (x < (width - 1) && !dungeon[x + 1][y].is_wall) {
      vector<vector<Point3D> > opposite_face2 = vector<vector<Point3D> >(FRACTAL_COMPLEXITY + 1);
      for (int i = 0; i < FRACTAL_COMPLEXITY + 1; i++) {
        opposite_face2[i] = vector<Point3D>(FRACTAL_COMPLEXITY + 1);
        for (int j = 0; j < FRACTAL_COMPLEXITY + 1; j++) {
          opposite_face2[i][j] = Point3D(face2[i][j][0] + PIECE_SIZE, face2[i][j][1], face2[i][j][2]);
        }
      }

      // Introduce randomness
      fractal(&opposite_face2, 0, FRACTAL_COMPLEXITY, 0, FRACTAL_COMPLEXITY, 0);
      dungeon[x][y].face4 = opposite_face2;
    }

    if (x > 0 && !dungeon[x - 1][y].is_wall) {
      // Introduce randomness into original face
      fractal(&face2, 0, FRACTAL_COMPLEXITY, 0, FRACTAL_COMPLEXITY, 0);
      dungeon[x][y].face2 = face2;
    }    
  }
}

void Dungeon::createDisplayLists() {
  // Create the display list for picking
  glNewList(DUNGEON_PICKING, GL_COMPILE);
  for (int i = 0 ; i < width; i++) {
    // Push the position of the block
    glPushName(i);
    for (int j = 0; j < height; j++) {
      glPushName(j);
      if (dungeon[i][j].is_wall) {
        glPushName(NAME_WALL);
        glPushMatrix();
        glTranslated(0, 0, 1);
        draw_cube(i, j, 0, 1);
        glPopMatrix();
        glPopName();
      }
      draw_floor(i, j, 1, 1);
      glPopName();
    }
    glPopName();
  }
  glEndList();

  has_display_list = true;
}

void Dungeon::initTexture() {
  wall_material = new TextureMaterial(ROCK_TEXTURE);
  has_texture = true;
}

// Get all the neighbours that we can traverse to from a point
// a vector to the diagonal is only valid if the corresponding points
// below and beside it are not walls either
vector<Point2D> Dungeon::getNeighbours(int x, int y) {
  vector<Point2D> ret_vector;

  // Left
  if (x > 0) {
    if (y > 0) {
      // To avoid overlap we can only go diagonal if the two neighbours are not walls
      if (!dungeon[x - 1][y - 1].is_wall &&
          (!dungeon[x - 1][y].is_wall && !dungeon[x][y - 1].is_wall)) {
        ret_vector.push_back(Point2D(x - 1, y - 1));
      }
    }

    if (!dungeon[x - 1][y].is_wall) {
      ret_vector.push_back(Point2D(x - 1, y));
    }

    if (y < height - 1) {
      if (!dungeon[x - 1][y + 1].is_wall &&
          (!dungeon[x - 1][y].is_wall && !dungeon[x][y + 1].is_wall)) {
        ret_vector.push_back(Point2D(x - 1, y + 1));
      }
    }
  }

  // Same column
  if (y > 0) {
    if (!dungeon[x][y - 1].is_wall) {
      ret_vector.push_back(Point2D(x, y - 1));
    }
  }

  if (y < height - 1) {
    if (!dungeon[x][y + 1].is_wall) {
      ret_vector.push_back(Point2D(x, y + 1));
    }
  }

  // Right
  if (x < width - 1) {
    if (y > 0) {
      // To avoid overlap we can only go diagonal if the two neighbours are not walls
      if (!dungeon[x + 1][y - 1].is_wall &&
          (!dungeon[x + 1][y].is_wall && !dungeon[x][y - 1].is_wall)) {
        ret_vector.push_back(Point2D(x + 1, y - 1));
      }
    }

    if (!dungeon[x + 1][y].is_wall) {
      ret_vector.push_back(Point2D(x + 1, y));
    }

    if (y < height - 1) {
      if (!dungeon[x + 1][y + 1].is_wall &&
          (!dungeon[x + 1][y].is_wall && !dungeon[x][y + 1].is_wall)) {
        ret_vector.push_back(Point2D(x + 1, y + 1));
      }
    }
  }

  return ret_vector;
}

vector<Point3D> Dungeon::pathfind(int x, int y, int xdest, int ydest) {
  vector<Point3D> ret_path;
  // If pathfinding is off we can quit early.  Woo!
  if (!settings.pathfinding) {
    ret_path.insert(ret_path.begin(), Point3D(xdest, ydest, 0));
    ret_path.insert(ret_path.begin(), Point3D(x, y, 0));
    return ret_path;
  }

  // Using a simple BFS search alg
  // The set of points we've seen
  set<Point2D, Point2DCompare> seen;
  // The nodes we'd like to examine
  queue<PathNode*> node_queue;
  // Node pointers for GC purposes
  vector<PathNode*> nodes_examined;
  // Using the PathNode structure to help keep track and backtrack when
  // we're done
  PathNode* start = new PathNode(Point2D(x, y), NULL);
  node_queue.push(start);
  nodes_examined.push_back(start);
  seen.insert(start->point);

  while (!node_queue.empty()) {
    PathNode* current_node = node_queue.front();
    node_queue.pop();

    if (current_node->point[0] == xdest && current_node->point[1] == ydest) {
      // We've found the point! Unwind the tree and form a path
      while(current_node->parent != NULL) {
        ret_path.insert(ret_path.begin(), Point3D(current_node->point[0], current_node->point[1], 0));
        current_node = current_node->parent;
      }
      for (int i = 0; i < nodes_examined.size(); i++) {
        delete nodes_examined[i];
      }

      return ret_path;

    }

    PathNode* neighbour;
    vector<Point2D> neighbours = getNeighbours((int)current_node->point[0], (int)current_node->point[1]);
    for (int i = 0; i < neighbours.size(); ++i) {
      if (!seen.count(neighbours[i])) {
        // If we haven't seen this neighbor, add it to the queue
        seen.insert(neighbours[i]);
        neighbour = new PathNode(neighbours[i], current_node);
        node_queue.push(neighbour);
        nodes_examined.push_back(neighbour);
      }
    }       
  }
  for (int i = 0; i < nodes_examined.size(); i++) {
    delete nodes_examined[i];
  }
  // Return an empty path: we couldn't find anything :(
  return ret_path;
}
