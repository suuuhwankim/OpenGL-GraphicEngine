/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.cpp
Purpose: To get a basic mesh
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_1
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 9/21/2021
End Header --------------------------------------------------------*/
#include "Mesh.h"

#include <glm/detail/func_geometric.inl>

Mesh::Mesh()
{
	Clear();
}

Mesh::~Mesh()
{
	Clear();
}

size_t Mesh::VertexBufferSizeInCount()
{
	return vertexBuffer.size();
}

size_t Mesh::IndicesBufferSizeInCount()
{
	return vertexIndices.size();
}


size_t Mesh::NormalBufferSizeInCount()
{
	return vertexNormals.size();
}

size_t Mesh::NormalDisplayBufferSizeInCount()
{
	return vertexNormalsDisplay.size();
}


size_t Mesh::FaceNormalDisplayBufferSizeInCount()
{
	return faceNormalsDisplay.size();
}

glm::vec3 Mesh::getModelScale()
{
	glm::vec3 scale = boundingBox[1] - boundingBox[0];

	if (scale.x == 0.0)
		scale.x = 1.0;

	if (scale.y == 0.0)
		scale.y = 1.0;

	if (scale.z == 0.0)
		scale.z = 1.0;

	return scale;
}

glm::vec3 Mesh::getModelBoundingSize()
{
	glm::vec3 range = getModelScale();
	return glm::vec3(2.0f / range.x, 2.0f / range.y, 2.0f / range.z);
}

glm::vec3 Mesh::getModelCentroid()
{
	return glm::vec3(boundingBox[0] + boundingBox[1]) * 0.5f;
}

glm::vec3 Mesh::getCentroidVector(glm::vec3 vVertex)
{
	return glm::normalize(vVertex - getModelCentroid());
}

std::vector<glm::vec3> Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

std::vector<glm::vec3> Mesh::GetVertexNormal()
{
	return vertexNormals;
}

float* Mesh::VertexData()
{
	return reinterpret_cast<float*>(vertexBuffer.data());
}

unsigned* Mesh::IndicesData()
{
	return vertexIndices.data();
}

float* Mesh::NormalData()
{
	return reinterpret_cast<float*>(vertexNormals.data());
}

float* Mesh::NormalDisplayData()
{
	return reinterpret_cast<float*>(vertexNormalsDisplay.data());
}

float* Mesh::FaceNormalDisplayData()
{
	return reinterpret_cast<float*>(faceNormalsDisplay.data());
}

void Mesh::PutVertexData(glm::vec3 data)
{
	vertexBuffer.emplace_back(data);
}

void Mesh::PutIndicesData(unsigned data)
{
	vertexIndices.emplace_back(data);
}

void Mesh::PutNormalData(glm::vec3 normal)
{
	vertexNormals.emplace_back(normal);
}

void Mesh::SetBoundingBox(int i, glm::vec3 val)
{
	boundingBox[i] = val;
}

void Mesh::CalculateNormals()
{
	unsigned numFaces = IndicesBufferSizeInCount();
	unsigned numVertices = VertexBufferSizeInCount();

	vertexNormals.resize(numVertices, glm::vec3());
	vertexNormalsDisplay.resize(numVertices * 2, glm::vec3());
	faceNormalsDisplay.resize(numFaces * 2, glm::vec3());

	memset(vertexNormals.data(), 0, vertexNormals.size() * sizeof(glm::vec3));
	memset(vertexNormalsDisplay.data(), 0, vertexNormalsDisplay.size() * sizeof(glm::vec3));
	memset(faceNormalsDisplay.data(), 0, faceNormalsDisplay.size() * sizeof(glm::vec3));

	unsigned index = 0;
	unsigned indexFace = 0;
	for (; index < numFaces; )
	{
		unsigned a = vertexIndices.at(index++);
		unsigned b = vertexIndices.at(index++);
		unsigned c = vertexIndices.at(index++);

		glm::vec3 vA = vertexBuffer[a];
		glm::vec3 vB = vertexBuffer[b];
		glm::vec3 vC = vertexBuffer[c];

		glm::vec3 vATemp(vA.x, vA.y, vA.z);
		glm::vec3 vBTemp(vB.x, vB.y, vB.z);
		glm::vec3 vCTemp(vC.x, vC.y, vC.z);

		glm::vec3 E1 = vBTemp - vATemp;
		glm::vec3 E2 = vCTemp - vATemp;

		glm::vec3 N = glm::normalize(glm::cross(E1, E2));

		vertexNormals[a] += glm::vec3(N);
		vertexNormals[b] += glm::vec3(N);
		vertexNormals[c] += glm::vec3(N);

		float midX = (vA.x + vB.x + vC.x) / 3;
		float midY = (vA.y + vB.y + vC.y) / 3;
		float midZ = (vA.z + vB.z + vC.z) / 3;

		faceNormalsDisplay[indexFace] = glm::vec3(midX, midY, midZ);
		glm::vec3 calcul = glm::vec3(midX + (N.x * 0.002f), midY + (N.y * 0.002f), midZ + (N.z * 0.002f));
		faceNormalsDisplay[indexFace + 1] = glm::vec3(calcul.x, calcul.y, calcul.z);
		indexFace += 2;
	}

	for (index = 0; index < numVertices; ++index)
	{
		glm::vec3 vertexNormalsVal = vertexNormals[index];
		glm::vec3 vertexVal = vertexBuffer[index];

		glm::vec3 normalized = glm::normalize(glm::vec3(vertexNormalsVal.x, vertexNormalsVal.y, vertexNormalsVal.z));

		vertexNormals[index] = glm::vec3(normalized.x, normalized.y, normalized.z);

		vertexNormalsVal = vertexNormals[index];

		vertexNormalsDisplay[2 * index] = vertexVal;
		vertexNormalsDisplay[2 * index + 1] = vertexVal + (vertexNormalsVal * 0.02f);

	}
}

void Mesh::Clear()
{
	vertexBuffer.clear();
	vertexIndices.clear();
}
