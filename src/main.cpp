/*
 * Ian Milligan 
 * CS 488 Assignment 5
 */
#include <GL/glew.h>
#include <GL/glut.h>
#include <SDL/SDL.h>
#include "sounds.hpp"
#include "image.hpp"
#include "dungeon.hpp"
#include "constants.hpp"
#include "scene_lua.hpp"
#include <iostream>
#include <sys/time.h>
#include <climits>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>
#include "settings.hpp"

using namespace std;

Dungeon dungeon; // The dungeon containing most of the objects
char * dungeon_file; // The location of the dungeon file
int sword_sound; // The id of the sword sound
int rotation; // Current rotation of the board
int zoom; // Current zoom level of the board

// Setup the gl
int init_gl()
{
  Uint32 flags;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 ) {
    cerr << "Couldn't init gl: " << SDL_GetError() << endl;
    return 1;
  }

  flags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE;
  // flags |= SDL_FULLSCREEN;

  if (SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 0, flags) == NULL) {
    cerr << "Couldn't set video mode: " << SDL_GetError() << endl;
    return 1;
  }

  SDL_WM_SetCaption("Cavern Caretaker", "Cavern Caretaker");

  // Reset The Current Viewport
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); 

  glMatrixMode(GL_PROJECTION);          
  glLoadIdentity();                 

  // Setup the perspective
  gluPerspective(40.0, (GLfloat)WINDOW_WIDTH/(GLfloat)WINDOW_HEIGHT, 0.1, 1000.0);

  glMatrixMode(GL_MODELVIEW);             
  glLoadIdentity();          

  return 0;
}


// Setup the dungeon.  This is also called when we reset things
int init_dungeon() {
  rotation = 0;
  zoom = ZOOM;

  SceneNode* player = import_lua(PLAYER_FILE);
  if (!player) {
    std::cerr << "Could not open " << PLAYER_FILE << std::endl;
    return 1;
  }

  // Create a dungeon and give it information about how to create
  // the player and enemies
  dungeon = Dungeon();
  dungeon.addPlayerModel(player, sword_sound);
  dungeon.addEnemyModel(ENEMY_FILE);
  dungeon.loadFromFile(dungeon_file);
  return 0;
}

// Draw the game, without any opengl setup
void draw_game(bool picking = false) {
  // move the scene 
  glTranslated(0, 0, -zoom);
  
  // Setup to get the nice dungeon crawler look
  glRotated(-60, 1, 0, 0);
  glRotated(45, 0, 0, 1);

  // Rotate the board
  glRotated(rotation, 0, 0, 1);

  dungeon.runGL(picking); 
}

// Call the proper opengl setup (including fog and lighting)
void perform_gl() {

  // Clear the stencil and depth buffers
  glClearStencil(0); 
  glClearDepth(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Clear the color to a red-tinted grey
  glClearColor(0.1f, 0.05f, 0.05f, 0.0f);
  
  // Enable lights if they are on
  if (settings.lights) {
    glEnable(GL_LIGHTING);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1);
  } else {
    glDisable(GL_LIGHTING);
  }

  // Other assorted bits
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glLoadIdentity();

  // If fog is enabled start it
  if (settings.mist) {
    // Same colour as our background
    float fog_colour[4] = {0.1f,0.05f,0.05f, 0.0f};
   
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, fog_colour);
    glFogf(GL_FOG_DENSITY, 0.1f);
    glHint(GL_FOG_HINT, GL_DONT_CARE);
    glFogf(GL_FOG_START, -1.0f);
    glFogf(GL_FOG_END, 5.0f);

    // Actually enable it
    glEnable(GL_FOG);
  } else {
    glDisable(GL_FOG);
  }
  
  // Draw the game itself
  draw_game(false);

  // Swap the buffers
  glFlush();
  SDL_GL_SwapBuffers();
}

// Process the hit results from picking
void process_hit_results(int hits, unsigned int * picking_buffer) {
  int i = 0;
  // Find the first result
  while (i < hits) {
    unsigned int q = picking_buffer[i];
    unsigned int min_z = picking_buffer[i + 1];
    unsigned int max_z = picking_buffer[i + 2];
    if (q >= 3) {
      // The dungeon was hit (cavern structure)
      if (picking_buffer[i + 3] == NAME_DUNGEON) {
        // The row and column should be stored
        int row = picking_buffer[i + 3 + 1];
        int column = picking_buffer[i + 3 + 2];
        if (q == 4) {
          // A wall was hit
        } else {
          // Move - this is a movement action
          dungeon.moveTo(row, column);
        }
      }
    } else if (q >= 2 && picking_buffer[i + 3] == NAME_ENEMY) {
      // We've hit an enemy.  Target him.
      int number = picking_buffer[i + 3 + 1];
      dungeon.target(number);
    }
    i += 3 + q;
  }
}

