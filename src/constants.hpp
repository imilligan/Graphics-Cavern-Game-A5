/*
 * Ian Milligan 
 * CS 488 Assignment 5
 */
#ifndef CS488_CONSTANTS_HPP
#define CS488_CONSTANTS_HPP

// For call lists
#define DUNGEON 2
#define DUNGEON_PICKING 3

#define FPS 20
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Generic epsilon used in calculations
#define EPSILON 0.00001

// The speed that game beings walk
#define WALK_SPEED 0.25

// Parameters for the board
#define ZOOM 8
#define ROTATION_INCREMENT 3
#define ZOOM_INCREMENT 2
#define ZOOM_MIN 5
#define ZOOM_MAX 50

// The joint speed (how fast arms move)
#define JOINT_SPEED 8
#define ATTACK_SPEED 8

// The size of a game peice
#define PIECE_SIZE 1.0
#define FRACTAL_EPSILON 0.25
#define FRACTAL_COMPLEXITY 8

// Interactions between beings
#define ENEMY_LIFE 20
#define HIT_STRENGTH 5
#define DIEING_LENGTH 150
#define FLEE_DISTANCE 2
#define ATTACK_DISTANCE 1.5

// The number of blocks to draw around the player
#define DRAW_DISTANCE 10

// Music-related constants
#define MAX_SOUNDS 100
#define MAX_MUSIC 100
#define AUDIO_RATE 22050
#define AUDIO_CHANNELS 2
#define AUTO_BUFFERS 4096

// Dungeon names (for picking)
#define NAME_DUNGEON 1
#define NAME_WALL 2
#define NAME_ENEMY 3

// The default file names
static char DUNGEON_FILE[] = "data/dungeon.png";
static char PLAYER_FILE[] = "data/player.lua";
static char ENEMY_FILE[] = "data/enemy.lua";
static char SWORD_SOUND[] = "data/sword.wav";
static char BACKGROUND_MUSIC[] = "data/background.mp3";
static char ROCK_TEXTURE[] = "data/detailed-rock.png";

#endif
