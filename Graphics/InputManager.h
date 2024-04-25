/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: inputManager.h
Purpose: To manage the keyboard input
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_1
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 6/20/2019
End Header --------------------------------------------------------*/

#ifndef _INPUTMANAGER_H
#define _INPUTMANAGER_H

#include "KeyCodes.h"

class InputManager
{
public:
	static InputManager& getInstance()
	{
		static InputManager instance;
		return instance;
	}

	bool isKeyDown		(int keyID);
	bool isKeyPressed	(int keyID);

private:
	InputManager		();
	~InputManager		();

private:
	bool m_keyWasPressed[256];
};

#endif