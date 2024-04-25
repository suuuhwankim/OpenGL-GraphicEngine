/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Object.cpp
Purpose: To create a object.
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_1
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 9/21/2021
End Header --------------------------------------------------------*/
#include "OBJLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>

#include "Object.h"

Object::Object(OBJ* obj, const glm::vec3& pos, const glm::vec3& scl, const glm::vec3& color)
	: obj(obj), mesh(nullptr), pos(pos), scl(scl), color(color), mat(1.0f), rotAxis(glm::vec3(0.0f, 1.0f, 0.0f))
	, rotDegree(0.0f)
{

}

Object::~Object()
{

}

void Object::Destroy()
{
	delete obj;
}

void Object::SetPosition(const glm::vec3& pos)
{
	this->pos = pos;
}
void Object::SetScale(const glm::vec3& scl)
{
	this->scl = scl;
}
void Object::SetColor(const glm::vec3& color)
{
	this->color = color;
}

void Object::SetRotation(float degree, const glm::vec3& axis)
{
	rotDegree = degree;
	rotAxis = axis;
}

glm::vec3& Object::GetPosition()
{
	return pos;
}

glm::vec3& Object::GetScale()
{
	return scl;
}

glm::vec3& Object::GetColor()
{
	return color;
}

glm::mat4& Object::GetObjectMatrix()
{
	glm::mat4 translate = glm::mat4(1.0f);
	glm::mat4 rotation  = glm::mat4(1.0f);
	glm::mat4 scaler    = glm::mat4(1.0f);

	scaler    = glm::scale(scl);
	rotation  = glm::rotate(glm::radians(rotDegree), rotAxis);
	translate = glm::translate(translate, pos);

	rot = rotation;
	mat = translate * rotation * scaler;

	return mat;
}

glm::mat4& Object::GetRotationMatrix()
{
	return rot;
}

OBJ* Object::GetOBJ()
{
	return obj;
}

Mesh* Object::GetMesh()
{
	return mesh;
}

void Object::SetOBJFile(OBJ& obj)
{
	this->obj = &obj;
}
