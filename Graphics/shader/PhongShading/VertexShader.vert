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
Creation date: 11/05/2021
End Header --------------------------------------------------------*/

#version 430 core

layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec3 normalVector;
layout(location = 2) in vec2 cpuUV;

out vec4 normal;
out vec3 fragPos;
out vec3 entity;
out vec2 uvs;
flat out int doCalc;

uniform mat4 rotationMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform int entity_mode;
uniform int isGpu;

void main()
{
	mat4 mvp = proj * view * model;
	gl_Position = mvp * vec4(modelPosition, 1.0);
	fragPos = vec3(model * vec4(modelPosition, 1.0));
	normal = rotationMatrix * vec4(normalVector, 1.0);

	if (isGpu == 1)
	{
		if (entity_mode == 1)
		{
			entity = normalize(modelPosition);
		}
		else if (entity_mode == 2)
		{
			entity = normalize(normal.xyz);
		}
	}
	else
	{
		uvs = cpuUV;
	}
	doCalc = isGpu;
}