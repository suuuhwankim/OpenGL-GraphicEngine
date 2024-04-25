/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ObjLoader.h
Purpose: To read the obj files.
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_1
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 9/21/2021
End Header --------------------------------------------------------*/

#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

struct OBJ
{

	OBJ(void) 
	: vertices(0), indices(0),
	  vertexCount(0), indexCount(0),
	  max(glm::vec3(1.0f)),
	  midPoint(glm::vec3(0.0f))

	{}

	OBJ(std::vector<glm::vec3>& vtx, std::vector<glm::ivec3>& idx,
		std::vector<glm::vec3>& vn, std::vector<glm::vec3>& fn)
	: vertices(vtx), indices(idx),
	  vertexNormal(vn), faceNormal(fn),
	  vertexCount(0), indexCount(0),
	  max(glm::vec3(1.0f)),
	  midPoint(glm::vec3(0.0f))

	{}
	std::vector<glm::vec3>        vertices;
	std::vector<glm::ivec3>       indices;	
	std::vector<glm::vec3>		  vertexNormal;
	std::vector<glm::vec3>		  faceNormal;
	std::vector<glm::vec3>		  pureVertexNormal;
	std::vector<glm::vec3>		  pureFaceNormal;
	glm::vec3                     max;
	glm::vec3					  midPoint;
	unsigned int vertexCount;
	unsigned int indexCount;
};


OBJ* LoadOBJFromFile(const std::string& filename);
OBJ* LoadSphere(int LOD, float radius);
#endif