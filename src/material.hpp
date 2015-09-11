/*
 * Ian Milligan 
 * CS 488 Assignment 5
 */
#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"
#include "image.hpp"

class Material {
public:
  virtual ~Material();
  virtual void apply_gl() = 0;
  virtual void end_gl() = 0;

protected:
  Material()
  {
  }
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess);
  virtual ~PhongMaterial();

  virtual void apply_gl();
  virtual void end_gl();

private:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;
};

// Texture material, to be applied to objects
class TextureMaterial : public Material {
public:
  TextureMaterial(const char* filename);
  virtual ~TextureMaterial();

  virtual void apply_gl();
  virtual void end_gl();
  
protected:
  // Actually register the texture with opengl
  void setup_texture();
private:
  float *data;
  bool has_setup;
  // Dimensions of the data
  int width;
  int height;
  unsigned int texture_map_index;
};


#endif
