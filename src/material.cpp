/*
 * Ian Milligan 
 * CS 488 Assignment 5
 */
#include "material.hpp"
#include "settings.hpp"
#include <GL/gl.h>
#include <GL/glu.h>

#define DEFAULT_DIFFUSE 0.5
#define DEFAULT_SPECULAR 0.1
#define DEFAULT_SHININESS 1.0

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
{
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::apply_gl()
{
  	// Perform OpenGL calls necessary to set up this material.
	float* specular = new float[4];
	specular[0] = (float)m_ks.R();
	specular[1] = (float)m_ks.G();
	specular[2] = (float)m_ks.B();
	specular[3] =  1.0;
	float* diffuse = new float[4];
	diffuse[0] = (float)m_kd.R();
	diffuse[1] = (float)m_kd.G();
	diffuse[2] = (float)m_kd.B();
	diffuse[3] =  1.0;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, (float)m_shininess);

}

void PhongMaterial::end_gl() {
  // Remove the material
	float* specular = new float[4];
	specular[0] = DEFAULT_SPECULAR;
	specular[1] = DEFAULT_SPECULAR;
	specular[2] = DEFAULT_SPECULAR;
	specular[3] =  1.0;
	float* diffuse = new float[4];
	diffuse[0] = DEFAULT_DIFFUSE;
	diffuse[1] = DEFAULT_DIFFUSE;
	diffuse[2] = DEFAULT_DIFFUSE;
	diffuse[3] =  1.0;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, DEFAULT_SHININESS);
}

TextureMaterial::TextureMaterial(const char * filename) 
	:	has_setup(false) {

  Image wall_image = Image();
  if (!wall_image.loadPng(filename)) {
  	std::cerr << "Error loading texture file" << std::endl;
  }

  // Turn the texture into a useful format
  float *pixels = (float*)malloc(sizeof(float) * wall_image.width() * wall_image.height() * 3);
  for (int i = 0; i < wall_image.width(); i++) {
    for (int j = 0; j < wall_image.height(); j++) {
      pixels[(i * wall_image.width() * 3) + (3 * j)] = (float)wall_image(i, j, 0);
      pixels[(i * wall_image.width() * 3) + (3 * j) + 1] = (float)wall_image(i, j, 1);
      pixels[(i * wall_image.width() * 3) + (3 * j) + 2] = (float)wall_image(i, j, 2);
    }
  }

  width = wall_image.width();
  height = wall_image.height();
  data = pixels;	
}

TextureMaterial::~TextureMaterial() {

}

void TextureMaterial::setup_texture() {
	glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &texture_map_index);
  glBindTexture(GL_TEXTURE_2D, texture_map_index);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  // Tell OpenGl to wrap the texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  // Give it the txture
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, data);
  glDisable(GL_TEXTURE_2D);

  delete data;
	has_setup = true;
}

void TextureMaterial::apply_gl() {
  // First we need to load in the texture
	if (!has_setup) {
		setup_texture();
	}

  // Only show if textures are enabled
  if (settings.textures) {
	  glEnable(GL_TEXTURE_2D);
	  glBindTexture(GL_TEXTURE_2D, texture_map_index);
  }
}

void TextureMaterial::end_gl() {
  if (settings.textures) {
    glDisable(GL_TEXTURE_2D);
  }
}

