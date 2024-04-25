/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ObjectManager.h
Purpose: To manage the objects.
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_1
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 9/21/2021
End Header --------------------------------------------------------*/

#ifndef _OBJECT_MANAGER_H
#define _OBJECT_MANAGER_H

#include <vector>

class Object;

class ObjectManager
{
public:
	static ObjectManager& getInstance()
	{
		static ObjectManager instance;
		return instance;
	}

	void AddObject(Object* obj);
	void ClearObjects();

	const std::vector<Object*>& GetObjects();

private:
	ObjectManager();
	std::vector<Object*> objects;
};

#endif