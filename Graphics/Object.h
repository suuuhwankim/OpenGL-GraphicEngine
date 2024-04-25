/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Object.h
Purpose: To create a object.
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_1
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 9/21/2021
End Header --------------------------------------------------------*/

#ifndef _OBJECT_H
#define _OBJECT_H

class Mesh;

class Object
{
public:
	Object(OBJ* obj, 
		const glm::vec3& pos   = glm::vec3(0.0f), 
		const glm::vec3& scl   = glm::vec3(1.0f), 
		const glm::vec3& color = glm::vec3(1.0f));
	~Object();

	void Destroy	();

	glm::vec3& GetPosition();
	glm::vec3& GetScale();
	glm::vec3& GetColor();

	void SetPosition	(const glm::vec3& pos);
	void SetScale		(const glm::vec3& scl);
	void SetColor		(const glm::vec3& color);
	void SetOBJFile		(OBJ& obj);
	void SetRotation	(float degree, const glm::vec3& axis);

	glm::mat4& GetObjectMatrix();
	glm::mat4& GetRotationMatrix();
	OBJ* GetOBJ();
	Mesh* GetMesh();
private:
	OBJ* obj;
	Mesh* mesh;
	glm::vec3 pos;		
	glm::vec3 scl;		
	glm::vec3 color;		
	glm::mat4 mat;		
	glm::mat4 rot;		

	glm::vec3 rotAxis;
	float rotDegree;
		
};

#endif