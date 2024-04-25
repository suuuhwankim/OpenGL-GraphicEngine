/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.cpp
Purpose: To watch the clip space and change the sight.
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_2
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 10/30/2021
End Header --------------------------------------------------------*/

#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glfw3.h>

namespace 
{
	const float PI = 3.141592f;
}

Camera::Camera(GLFWwindow* window, int width, int height) : 
	window(window),
	position(0.0f),
	direction(0.0f),
	up(0.0f),
	view(0.0f),
	hAngle(PI),
	vAngle(0.0f),
	width(width),
	height(height)
{
}

Camera::Camera(GLFWwindow* window, const glm::vec3& dir, const glm::vec3& up) :
	window(window), position(0.0f), direction(dir), up(up), view(0.0f),
hAngle(glm::radians(90.0f)),
vAngle(0.0f),
width(1.0f),
height(1.0f)
{
}

Camera::~Camera()
{

}

void Camera::Initialize(GLFWwindow* window, int width, int height)
{
	this->window = window;
	this->width = width;
	this->height = height;
}

const glm::mat4& Camera::GetViewMatrix() 
{	
	view = glm::lookAt(
			position,
			position + direction,
			up
		);

	return view;
}

glm::vec3& Camera::GetPosition()
{
	return position;
}

void Camera::SetPosition(const glm::vec3& pos)
{
	position = pos;
}

void Camera::Move(const glm::vec3& dir, float speed)
{
	position += (dir * speed);
}

void Camera::ControlView(float mouseSpeed, float dt)
{
	double x, y;

	glfwGetCursorPos(window, &x, &y);

	hAngle += mouseSpeed * dt * (width / 2 - static_cast<float>(x));
	vAngle += mouseSpeed * dt * (height / 2 - static_cast<float>(y));

	direction = glm::vec3(
		cos(vAngle) * sin(hAngle),
		sin(vAngle),
		cos(vAngle) * cos(hAngle)
	);

	right = glm::vec3(
		sin(hAngle - (PI / 2.f)),
		0,
		cos(hAngle - (PI / 2.f))
	);

	up = glm::cross(right, direction);
}