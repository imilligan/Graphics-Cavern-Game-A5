/*
 * Ian Milligan 
 * CS 488 Assignment 5
 */
#include "sounds.hpp"
#include "constants.hpp"
#include <stdlib.h>
#include <iostream>
#include "settings.hpp"

Sounds sounds;

Sounds::Sounds() {
  nSounds = 0;
  nMusics = 0;

  // Initialize SDL
  SDL_Init(SDL_INIT_AUDIO);

  // Open audio stream
  if (Mix_OpenAudio(AUDIO_RATE, AUDIO_S16, AUDIO_CHANNELS, AUTO_BUFFERS)) {
    std::cerr << "Unable to open audo" << std::endl;
    exit(1);
  }
}

Sounds::~Sounds() { }

int Sounds::loadSound(char* file){
  chunks[nSounds] = Mix_LoadWAV(file);
  channel[nSounds] = -1;

  nSounds++;
  return nSounds - 1;
}

int Sounds::playSound(int sound){
  if (sound < 0 || sound >= nSounds || !settings.sound) {
    return -1;
  }
  channel[sound] = Mix_PlayChannel(-1, chunks[sound], 0 /*-1*/);
  return 0;
}

int Sounds::stopSound(int sound){
  if (sound < 0 || sound >= nSounds) {
    return -1;
  }
  Mix_HaltChannel(channel[sound]);
  channel[sound] = -1;
  return 0;
}

int Sounds::stopAllSounds() {
  for (int i = 0; i < nSounds; i++) {
    stopSound(i);
  }
}

int Sounds::loadMusic(char* file) { 
  music[nMusics] = Mix_LoadMUS(file);

  if (music[nMusics] == NULL) {
    std::cerr << "Error loading music: " << SDL_GetError() << std::endl;
  }

  nMusics += 1;
  return nMusics - 1;
}

int Sounds::playMusic(int sound){
  if (sound < 0 || sound >= nMusics) {
    return -1;
  }
  Mix_PlayMusic(music[sound], -1);
  return 0;
}

int Sounds::pauseMusic(){
  Mix_PauseMusic();
  return 0;
}

int Sounds::resumeMusic(){
  Mix_ResumeMusic();
  return 0;
}
