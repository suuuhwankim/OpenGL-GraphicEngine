/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: VertexShader.frag
Purpose: Fragment shader for normal Drawing
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_2
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 10/01/2021
End Header --------------------------------------------------------*/

#version 430 core

uniform vec3 normColor;

out vec4 fragColor;

void main()
{
	fragColor = vec4(normColor, 1.0);
}