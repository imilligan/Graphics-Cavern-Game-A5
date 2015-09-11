/*
 * Ian Milligan 
 * CS 488 Assignment 5
 */
#include "being.hpp"
#include <list>
#include <queue>
#include "sounds.hpp"

// Walk the tree string matching for joints
// Pass by reference used
bool searchForJoints(SceneNode* root, Anatomy* ret_anatomy) {
  // Flatten the tree
  std::list<JointNode*> nodes;
  std::queue<SceneNode*> examining;
  examining.push(root);
  while(!examining.empty()) {
    SceneNode* current_node = examining.front();
    examining.pop();

    std::list<SceneNode*> childen = current_node->get_children();
    for (std::list<SceneNode*>::const_iterator iterator = childen.begin(); iterator != childen.end(); ++iterator) {
      examining.push(*iterator);
    }
    // Only add the joints
    if (current_node->is_joint()) {
      nodes.push_back((JointNode*)current_node);
    }
  }

  for (std::list<JointNode*>::const_iterator iterator = nodes.begin(); iterator != nodes.end(); ++iterator) {
    if ((*iterator)->get_name() == "left_hip_jt") {
      (*ret_anatomy).left_leg = (*iterator);
    } if ((*iterator)->get_name() == "right_hip_jt") {
      (*ret_anatomy).right_leg = (*iterator);
    } if ((*iterator)->get_name() == "left_shoulder_jt") {
      (*ret_anatomy).left_arm = (*iterator);
    } if ((*iterator)->get_name() == "right_shoulder_jt") {
      (*ret_anatomy).right_arm = (*iterator);
    }
  }

  // Bools to keep track of what direction we're moving in (back or forth)
  (*ret_anatomy).legs_direction = true;
  (*ret_anatomy).arms_direction = false;
  (*ret_anatomy).attack_direction = true;

  // Return true if everything is present
  return (*ret_anatomy).left_leg != NULL &&
         (*ret_anatomy).right_leg != NULL &&
         (*ret_anatomy).left_arm != NULL &&
         (*ret_anatomy).right_arm != NULL;
}

Being::Being(SceneNode* gl, int sound)
  : target(NULL), fleeing(NULL), life(ENEMY_LIFE), dying(-1) {
  this->gl = gl;
  this->swordSound = sound;
  // Search for joints
  if (!searchForJoints(this->gl, &this->structure)) {
    std::cerr << "The joints was not found" << std::endl;
  }
}

Being::~Being() { }

// Actually draw the being
void Being::runGL(bool picking) {
  if (dying == 0) {
    return;
  } else if (dying > 0) {
    // Sink into the ground if we are dying
    glTranslated(0, -3.0 * (1.0 - ((float)dying / DIEING_LENGTH)), 0);
  }

  // Rotate to the correct direction
  glRotated(90, 0, 1, 0);
  glRotated(this->angle, 0, 1, 0);

  gl->walk_gl(picking);
}

// Point the figure at a point (relative to the location)
// of the being (very important to remember)
void Being::point(Point2D point) {
  double radians;
  if (point[1] > 0) {
    if (point[0] > 0) {
      radians = atan2((float)(point[1]), (float)(point[0]));  
    } 
    else if (point[0] == 0) {
      radians = M_PI / 2;
    } else {
      radians = M_PI - atan2((float)(point[1]), (float)(-point[0])); 
    } 
  } else if (point[1] == 0) {
    if (point[0] >= 0) {
      radians = 0; 
    } else {
      radians = M_PI;
    } 
  } else {
    if (point[0] > 0) {
      radians = (2 * M_PI) - atan2((float)(-point[1]), (float)(point[0]));  
    } else if (point[0] == 0) {
      radians = 3 * M_PI / 2;
    } else {
      radians = M_PI + atan2((float)(-point[1]), (float)(-point[0])); 
    } 
  }
  this->angle = radians * 180 / M_PI;
}

// Move joints (as if we are walking)
void Being::move_joints() {
  // Handle the legs, moving them in opposite directions
  JointRange left_leg_joint = structure.left_leg->get_x_range();
  JointRange right_leg_joint = structure.right_leg->get_x_range();
  if (left_leg_joint.current == left_leg_joint.max ||
    left_leg_joint.current == left_leg_joint.min ||
    right_leg_joint.current == right_leg_joint.max ||
    right_leg_joint.current == right_leg_joint.min) {
    // Switch directions
    structure.legs_direction = !structure.legs_direction;
  }

  // Actually move them
  int leg_increment;
  if (structure.legs_direction) {
    leg_increment = JOINT_SPEED;
  } else {
    leg_increment = -JOINT_SPEED;
  }
  structure.left_leg->set_x_rotation(left_leg_joint.current + leg_increment);
  structure.right_leg->set_x_rotation(right_leg_joint.current - leg_increment);

  // Handle the arms, moving them in opposite directions
  JointRange left_arm_joint = structure.left_arm->get_x_range();
  JointRange right_arm_joint = structure.right_arm->get_x_range();
  if (left_arm_joint.current == left_arm_joint.max ||
    left_arm_joint.current == left_arm_joint.min ||
    right_arm_joint.current == right_arm_joint.max ||
    right_arm_joint.current == right_arm_joint.min) {
    // Switch directions
    structure.arms_direction = !structure.arms_direction;
  }

  // Actually move them
  int arm_increment;
  if (structure.arms_direction) {
    arm_increment = JOINT_SPEED;
  } else {
    arm_increment = -JOINT_SPEED;
  }
  structure.left_arm->set_x_rotation(left_arm_joint.current + arm_increment);
  structure.right_arm->set_x_rotation(right_arm_joint.current - arm_increment);
}

