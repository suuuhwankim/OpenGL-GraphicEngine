/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: shader.hpp
Purpose: basic shader by prof. Pushpak
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_1
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 9/21/2021
End Header --------------------------------------------------------*/
#ifndef SHADER_HPP
#define SHADER_HPP

enum Primitive_Enum
{
    TriangleStrip = 0,
    Points,
    LineStrip
};

///////////////////
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

///////////////////
GLuint LoadPipeline(const char* vertex_file_path, const char* fragment_file_path, GLuint* programIDs);

// Load shaders where multiple shader files == one complete shader
// Show how code can be reused across shaders
GLuint LoadMultiShaders(const char* vertex_file_path, const char* fragment_file_path,
    const char* geom_file_path, Primitive_Enum  out_primitive_type);


#endif
