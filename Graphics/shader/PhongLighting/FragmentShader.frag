/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: FragmnetShader.frag
Purpose: Fragment shader for Phong Lighting
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_2
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 11/05/2021
End Header --------------------------------------------------------*/

#version 430 core

in vec3 outColor;
out vec4 fragColor;

void main()
{
	fragColor = vec4(outColor, 1.0);
}