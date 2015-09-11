/*
 * Ian Milligan 
 * CS 488 Assignment 5
 */
#include "scene.hpp"
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::walk_gl(bool picking) const
{
  // Push the old matrix and multiply the new one
  glPushMatrix();
  glMultMatrixd(m_trans.transpose().begin());

  // Walk the children
  std::list<SceneNode*>::const_iterator iterator;
  for (iterator = m_children.begin(); iterator != m_children.end(); ++iterator) {
      (*iterator)->walk_gl(picking);
  } 

  glPopMatrix();
}

void SceneNode::rotate(char axis, double angle)
{
  double sin_angle = sin(angle * M_PI / 180);
  double cos_angle = cos(angle * M_PI / 180);
  Matrix4x4 rotate_matrix;
  switch (axis) {
    case 'x':
      rotate_matrix = Matrix4x4(
        Vector4D(1, 0, 0, 0),
        Vector4D(0, cos_angle, 0 - sin_angle, 0),
        Vector4D(0, sin_angle, cos_angle, 0),
        Vector4D(0, 0, 0, 1)
      );
    break;
    case 'y':
      rotate_matrix = Matrix4x4(
        Vector4D(cos_angle, 0, sin_angle, 0),
        Vector4D(0, 1, 0, 0),
        Vector4D(-sin_angle, 0, cos_angle, 0),
        Vector4D(0, 0, 0, 1)
      );
    break;
    case 'z':
      rotate_matrix = Matrix4x4(
        Vector4D(cos_angle, 0 - sin_angle, 0, 0),
        Vector4D(sin_angle, cos_angle, 0, 0),
        Vector4D(0, 0, 1, 0),
        Vector4D(0, 0, 0, 1)
      );
    break;
  }
  set_transform(m_trans * rotate_matrix);
}

void SceneNode::scale(const Vector3D& amount)
{
  Matrix4x4 scale_matrix = Matrix4x4(
    Vector4D(amount[0], 0, 0, 0),
    Vector4D(0, amount[1], 0, 0),
    Vector4D(0, 0, amount[2], 0),
    Vector4D(0, 0, 0, 1)
  );
  set_transform(m_trans * scale_matrix);  
}

void SceneNode::translate(const Vector3D& amount)
{
  Matrix4x4 translate_matrix = Matrix4x4(
    Vector4D(1, 0, 0, amount[0]),
    Vector4D(0, 1, 0, amount[1]),
    Vector4D(0, 0, 1, amount[2]),
    Vector4D(0, 0, 0, 1)
  );
  set_transform(m_trans * translate_matrix);
}

bool SceneNode::is_joint() const
{
  return false;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

void JointNode::walk_gl(bool picking) const
{ 
  // Push the matrix and multiply by the one specified.
  glPushMatrix();
  glMultMatrixd(m_trans.transpose().begin());

  // Using opengl for rotations
  glRotated(m_joint_x.current, 1, 0, 0);
  glRotated(m_joint_y.current, 0, 1, 0);

  // Walk the children
  std::list<SceneNode*>::const_iterator iterator;
  for (iterator = m_children.begin(); iterator != m_children.end(); ++iterator) {
      (*iterator)->walk_gl(picking);
  }

  glPopMatrix();
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
  m_joint_x.current = init;
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
  m_joint_y.current = init;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}

void GeometryNode::walk_gl(bool picking) const
{
  // Push the matrix and multiply by the one specified.
  glPushMatrix();
  glMultMatrixd(m_trans.transpose().begin());

  if (!picking) {
    // Turn off shading while picking
    m_material->apply_gl();
  }
  
  // Walk the primative
  m_primitive->walk_gl(picking);

  if (!picking) {
    m_material->end_gl();
  }
  
  // Walk the children
  std::list<SceneNode*>::const_iterator iterator;
  for (iterator = m_children.begin(); iterator != m_children.end(); ++iterator) {
      (*iterator)->walk_gl(picking);
  }

  glPopMatrix();
}
 
