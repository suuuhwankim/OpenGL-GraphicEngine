/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Texture.h
Purpose: To load the texture.
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_2
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 10/30/2021
End Header --------------------------------------------------------*/

#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Texture 
{
public:
	std::vector<glm::vec3> rgb;
	int width;
	int height;
};

Texture* LoadTexture(const std::string& filename);
unsigned int LoadCubeMapping(const std::string& face);

#endif