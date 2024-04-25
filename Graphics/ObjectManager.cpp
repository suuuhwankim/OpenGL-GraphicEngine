/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ObjectManager.cpp
Purpose: To manage the objects.
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_1
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 9/21/2021
End Header --------------------------------------------------------*/

#include "ObjectManager.h"
#include "OBJLoader.h"
#include "Object.h"

ObjectManager::ObjectManager() 
	: objects()
{
}

void ObjectManager::AddObject(Object* obj)
{
	objects.push_back(obj);
}

void ObjectManager::ClearObjects()
{
	for (auto& it : objects)
	{
		delete it;
	}
}

const std::vector<Object*>& ObjectManager::GetObjects()
{
	return objects;
}