// Actually perform the picking (using the x and y)
void perform_picking(int x, int y) {
  // Create the buffer
  int * viewport = new int[16];
  int buffer_size = 512;
  unsigned int * picking_buffer = new unsigned int[512];
  glSelectBuffer(buffer_size, picking_buffer);
  glRenderMode(GL_SELECT);
  glInitNames();
  glPushName(-1);
  glGetIntegerv(GL_VIEWPORT, viewport); // Setup pick view
  
  // Setup the same perspective and translations as our viewing mode
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluPickMatrix(x, WINDOW_HEIGHT - y, 1, 1, viewport);
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  gluPerspective(40.0, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1, 1000.0);
    
  // Back into MODELVIEW.  Apply transforms
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  draw_game(true);
  
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  
  // Grab the number of hits and process them
  int hits = glRenderMode(GL_RENDER);
  process_hit_results(hits, picking_buffer);
}

// Handle some keydown action (move the perspective around)
void handle_key_down(SDL_KeyboardEvent key) {
  // Switch on the key
  switch(key.keysym.sym) {
    case SDLK_RIGHT:
      rotation += ROTATION_INCREMENT;
    break;
    case SDLK_LEFT:
      rotation -= ROTATION_INCREMENT;
    break;
    case SDLK_UP:
      zoom = CLAMP(zoom - ZOOM_INCREMENT, ZOOM_MIN, ZOOM_MAX);
    break;
    case SDLK_DOWN:
      zoom = CLAMP(zoom + ZOOM_INCREMENT, ZOOM_MIN, ZOOM_MAX);
    break;
    default:
    break;
  }
}

// Handle some keyup action (adjusting a setting)
void handle_key_up(SDL_KeyboardEvent key) {
  switch(key.keysym.sym) {
    case SDLK_m:
      settings.toggleMist();
    break;
    case SDLK_l:
      settings.toggleLights();
    break;
    case SDLK_f:
      settings.toggleFractals();
    break;
    case SDLK_t:
      settings.toggleTextures();
    break;
    case SDLK_p:
      settings.togglePathfinding();
    break;
    case SDLK_s:
      settings.toggleSound();
      // Make sure to stop both music and sound
      if (settings.sound) {
        // We don't resume sounds.  They are assumed to be short enough
        // so as to be trigged again.
        sounds.resumeMusic();
      } else {
        sounds.pauseMusic();
        sounds.stopAllSounds();
      }
    break;
    case SDLK_r:
      init_dungeon();
    break;
    default:
    break;
  }
}

// Handle the mouseup events.  This should just init picking
void handle_mouse_buttons(SDL_MouseButtonEvent button) {
  if (button.type == SDL_MOUSEBUTTONUP){
    perform_picking(button.x, button.y);
  }
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    dungeon_file = argv[1];
    cerr << "Dungeon file: " << dungeon_file << endl;
  } else {
    dungeon_file = DUNGEON_FILE;
  }

  // Load in the sounds
  int music = sounds.loadMusic(BACKGROUND_MUSIC);
  sword_sound = sounds.loadSound(SWORD_SOUND); 
  srand(time(NULL));

  if (init_gl() != 0) {
    return 1;
  } 
  if (init_dungeon() != 0) {
    return 1;
  }
  sounds.playMusic(music);

  // This is the main loop for the entire program and it will run until done==TRUE
  bool repaint = true;
  bool run = true;

  while(run){
    Uint32 start_time = SDL_GetTicks();

    // Get the next event
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_KEYDOWN:
          handle_key_down(event.key);
          repaint = true;
        break;
        case SDL_KEYUP:
          handle_key_up(event.key);
          repaint = true;
        break;
        case SDL_MOUSEBUTTONUP:
          handle_mouse_buttons(event.button);
          repaint = true;
        break;
        case SDL_QUIT:
          // We're done and we'll end this program
          run = false;
        break;
        default:
        break;
      }

    }

    // Check if ESCAPE has been pressed
    if (SDL_GetKeyState(NULL)[SDLK_ESCAPE]) {
      run = false;
    }

    // Handle the logic in the game
    repaint |= dungeon.tick();

    if (repaint) {
      perform_gl();
      repaint = false;
    }

    // Sleep until the next time we should repaint
    if ((1000 / FPS) > (SDL_GetTicks() - start_time)) {
       SDL_Delay((1000 / FPS) - (SDL_GetTicks() - start_time));
    }

  }

  SDL_Quit();
  return 0;
}
