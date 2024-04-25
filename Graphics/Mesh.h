/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.h
Purpose: To get a basic mesh
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_1
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 9/21/2021
End Header --------------------------------------------------------*/
#pragma once
#include <vector>
#include <glm/detail/type_vec3.hpp>

class Mesh
{
public:
	friend class OBJReader;

	Mesh();
	~Mesh();


	glm::vec3   getModelScale();
	glm::vec3   getModelBoundingSize();
	glm::vec3   getModelCentroid();
	glm::vec3   getCentroidVector(glm::vec3 vVertex);


	size_t VertexBufferSizeInCount();


	size_t IndicesBufferSizeInCount();


	size_t NormalBufferSizeInCount();


	size_t NormalDisplayBufferSizeInCount();

	size_t FaceNormalDisplayBufferSizeInCount();


	std::vector<glm::vec3> GetVertexBuffer();
	std::vector<glm::vec3> GetVertexNormal();

	float* VertexData();
	unsigned* IndicesData();
	float* NormalData();
	float* NormalDisplayData();
	float* FaceNormalDisplayData();


	void PutVertexData(glm::vec3 data);
	void PutIndicesData(unsigned data);
	void PutNormalData(glm::vec3 normal);

	void SetBoundingBox(int i, glm::vec3 val);
	void CalculateNormals();

private:
	void Clear();
	std::vector<glm::vec3> vertexBuffer;
	std::vector<unsigned> vertexIndices;
	std::vector<glm::vec3> vertexNormals;
	std::vector<glm::vec3> vertexNormalsDisplay;
	std::vector<glm::vec3> faceNormalsDisplay;
	std::vector<glm::vec2> vertexUVs;
	glm::vec3 boundingBox[2];
};