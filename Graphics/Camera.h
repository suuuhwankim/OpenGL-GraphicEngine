/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.h
Purpose: To watch the clip space and change the sight.
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_2
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 10/30/2021
End Header --------------------------------------------------------*/

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

struct GLFWwindow;
class Camera
{
public:
	Camera(GLFWwindow* window, int width, int height);
	Camera(GLFWwindow* window, const glm::vec3& dir, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));
	~Camera();

	void Initialize(GLFWwindow* window, int width, int height);

	const glm::mat4& GetViewMatrix();

	glm::vec3& GetPosition();

	void Move(const glm::vec3& dir, float speed);
	void SetPosition(const glm::vec3& pos);
	void ControlView(float mouseSpeed, float dt);

private:
	GLFWwindow* window;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;
	glm::vec3 right;
	glm::mat4 view;

	float hAngle;
	float vAngle;
	int	  width, height;
};

#endif