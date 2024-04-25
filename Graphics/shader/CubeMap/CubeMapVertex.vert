/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: CubeMapVertex.vert
Purpose: vertex shader for cube Mapping
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_2
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 11/25/2021
End Header --------------------------------------------------------*/
#version 430 core


layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    texCoords = aTexCoord;
    mat4 new_view = view;

    for(int i = 0; i < 3; i++)
    {
        new_view[3][i] = 0.0;
    }

    vec4 pos = projection * new_view * vec4(aPos - 1, 1.0);
    gl_Position = pos.xyww;
}