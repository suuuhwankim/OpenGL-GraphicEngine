/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Texture.cpp
Purpose: To load the texture.
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_2
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 10/30/2021
End Header --------------------------------------------------------*/
#include "TExture.h"
#include <fstream>
#include <cstring>
#include <sstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb.image.h"
#include <GL/glew.h>

Texture* LoadTexture(const std::string& filename)
{
	std::ifstream tex(filename);
	std::string version;
	std::string creator;
	std::string size;
	std::string maxVal;

	Texture* result = new Texture();
	
	// Version 
	std::getline(tex, version);
	// creator(comment)
	std::getline(tex, creator);
	// size
	std::getline(tex, size);
	std::stringstream(size) >> result->width >> result->height;	
	// maximum value
	float max = 255;
	std::getline(tex, maxVal);
	std::stringstream(maxVal) >> max;

	//int maxSize = result->width * result->height;
	result->rgb.reserve(50000);
	
	int index = 0;
	int i = 0;
	std::string line;
	while(std::getline(tex, line))
	{	
		std::stringstream ss(line);
		if (i % 3 == 0) {
			result->rgb.push_back(glm::vec3(0.0f));
			ss >> result->rgb[index].r;
			result->rgb[index].r /= max;
		}
		else if (i % 3 == 1) {
			ss >> result->rgb[index].g;
			result->rgb[index].g /= max;
		}
		else if (i % 3 == 2) {
			ss >> result->rgb[index].b;
			result->rgb[index].b /= max;
			++index;
		}
		++i;
	}

	return result;
}

unsigned int LoadCubeMapping(const std::string& face)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	int width, height, channels;

	unsigned char* data = stbi_load(face.c_str(), &width, &height, &channels, 0);

	if(data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	else
	{
		std::cout << " Failed to load the cube map texture" << std::endl;
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	
	return textureID;
}
