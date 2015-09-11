/*
 * Ian Milligan 
 * CS 488 Assignment 5
 */
#include "primitive.hpp"
#include "cube.hpp"
#include <GL/gl.h>
#include <GL/glu.h>

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

void Sphere::walk_gl(bool picking)
{
  (void)picking;

  GLUquadricObj * quad = gluNewQuadric();
  gluQuadricNormals(quad, GLU_SMOOTH);
  gluQuadricTexture(quad, GL_TRUE); 

  gluSphere(quad, 1, 20, 20);
  gluDeleteQuadric(quad);

}

Box::~Box()
{
}

void Box::walk_gl(bool picking) {

  draw_cube(0, 0, 0, 1);
} 
