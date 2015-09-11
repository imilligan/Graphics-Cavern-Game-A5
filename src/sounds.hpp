/*
 * Ian Milligan 
 * CS 488 Assignment 5
 */
#ifndef CS488_SOUND_HPP
#define CS488_SOUND_HPP

#include "SDL.h"
#include "SDL_mixer.h"
#include "constants.hpp"

// Simple class for managing sound.
// Based off of work by Tiberiu Popa in project included in A5 directory
class Sounds {
public:
  Sounds();
  ~Sounds();

  // Adjust sound clips
  // Multiple sound clips can be played simultaneously
  int loadSound(char* file);

  // Sound is the id returned by loadSound
  int playSound(int sound);
  int stopAllSounds();  

  // Adjust sound in background (music)
  // Only one plays at a time    
  int loadMusic(char* file);
  int playMusic(int sound);
  int pauseMusic();
  int resumeMusic();

protected:
  int stopSound(int sound);

private:
  // Clips
  Mix_Chunk * chunks[MAX_SOUNDS];
  Mix_Music * music[MAX_MUSIC];

  // Channels for active sounds
  int channel[MAX_SOUNDS];

  // Number of music and sound clips
  int nSounds;
  int nMusics;
};

// Globally referenceable instance of sound manager
// is accessed by main and being
extern Sounds sounds;

#endif