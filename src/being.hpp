/*
 * Ian Milligan 
 * CS 488 Assignment 5
 */
#ifndef CS488_BEING_HPP
#define CS488_BEING_HPP

#include "scene_lua.hpp" 
#include "algebra.hpp"
#include "constants.hpp"
#include <vector>
#include <GL/gl.h>
#include <GL/glu.h> 

// A struct describing a figure
// This is used for moving feet and hands while walking
struct Anatomy {
  bool legs_direction;
  JointNode* left_leg;
  JointNode* right_leg;
  bool arms_direction;
  JointNode* left_arm;
  JointNode* right_arm;
  bool attack_direction;   
};

// A class for managing a character in the game.  Contains infomation about
// pathfinding, pointing and moving the joints
class Being {
public: 

  Being(SceneNode* gl, int sound = -1);
  ~Being();

  // Draw the figure
  void runGL(bool picking = false);
  // Point the figure at a specific point
  void point(Point2D aimpoint);
  // Advance all animations, returning true if we need to
  // repaint
  bool tick();
  // Hit some other being
  bool hit(Being * sender);

  // Exposing private members
  Point3D getPosition() {
    return position;
  }
  void setPosition(Point3D position) {
    this->position = position;
  }
  void setPath(std::vector<Point3D> path) {
    this->path = path;
  }
  void setTarget(Being* target) {
    this->target = target;
  }
  Being *getTarget() {
    return target;
  }
  Being *getFleeing() {
    return fleeing;
  }

protected:
  // Move the joints one step
  void move_joints();
  // Move the attach arm one step
  void attack();
  // Reset all joints to their inital positions
  void reset_joints();

private:
  // The angle that the figure is pointing
  double angle;
  // The root node of the figure
  SceneNode* gl;
  // The position of the figure
  // Using a 3D point here to make calculations simple
  Point3D position;
  // Path currently following
  std::vector<Point3D> path;
  // The structure of the being, as described earlier
  Anatomy structure;
  // The target we are chasing
  Being* target;
  // The target we are fleeing from
  Being* fleeing;
  // A pointer to the "Cling" sound
  int swordSound;
  // The amount of life left before the being dies
  int life;
  // Whether or not the being is dead: 0 == dead, -1 == living, > 0 dying
  int dying;
};

#endif