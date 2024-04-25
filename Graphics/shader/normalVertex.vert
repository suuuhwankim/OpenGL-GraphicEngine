/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: VertexShader.frag
Purpose: Vertex shader for Phong Shading
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_2
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 10/01/2021
End Header --------------------------------------------------------*/

#version 430 core

layout(location = 0) in vec3 modelPosition;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	mat4 mvp = proj * view * model;
	gl_Position = mvp * vec4(modelPosition, 1.0);
}