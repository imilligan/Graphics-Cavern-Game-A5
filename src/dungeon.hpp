/*
 * Ian Milligan 
 * CS 488 Assignment 5
 */
#ifndef CS488_DUNGEON_HPP
#define CS488_DUNGEON_HPP

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <vector>
#include <queue>
#include <set>
#include "constants.hpp"
#include "algebra.hpp"
#include "scene_lua.hpp"
#include "being.hpp"
#include "material.hpp"
#include "cube.hpp"

// A dungeon peice (contains info about fractal walls)
struct Piece {
  bool is_wall;
  int display_list;
  std::vector<std::vector<Point3D> > face1;
  std::vector<std::vector<Point3D> > face2;
  std::vector<std::vector<Point3D> > face3;
  std::vector<std::vector<Point3D> > face4;
};

// The class describing the dungeon itself
class Dungeon {
public:
  Dungeon(); // Construct an empty dungeon

  ~Dungeon();

  // Load the structure of the dungeon from a file
  bool loadFromFile(char * file);
  // Add the description of the player (and what a sword sounds like) 
  void addPlayerModel(SceneNode* player, int sword_sound);
  // Add the description of the enemies
  void addEnemyModel(char* enemy_file);
  // Actually at an anemy to the array
  void addEnemy(double x, double y);
  // Draw various things
  void runGL(bool picking = false);
  // Move the dungeon (main player) to a location
  void moveTo(double x, double y);
  // From picking: target an enemy
  void target(int enemy_number);
  // Pathfind from a point to another point
  std::vector<Point3D>  pathfind(int x, int y, int xdest, int ydest);
  // Return whether or not to repaint
  bool tick();

protected:
  // Draw a wall (from a descriptor)
  void draw_wall(double x, double y, double z, Piece description, double size);
  // Create a wall peice, knowing the entire structure of the dungeon (to optimize)
  // what walls to fractal 
  void createPeice(int x, int y);
  // initiate the display lists in use
  void createDisplayLists();
  // Get the neighbours on the board
  std::vector<Point2D> getNeighbours(int x, int y);
  // Init the board's texture
  void initTexture();

private:
  // Are the display lists created
  bool has_display_list;
  // Is the texture initialized?
  bool has_texture;
  // Are lights setup?
  bool has_lights;
  // The board itself
  Piece ** dungeon;

  int width;
  int height;
  
  // Game objects
  Being *player;
  std::vector<Being *> enemies;
  
  // Models for the game objects
  SceneNode *playerModel;
  char *enemyFile;

  // Wall material
  Material *wall_material;  
};

#endif
