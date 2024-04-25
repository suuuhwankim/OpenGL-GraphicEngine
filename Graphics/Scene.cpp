/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Scene.cpp
Purpose: basic scene by Prof. Pushpak
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_1
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 9/21/2021
End Header --------------------------------------------------------*/
#include "Scene.h"

Scene::Scene() : currWindow(nullptr),_windowWidth(100), _windowHeight(100)
{

}

Scene::Scene(GLFWwindow* window, int windowWidth, int windowHeight)
{
    currWindow = window;
    _windowHeight = windowHeight;
    _windowWidth = windowWidth;
}

Scene::~Scene()
{
    CleanUp();
}

// Public methods

// Init: called once when the scene is initialized
int Scene::Init()
{
    return -1;
}

// LoadAllShaders: This is the placeholder for loading the shader files
void Scene::LoadAllShaders()
{
    return;
}


// preRender : called to setup stuff prior to rendering the frame
int Scene::preRender()
{
    return -1;
}

// Render : per frame rendering of the scene
int Scene::Render()
{
    return -1;
}

// postRender : Any updates to calculate after current frame
int Scene::postRender()
{
    return -1;
}

// CleanUp : clean up resources before destruction
void Scene::CleanUp()
{
    return;
}

// Display : Per-frame execution of the scene
int Scene::Display(float dt)
{
    preRender();

    Render();

    postRender();

    return -1;
}