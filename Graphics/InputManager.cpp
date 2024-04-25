/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: InputManager.cpp
Purpose: To manage the keyboard input
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_2
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 9/21/2021
End Header --------------------------------------------------------*/
#include "InputManager.h"
#include <windows.h>

InputManager::InputManager() :
	m_keyWasPressed {false}
{
	for (int i = 0; i < 256; ++i)
	{
		m_keyWasPressed[i] = false;
	}
}

InputManager::~InputManager()
{

}

bool InputManager::isKeyDown(int keyID)
{
	if ((GetAsyncKeyState(keyID) & 0x0001) == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool InputManager::isKeyPressed(int keyID)
{
	// Check if key is pressed or not.
	bool isPressed = (GetAsyncKeyState(keyID) & 0x8000) != 0;

	if (!m_keyWasPressed[keyID] && isPressed)
	{
		m_keyWasPressed[keyID] = isPressed;
		return true;
	}

	// 8 or 0 - being pressed.
	// 0 or 1 - have been pressed.
	m_keyWasPressed[keyID] = isPressed;
	return false;
}
