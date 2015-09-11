/*
 * Ian Milligan 
 * CS 488 Assignment 5
 */
#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include "material.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual void walk_gl(bool picking) = 0;
};

class Sphere : public Primitive {
private:
  void create_display_list();
  // Using one display list for all spheres
  static unsigned int display_list_index;
  // Flag for whether or not we've create it
  static bool display_list_created;

public:
  virtual ~Sphere();
  virtual void walk_gl(bool picking);
};

// Adding a new primative: the box: draws a unit box a 0, 0, 0
class Box : public Primitive {
public:
  virtual ~Box();
  virtual void walk_gl(bool picking);
};

#endif