// Move the joints (as id we are attacking)
// also hit the target and play a sound
void Being::attack() {
  // We will always attack with our right arm
  JointRange right_arm_joint = structure.right_arm->get_x_range();
  int rotation = right_arm_joint.current;
  if (right_arm_joint.current > -130) {
    rotation = -130;
    structure.attack_direction = true;
  }

  if (right_arm_joint.current < -160) {
    rotation = -160;
    structure.attack_direction = false;
    // On the down stroke hit the enemy
    if (swordSound != -1) {
      sounds.playSound(swordSound);
    }
    if (target->hit(this)) {
      // returns true if the enemy dies
      reset_joints();
      target = NULL;
      path.clear();
      return;
    }
  }

  // Actually move the arm
  int arm_increment;
  if (structure.attack_direction) {
    arm_increment = -ATTACK_SPEED;
  } else {
    arm_increment = ATTACK_SPEED;
  }
  structure.right_arm->set_x_rotation(rotation + arm_increment);
}

// Reset everything to our inital state
void Being::reset_joints() {
  JointRange left_leg_joint = structure.left_leg->get_x_range();
  JointRange right_leg_joint = structure.right_leg->get_x_range();

  structure.left_leg->set_x_rotation(left_leg_joint.init);
  structure.right_leg->set_x_rotation(right_leg_joint.init);

  JointRange left_arm_joint = structure.left_arm->get_x_range();
  JointRange right_arm_joint = structure.right_arm->get_x_range();

  structure.left_arm->set_x_rotation(left_arm_joint.init);
  structure.right_arm->set_x_rotation(right_arm_joint.init);
}

// Tick in the animation
bool Being::tick() {
  // If we are dying just continue to do so.
  if (dying > 0) {
    // Further our demise
    dying -= 3;
    return true;
  }

  // Check and see if we are near our target (if there is one)
  if (target != NULL) {
    double distance_to_enemy = (getPosition() - target->getPosition()).length();
    if (distance_to_enemy < ATTACK_DISTANCE) {
      // If we are within range, attack!
      point(Point2D(target->getPosition()[0] - position[0], target->getPosition()[1] - position[1]));
      attack();
      return true;
    }
  }

  // There is nothing left to do if we don't have a path
  if (path.size() == 0) {
    return false;
  }

  // Loop through all the path points that we can with the walk speed allocated
  // This is essentially our 't' value for the displacement function (x, y) = f(t)
  double remaining_distance = WALK_SPEED;
  while (remaining_distance != 0) {
    if (path.size() == 0) {
      reset_joints();
      return true;
    }

    // Pop off the top
    Point3D next_vertex = path.front();
    path.erase(path.begin());

    if (target != NULL) {
      double distance_to_enemy = (next_vertex - target->getPosition()).length();
      if (distance_to_enemy < 0.5) {
        // If we are close to our enemy just stop.  We'll attack next tick
        path.insert(path.begin(), next_vertex);
        // Do some logic here to attack the enemy
        reset_joints();
        return true;
      }
    }

    Vector3D travel = next_vertex - position;
    if (travel.length() < EPSILON) {
      continue;
    } else if (travel.length() < remaining_distance) {
      position = next_vertex;
      remaining_distance = remaining_distance - travel.length();
    } else {
      travel.normalize();
      position = position + (WALK_SPEED * travel);
      remaining_distance = 0;
      path.insert(path.begin(), next_vertex);
    }
  }

  // Point towards the next place we are travelling to
  Point3D next_point = path.front();
  if ((next_point - position).length() > EPSILON) {
    point(Point2D(next_point[0] - position[0], next_point[1] - position[1]));
  } 

  // Move the joints
  move_joints();
  return true;
}

// Get hit by something
bool Being::hit(Being * sender) {
  if (life > 0) {
    // We losing life, so flee
    life -= HIT_STRENGTH;
    fleeing = sender;
  }
  
  if (life <= 0 && dying == -1) {
    dying = DIEING_LENGTH;
    return true;
  }

  return false;
}
