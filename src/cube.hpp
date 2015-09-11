/*
 * Ian Milligan 
 * CS 488 Assignment 5
 */
#ifndef CS488_DRAW_CUBE_HPP
#define CS488_DRAW_CUBE_HPP

// Generic cube drawing proc (with normals and texture)
static void draw_cube(double x, double y, double z, double size) {

  glBegin(GL_QUADS);

  // First face in xy plane 
  glNormal3d(0, 0, -1);
  glTexCoord2f (0.0, 0.0);
  glVertex3d(x, y, z);
  glTexCoord2f (1.0, 0.0);
  glVertex3d(x, y + size, z);
  glTexCoord2f (1.0, 1.0);
  glVertex3d(x + size, y + size, z);
  glTexCoord2f (0.0, 1.0);
  glVertex3d(x + size, y, z);

  // First face in xz plane
  glNormal3d(0, -1, 0);
  glTexCoord2f (0.0, 0.0);
  glVertex3d(x, y, z);
  glTexCoord2f (1.0, 0.0);
  glVertex3d(x + size, y, z);
  glTexCoord2f (1.0, 1.0);
  glVertex3d(x + size, y, z + size);
  glTexCoord2f (0.0, 1.0);
  glVertex3d(x, y, z + size);

  // First face in yz plane
  glNormal3d(-1, 0, 0);
  glTexCoord2f (0.0, 0.0);
  glVertex3d(x, y, z);
  glTexCoord2f (1.0, 0.0);
  glVertex3d(x, y, z + size);
  glTexCoord2f (1.0, 1.0);
  glVertex3d(x, y + size, z + size);
  glTexCoord2f (0.0, 1.0);
  glVertex3d(x, y + size, z);

  // Second face in xy plane
  glNormal3d(0, 0, 1);
  glTexCoord2f (0.0, 0.0);
  glVertex3d(x, y, z + size);
  glTexCoord2f (1.0, 0.0);
  glVertex3d(x + size, y, z + size);
  glTexCoord2f (1.0, 1.0);
  glVertex3d(x + size, y + size, z + size);
  glTexCoord2f (0.0, 1.0);
  glVertex3d(x, y + size, z + size);

  // First face in xz plane
  glNormal3d(0, 1, 0);
  glTexCoord2f (0.0, 0.0);  
  glVertex3d(x, y + size, z);
  glTexCoord2f (1.0, 0.0);
  glVertex3d(x + size, y + size, z);
  glTexCoord2f (1.0, 1.0);
  glVertex3d(x + size, y + size, z + size);
  glTexCoord2f (0.0, 1.0);
  glVertex3d(x, y + size, z + size);

  // First face in yz plane
  glNormal3d(1, 0, 0);
  glTexCoord2f (0.0, 0.0);  
  glVertex3d(x + size, y, z);
  glTexCoord2f (1.0, 0.0);
  glVertex3d(x + size, y, z + size);
  glTexCoord2f (1.0, 1.0);
  glVertex3d(x + size, y + size, z + size);
  glTexCoord2f (0.0, 1.0);
  glVertex3d(x + size, y + size, z);

  glEnd();
}

#endif