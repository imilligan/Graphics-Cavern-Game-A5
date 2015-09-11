/*
 * Ian Milligan 
 * CS 488 Assignment 5
 */
#ifndef CS488_SETTINGS_HPP
#define CS488_SETTINGS_HPP

// The settings object for our game
class Settings {
public:
  Settings()
    : mist(true), fractals(true), textures(true), pathfinding(true), lights(true), sound(true) {

  }
  void toggleMist() {
    mist = !mist;
  }
  void toggleFractals() {
    fractals = !fractals;
  }
  void toggleTextures() {
    textures = !textures;
  }
  void togglePathfinding() {
    pathfinding = !pathfinding;
  }
  void toggleLights() {
    lights = !lights;
  }
  void toggleSound() {
    sound = !sound;
  }

  // Just fog
  bool mist;

  // Whether fractal terrain is on
  bool fractals;

  // Whether textures are enabled
  bool textures;

  // Whether pathfinding is enabled
  bool pathfinding;

  // Whether lights are functioning
  bool lights;

  // Whether sounds are played
  bool sound;
};

extern Settings settings;

#endif