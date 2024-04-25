/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SimpleScene.cpp
Purpose: The main scene for Assignments
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_2
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 9/21/2021
End Header --------------------------------------------------------*/
#include "SimpleScene.h"
#include <iostream>
#include <GL/glew.h>

#include "Camera.h"
#include "InputManager.h"
#include "shader.hpp"
#include "OBJLoader.h"
#include "Texture.h"
#include "Object.h"
#include "ObjectManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <cstdlib>
#include <time.h>

#pragma warning(push)
#pragma warning(disable:6385)
namespace
{
	const int MAX_LIGHTS = 16;
	const float PI = 3.141592f;
	OBJ* skyBoxOBJ = nullptr;
}

SimpleScene::SimpleScene(GLFWwindow* window, int windowWidth, int windowHeight) :
	Scene(window, windowWidth, windowHeight), windowWidth(windowWidth), windowHeight(windowHeight),
	zNear(0.1f), zFar(15.f), phongShadingID(0), phongLightingID(0), blinShadingID(0), skyBoxProgramID(0),
	normalProgramID(0), currentProgramID(0), vao(0), buffers(nullptr), fbCam{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
	skyBoxVBO{ 0, 0, 0, 0, 0, 0 }, skyBoxUV(0), skyBoxVAO(0), skyBoxIdx(0), cubeTexture{ 0, 0, 0, 0, 0, 0 },
	sphVbo(0), sphIbo(0), sphVn(0), spheres(nullptr), fbo(0), rbo(0), fbTexture{ 0, 0, 0, 0, 0, 0 },
	orbitVbo(0), orbitVertSize(0), texture1(0), texture2(0), cam(nullptr), isFill(true), showFn(false),
	showVn(false), showObj(true), isPlanarMap(true), isCylin(false), isSph(false), entityPos(true), entityNor(false),
	isCpu(false), isGpu(true)

{
}

SimpleScene::~SimpleScene()
{
	CleanUp();
}

void SimpleScene::SetupNanoGUI(GLFWwindow* /*pWwindow*/)
{
}

void SimpleScene::InitMembers()
{

	// Basic object initialize

	OBJ* bunny = LoadOBJFromFile("obj/bunny.obj");
	//OBJ* plane = LoadOBJFromFile("obj/quad.obj");
	Object* obj1 = new Object(bunny);
	//Object* obj2 = new Object(plane);
	obj1->SetScale(glm::vec3(1.0f));

	//obj2->SetScale(glm::vec3(10.0f, 10.0f, 1.0f));
	//obj2->SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
	//obj2->SetRotation(-89.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	ObjectManager::getInstance().AddObject(obj1);
	//ObjectManager::getInstance().AddObject(obj2);



	// camera allocation
	cam = new Camera(currWindow, windowWidth, windowHeight);
	

	fbCam[SKYBOX::FRONT] = new Camera(currWindow, glm::vec3(0.0f, 0.0f, -1.0f));
	fbCam[SKYBOX::RIGHT] = new Camera(currWindow, glm::vec3(1.0f, 0.0f, 0.0f));
	fbCam[SKYBOX::BACK] = new Camera(currWindow, glm::vec3(0.0f, 0.0f, 1.0f));
	fbCam[SKYBOX::LEFT] = new Camera(currWindow, glm::vec3(-1.0f, 0.0f, 0.0f));
	fbCam[SKYBOX::TOP] = new Camera(currWindow, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	fbCam[SKYBOX::BOTTOM] = new Camera(currWindow, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));

	cam->SetPosition(glm::vec3(0.0f, 1.0f, 4.f));

	// version check
	char* glVersion = {};
	glVersion = (char*)glGetString(GL_VERSION);
	std::cout << "openGL version :  " << glVersion << std::endl;

	LoadAllShaders();
	currentProgramID = phongShadingID;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	ObjectManager* objManager = &ObjectManager::getInstance();
	const size_t numObjects = objManager->GetObjects().size();
	buffers = new ObjectBuffer[numObjects];
	int i = 0;
	for (auto& it : objManager->GetObjects())
	{

		SetUpObjBuffer(it->GetOBJ(), buffers[i]);
		SetUpUV(it->GetOBJ(), buffers[i]);
		i++;
	}
	spheres = new Object(LoadSphere(18, 1.0f));
	OBJ* sphOBJ = spheres->GetOBJ();
	Texture* tex_spec = LoadTexture("textures/metal_roof_diff_512x512.ppm");
	Texture* tex_diff = LoadTexture("textures/metal_roof_spec_512x512.ppm");

	size_t sphVertexSize = sphOBJ->vertexCount * sizeof(glm::vec3);
	glGenBuffers(1, &sphVbo);
	glBindBuffer(GL_ARRAY_BUFFER, sphVbo);
	glBufferData(GL_ARRAY_BUFFER, sphVertexSize, sphOBJ->vertices.data(), GL_STATIC_DRAW);

	size_t sphNormalSize = sphOBJ->pureVertexNormal.size() * sizeof(glm::vec3);
	glGenBuffers(1, &sphVn);
	glBindBuffer(GL_ARRAY_BUFFER, sphVn);
	glBufferData(GL_ARRAY_BUFFER, sphNormalSize, sphOBJ->pureVertexNormal.data(), GL_STATIC_DRAW);

	size_t sphIndicesSize = sphOBJ->indexCount * sizeof(glm::ivec3);
	glGenBuffers(1, &sphIbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphIndicesSize, sphOBJ->indices.data(), GL_STATIC_DRAW);


	// Skybox setting

	std::string skyboxPath = "textures";
	std::vector<std::string> faces =
	{
		"textures/skybox_front.jpg",
		"textures/skybox_right.jpg",
		"textures/skybox_back.jpg",
		"textures/skybox_left.jpg",
		"textures/skybox_top.jpg",
		"textures/skybox_bottom.jpg",
	};

	for (int i = 0; i < 6; i++)
	{
		cubeTexture[i] = LoadCubeMapping(faces[i]);
	}

	skyBoxOBJ = LoadOBJFromFile("obj/quad.obj");
	size_t skyboxVertices = skyBoxOBJ->vertexCount * sizeof(glm::vec3);


	std::vector<glm::vec3> skyboxVerticesData[6];
	std::vector<glm::vec2> skyBoxUVdata;

	skyBoxUVdata.push_back(glm::vec2(0, 1));
	skyBoxUVdata.push_back(glm::vec2(1, 1));
	skyBoxUVdata.push_back(glm::vec2(1, 0));
	skyBoxUVdata.push_back(glm::vec2(0, 0));

	skyboxVerticesData[SKYBOX::FRONT].push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	skyboxVerticesData[SKYBOX::FRONT].push_back(glm::vec3(2.0f, 0.0f, 0.0f));
	skyboxVerticesData[SKYBOX::FRONT].push_back(glm::vec3(2.0f, 2.0f, 0.0f));
	skyboxVerticesData[SKYBOX::FRONT].push_back(glm::vec3(0.0f, 2.0f, 0.0f));

	skyboxVerticesData[SKYBOX::RIGHT].push_back(glm::vec3(2.0f, 0.0f, 0.0f));
	skyboxVerticesData[SKYBOX::RIGHT].push_back(glm::vec3(2.0f, 0.0f, 2.0f));
	skyboxVerticesData[SKYBOX::RIGHT].push_back(glm::vec3(2.0f, 2.0f, 2.0f));
	skyboxVerticesData[SKYBOX::RIGHT].push_back(glm::vec3(2.0f, 2.0f, 0.0f));

	skyboxVerticesData[SKYBOX::LEFT].push_back(glm::vec3(0.0f, 0.0f, 2.0f));
	skyboxVerticesData[SKYBOX::LEFT].push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	skyboxVerticesData[SKYBOX::LEFT].push_back(glm::vec3(0.0f, 2.0f, 0.0f));
	skyboxVerticesData[SKYBOX::LEFT].push_back(glm::vec3(0.0f, 2.0f, 2.0f));

	skyboxVerticesData[SKYBOX::BACK].push_back(glm::vec3(2.0f, 0.0f, 2.0f));
	skyboxVerticesData[SKYBOX::BACK].push_back(glm::vec3(0.0f, 0.0f, 2.0f));
	skyboxVerticesData[SKYBOX::BACK].push_back(glm::vec3(0.0f, 2.0f, 2.0f));
	skyboxVerticesData[SKYBOX::BACK].push_back(glm::vec3(2.0f, 2.0f, 2.0f));

	skyboxVerticesData[SKYBOX::TOP].push_back(glm::vec3(0.0f, 2.0f, 0.0f));
	skyboxVerticesData[SKYBOX::TOP].push_back(glm::vec3(2.0f, 2.0f, 0.0f));
	skyboxVerticesData[SKYBOX::TOP].push_back(glm::vec3(2.0f, 2.0f, 2.0f));
	skyboxVerticesData[SKYBOX::TOP].push_back(glm::vec3(0.0f, 2.0f, 2.0f));

	skyboxVerticesData[SKYBOX::BOTTOM].push_back(glm::vec3(0.0f, 0.0f, 2.0f));
	skyboxVerticesData[SKYBOX::BOTTOM].push_back(glm::vec3(2.0f, 0.0f, 2.0f));
	skyboxVerticesData[SKYBOX::BOTTOM].push_back(glm::vec3(2.0f, 0.0f, 0.0f));
	skyboxVerticesData[SKYBOX::BOTTOM].push_back(glm::vec3(0.0f, 0.0f, 0.0f));

	glGenVertexArrays(1, &skyBoxVAO);

	for (int i = 0; i < 6; i++)
	{
		glGenBuffers(1, &skyBoxVBO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO[i]);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), skyboxVerticesData[i].data(), GL_STATIC_DRAW);
		glGenBuffers(1, &skyBoxUV);
		glBindBuffer(GL_ARRAY_BUFFER, skyBoxUV);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), skyBoxUVdata.data(), GL_STATIC_DRAW);
	}

	glGenBuffers(1, &skyBoxIdx);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyBoxIdx);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, skyBoxOBJ->indexCount * sizeof(glm::ivec3), skyBoxOBJ->indices.data(), GL_STATIC_DRAW);


	std::vector<glm::vec3> orbVertices;
	float angle = 0.0f;
	while (angle < 2 * PI)
	{
		orbVertices.push_back(glm::vec3(cosf(angle), sinf(angle), 0.0f));
		angle += (glm::radians(360.f / 180));
	}

	orbitVertSize = orbVertices.size();
	size_t orbIndicesSize = orbitVertSize * sizeof(glm::vec3);
	glGenBuffers(1, &orbitVbo);
	glBindBuffer(GL_ARRAY_BUFFER, orbitVbo);
	glBufferData(GL_ARRAY_BUFFER, orbIndicesSize, orbVertices.data(), GL_STATIC_DRAW);

	glGenTextures(1, &texture1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_diff->width, tex_diff->height, 0, GL_RGB, GL_FLOAT, tex_spec->rgb.data());

	glGenTextures(1, &texture2);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 1, GL_RGB, tex_diff->width, tex_diff->height, 0, GL_RGB, GL_FLOAT, tex_diff->rgb.data());


	// FBO

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	for (int i = 0; i < 6; ++i)
	{
		glGenTextures(1, &fbTexture[i]);
		glBindTexture(GL_TEXTURE_2D, fbTexture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbTexture[i], 0);
	}


	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Error in FrameBuffer " << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SimpleScene::CleanUp()
{
	glDeleteBuffers(1, &buffers->vbo);
	glDeleteBuffers(1, &buffers->vno);
	glDeleteBuffers(1, &buffers->pureVn);
	glDeleteBuffers(1, &buffers->fno);
	glDeleteBuffers(1, &buffers->cbf);
	glDeleteBuffers(1, &buffers->idx);
	glDeleteBuffers(1, &buffers->uv);
	glDeleteBuffers(1, &sphVbo);
	glDeleteBuffers(1, &sphIbo);
	glDeleteBuffers(1, &sphVn);
	glDeleteBuffers(1, &orbitVbo);
	glDeleteBuffers(1, &skyBoxUV);
	glDeleteBuffers(1, &skyBoxVAO);
	glDeleteBuffers(1, &skyBoxIdx);
	glDeleteBuffers(1, &fbo);
	glDeleteBuffers(1, &rbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(phongShadingID);
	glDeleteProgram(phongLightingID);
	glDeleteProgram(blinShadingID);
	glDeleteProgram(normalProgramID);
	glDeleteProgram(skyBoxProgramID);
	delete cam;
	delete spheres;

	for(int i = 0; i < 6; i++)
	{
		delete fbCam[i];
	}
}

int SimpleScene::Init()
{
	// Imgui initialize
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(currWindow, true);
	ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

	ImGui::StyleColorsDark();
	InitMembers();

	return 0;
}

void SimpleScene::LoadAllShaders()
{
	phongShadingID = LoadShaders("shader/PhongShading/VertexShader.vert", "shader/PhongShading/FragmentShader.frag");
	phongLightingID = LoadShaders("shader/PhongLighting/VertexShader.vert", "shader/PhongLighting/FragmentShader.frag");
	blinShadingID = LoadShaders("shader/BlinShading/VertexShader.vert", "shader/BlinShading/FragmentShader.frag");
	normalProgramID = LoadShaders("shader/normalVertex.vert", "shader/normalFrag.frag");
	skyBoxProgramID = LoadShaders("shader/CubeMap/CubeMapVertex.vert", "shader/CubeMap/CubeMapFrag.frag");
}

void SimpleScene::DisplaySetting()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SimpleScene::SelectMode()
{
	if (isFill == true) { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
	else { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }

}

int SimpleScene::Display(float dt)
{
	SelectMode();
	DisplaySetting();

	glm::mat4 proj = glm::perspective(
		glm::radians(90.0f),
		(float)windowWidth / (float)windowHeight,
		zNear,
		100.0f
	);
	glm::mat4 view = cam->GetViewMatrix();




	/******************************************************
	 *
	 *                ImGui Setting
	 *
	 ******************************************************/

	float orbitSetSize = 2.f;
	static float rot = 0.01f;
	static float orbitSize = orbitSetSize;
	static float sphScale = 0.1f;
	static float add_scale = 2.5f;
	static int   numLights = 16;
	static float cutOff[MAX_LIGHTS];
	static float outerCutOff[MAX_LIGHTS];
	static float fallOff[MAX_LIGHTS];
	static bool  isRot = true;
	static bool  dataChanged = false;
	static glm::vec3 globalAmbient = { 0.01f, 0.01f, 0.01f };
	float rotSpeed = 1.5f;
	static glm::vec3 lightColorVector[MAX_LIGHTS] = { glm::vec3(1.0f) };
	static LightType lightTypes[MAX_LIGHTS] = { L_POINT };
	static int lightTypeCurrent[MAX_LIGHTS] = { 0 };
	static bool specialEffect = false;

	static bool environmentMapping = false;
	static bool isReflect = true;
	static bool isRefract = true;
	static bool isShading = false;

	static bool press = false;

	static float fresnel = 0.6f;

	float tempAngle = glm::radians(360.f) + rot;
	if (InputManager::getInstance().isKeyDown(KEY_D))
	{
		cam->Move(glm::vec3(1.0f, 0.0f, 0.0f), 20.0f * dt);
	}
	else if(InputManager::getInstance().isKeyDown(KEY_A))
	{
		cam->Move(glm::vec3(-1.0f, 0.0f, 0.0f), 20.0f * dt);
	}

	
	if (InputManager::getInstance().isKeyDown(KEY_W))
	{
		cam->Move(glm::vec3(0.0f, 0.0f, -1.0f), 20.0f * dt);
	}
	else if (InputManager::getInstance().isKeyDown(KEY_S))
	{
		cam->Move(glm::vec3(0.0f, 0.0f, 1.0f), 20.0f * dt);
	}

	if (InputManager::getInstance().isKeyDown(KEY_Z))
	{
		cam->Move(glm::vec3(0.0f, -1.0f, 0.0f), 20.0f * dt);
	}
	else if (InputManager::getInstance().isKeyDown(KEY_C))
	{
		cam->Move(glm::vec3(0.0f, 1.0f, 0.0f), 20.0f * dt);
	}


	if(InputManager::getInstance().isKeyDown(KEY_E))
	{
		
		if(press == true)
		{
			press = false;
		}
		else
		{
			press = true;
		}
	}

	if(press == true)
	{
		cam->ControlView(0.005, dt);
	}
	
	

	static bool init = false;

	if (init == false)
	{
		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			cutOff[i] = 30.0f;
			outerCutOff[i] = 45.0f;
			fallOff[i] = 10.0f;
			lightColorVector[i] = glm::vec3(1.0f);
		}
		init = true;
	}
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Control Panel");

	srand(static_cast<unsigned int>(time(NULL)));
	if (ImGui::Button("Scenario 1"))
	{
		// All same color, all same types
		
		orbitSize = orbitSetSize;
		numLights = MAX_LIGHTS;
		int randomType = rand() % 3;
		float randomFloat_1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float randomFloat_2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float randomFloat_3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		glm::vec3 randomColor = { randomFloat_1, randomFloat_2, randomFloat_3 };
		for (int i = 0; i < numLights; i++)
		{
			lightTypes[i] = (LightType)randomType;
			lightTypeCurrent[i] = (LightType)randomType;
			lightColorVector[i] = randomColor;
		}
		specialEffect = false;
	}
	if (ImGui::Button("Scenario 2"))
	{
		orbitSize = orbitSetSize;
		numLights = MAX_LIGHTS;
		int randomType = rand() % 3;
		for (int i = 0; i < numLights; i++)
		{
			float randomFloat_1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float randomFloat_2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float randomFloat_3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

			lightTypes[i] = (LightType)randomType;
			lightTypeCurrent[i] = (LightType)randomType;
			lightColorVector[i] = glm::vec3(randomFloat_1, randomFloat_2, randomFloat_3);
		}
		specialEffect = false;
	}
	if (ImGui::Button("Scenario 3"))
	{
		specialEffect = true;
		numLights = MAX_LIGHTS;
		int sp = 0;
		for (int i = 0; i < numLights; i++)
		{
			int randomType = rand() % 3;
			float randomFloat_1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float randomFloat_2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float randomFloat_3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

			lightTypes[i] = (LightType)randomType;
			if (lightTypes[i] == L_SPOT)
			{
				fallOff[sp] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 100.f)) + 1.f;
				outerCutOff[sp] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 100.f));
				cutOff[sp] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 100.f));
				sp++;
			}
			lightTypeCurrent[i] = (LightType)randomType;
			lightColorVector[i] = glm::vec3(randomFloat_1, randomFloat_2, randomFloat_3);

		}
	}

	static float  ratio = 1.0f;

	ImGui::Checkbox("Shading", &isShading);
	ImGui::Checkbox("Reflection", &isReflect);
	ImGui::Checkbox("Refraction", &isRefract);
	ImGui::DragFloat("Ratio", &ratio, 0.05f, 0.0f, 100.0f, "%f");
	ImGui::DragFloat("Fresnel Constant", &fresnel, 0.001f, 0.01f, 1.0f, "%f");

	const char* refractionMat[] =
	{
		"Air", "Hydrogen", "Water",
		"Olive Oil", "Ice", "Quartz",
		"Diamond", "Acrylic", "Plexiglas","Lucite",
	};


	static int currRefractionMat = 0;

	if (ImGui::Combo("Refraction Materials\n", &currRefractionMat, refractionMat, IM_ARRAYSIZE(refractionMat)))
	{
		switch (currRefractionMat)
		{
		case 0:
			ratio = 1.000293f;
			break;
		case 1:
			ratio = 1.000132f;
			break;
		case 2:
			ratio = 1.333f;
			break;
		case 3:
			ratio = 1.47f;
			break;
		case 4:
			ratio = 1.31f;
			break;
		case 5:
			ratio = 1.46f;
			break;
		case 6:
			ratio = 2.42f;
			break;
		default:
			ratio = 1.49f;
			break;
		}
	}

	static float mixRatio = 0.5f;
	ImGui::DragFloat("Mix Ratio", &mixRatio, 0.01f, 0.0f, 1.0f, "%f");

	const float orbitMax = 5.0f;
	const float orbitMin = 2.0f;
	ImGui::Checkbox("Draw Object", &showObj);
	ImGui::Checkbox("Draw Fill", &isFill);
	ImGui::Checkbox("Draw Face Normal", &showFn);
	ImGui::Checkbox("Draw Vertex Normal", &showVn);
	ImGui::DragFloat("Object Scale", &add_scale, 0.1f, 0.0f, 4.0f, "%f");
	ImGui::DragFloat("Sphere Scale", &sphScale, 0.01f, 0.0f, 4.0f, "%f");
	ImGui::DragFloat("Orbit Radius", &orbitSize, 0.1f, orbitMin, orbitMax, "%f");


	static bool inc = true;
	static bool dec = false;
	if (specialEffect == true)
	{
		if (orbitSize >= (orbitMax - 0.5f)) {
			dec = true; inc = false;
		}
		if (orbitSize <= (orbitMin + 0.5f)) { inc = true; dec = false; }

		if (inc == true)
		{
			orbitSize += dt * (orbitMax - orbitSize);
		}
		if (dec == true)
		{
			orbitSize -= dt * (orbitSize - orbitMin);
		}
	}


	const char* objFiles[] =
	{ "4Sphere.obj", "bunny.obj",
	  "bunny_high_poly.obj", "cube.obj", "cube2.obj",
	  "cup.obj", "lucy_princeton.obj",
	  "quad.obj", "rhino.obj",
	  "sphere.obj","sphere_modified.obj",
	  "starwars1.obj","triangle.obj" };
	static int objFilesCurrent = 1;
	if (ImGui::Combo(".obj Files\n", &objFilesCurrent, objFiles, IM_ARRAYSIZE(objFiles)))
	{
		Object* target = ObjectManager::getInstance().GetObjects()[0];
		std::string objFilesString(objFiles[objFilesCurrent]);
		// re-load obj file and draw.
		std::string filepath = "obj/" + objFilesString;
		target->SetOBJFile(*(LoadOBJFromFile(filepath)));
		SetUpObjBuffer(target->GetOBJ(), buffers[0]);
		SetUpUV(target->GetOBJ(), buffers[0]);
	}

	static int shaderFileCurrent = 0;
	if (ImGui::Button("Recompile Shader"))
	{
		LoadAllShaders();
		currentProgramID = phongShadingID;
		shaderFileCurrent = 0;
	}
	const char* shaderFiles[] =
	{
		"Phong Shading",
		"Phong Lighting",
		"Blin Shading"
	};

	if (ImGui::Combo(".shader Files\n", &shaderFileCurrent, shaderFiles, IM_ARRAYSIZE(shaderFiles)))
	{
		switch (shaderFileCurrent)
		{
		case 0:
			currentProgramID = phongShadingID;
			break;
		case 1:
			currentProgramID = phongLightingID;
			break;
		case 2:
			currentProgramID = blinShadingID;
			break;
		}
	}

	ImGui::NewLine();

	if (ImGui::Checkbox("Toggle CPU", &isCpu))
	{
		isGpu = false;
		dataChanged = true;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Toggle GPU", &isGpu))
	{
		isCpu = false;
	}

	if (ImGui::Checkbox("Planar", &isPlanarMap))
	{
		isCylin = false;
		isSph = false;
		dataChanged = true;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Cylindrical", &isCylin))
	{
		isPlanarMap = false;
		isSph = false;
		dataChanged = true;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Spherical", &isSph))
	{
		isPlanarMap = false;
		isCylin = false;
		dataChanged = true;
	}
	if (ImGui::Checkbox("Entity Pos", &entityPos))
	{
		entityNor = false;
		dataChanged = true;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Entity Normal", &entityNor))
	{
		entityPos = false;
		dataChanged = true;
	}



	ImGui::ColorEdit3("Global Ambient", (float*)&globalAmbient);
	ImGui::SliderInt("Num Lights", &numLights, 1, 16);
	const char* lightTypesList[] = {
		"Point",
		"Directional",
		"Spot"
	};
	ImGui::Checkbox("Toggle Rotation", &isRot);


	// detrermine light type
	int numDir = 0, numPoint = 0, numSpot = 0;
	int spot = 0;
	for (int i = 0; i < numLights; i++)
	{
		// Each light's options. (color, type);
		std::string label = "Light " + std::to_string(i);
		ImGui::Text(label.c_str());
		ImGui::ColorEdit3(label.c_str(), (float*)&lightColorVector[i]);

		if (ImGui::Combo(label.c_str(), &lightTypeCurrent[i], lightTypesList, IM_ARRAYSIZE(lightTypesList)))
		{
			lightTypes[i] = (LightType)lightTypeCurrent[i];
		}
		if (lightTypes[i] == L_SPOT)
		{
			ImGui::DragFloat("Cut Off", &cutOff[spot], 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("Outer Cut Off", &outerCutOff[spot], 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("Fall Off", &fallOff[spot], 0.1f, 1.0f, 100.0f);
			spot++;
		}

		if (lightTypes[i] == L_DIR) { numDir++; }
		else if (lightTypes[i] == L_POINT) { numPoint++; }
		else if (lightTypes[i] == L_SPOT) { numSpot++; }
		ImGui::NewLine();
	}


	ImGui::End();

	/****************************************************
	 *
	 *           Skybox Rendering
	 *
	 ***************************************************/

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glUseProgram(skyBoxProgramID);


	glm::mat4 skyboxView = glm::mat4(glm::mat3(cam->GetViewMatrix()));

	GLuint skyboxViewLocation = glGetUniformLocation(skyBoxProgramID, "view");
	GLuint skyboxProjectionLocation = glGetUniformLocation(skyBoxProgramID, "projection");
	glUniformMatrix4fv(skyboxViewLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(skyboxProjectionLocation, 1, GL_FALSE, glm::value_ptr(proj));

	glBindVertexArray(skyBoxVAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	for (int i = 0; i < 6; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, skyBoxUV);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, cubeTexture[i]);

		glUniform1i(glGetUniformLocation(skyBoxProgramID, "skybox"), 3);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyBoxIdx);
		glDrawElements(GL_TRIANGLES, skyBoxOBJ->indexCount * 3, GL_UNSIGNED_INT, 0);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	


	/****************************************************
	 *
	 *           Light Rendering
	 *
	 ***************************************************/


	glUseProgram(normalProgramID);
	cam->ControlView(0.0f, dt);

	GLuint normColorLocation = glGetUniformLocation(normalProgramID, "normColor");
	GLuint normalMLocation = glGetUniformLocation(normalProgramID, "model");
	GLuint normalVLocation = glGetUniformLocation(normalProgramID, "view");
	GLuint normalPLocation = glGetUniformLocation(normalProgramID, "proj");

	glm::vec3 lightPosVector[MAX_LIGHTS];
	for (int i = 0; i < numLights; ++i)
	{
		float angle = i * (glm::radians(360.f / MAX_LIGHTS)) + rot;

		spheres->SetPosition(glm::vec3(cosf(angle) * orbitSize, 0.0f, sinf(angle) * orbitSize));
		glm::mat4 sphModel = spheres->GetObjectMatrix();

		sphModel = glm::scale(sphModel, glm::vec3(sphScale));

		
		glUniform3f(normColorLocation, lightColorVector[i].x, lightColorVector[i].y, lightColorVector[i].z);

		glUniformMatrix4fv(normalMLocation, 1, GL_FALSE, glm::value_ptr(sphModel));
		glUniformMatrix4fv(normalVLocation, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(normalPLocation, 1, GL_FALSE, glm::value_ptr(proj));

		lightPosVector[i] = spheres->GetPosition();

		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, sphVbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, sphVn);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphIbo);
		glDrawElements(GL_TRIANGLES, spheres->GetOBJ()->indexCount * 3, GL_UNSIGNED_INT, 0);
	}
	if (isRot == true)
	{
		rot += dt * rotSpeed;
	}




	/****************************************************
	 *
	 *           Object Rendering
	 *
	 ***************************************************/

	int count = 0;
	const int numObjects = (int)ObjectManager::getInstance().GetObjects().size();
	for (auto& it : ObjectManager::getInstance().GetObjects())
	{
		glUseProgram(currentProgramID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		GLuint numDirLoc = glGetUniformLocation(currentProgramID, "numDir");
		GLuint numPointLoc = glGetUniformLocation(currentProgramID, "numPoint");
		GLuint numSpotLoc = glGetUniformLocation(currentProgramID, "numSpot");
		GLuint gAmbientLoc = glGetUniformLocation(currentProgramID, "g_ambient");
		GLuint fogColorLoc = glGetUniformLocation(currentProgramID, "fogColor");
		GLuint zFarLoc = glGetUniformLocation(currentProgramID, "zFar");
		GLuint zNearLoc = glGetUniformLocation(currentProgramID, "zNear");
		GLuint tex1Loc = glGetUniformLocation(currentProgramID, "tex1");
		GLuint tex2Loc = glGetUniformLocation(currentProgramID, "tex2");
		GLuint mapTypeLoc = glGetUniformLocation(currentProgramID, "mapType");
		GLuint entityModeLoc = glGetUniformLocation(currentProgramID, "entity_mode");
		GLuint isGpuLoc = glGetUniformLocation(currentProgramID, "isGpu");

		GLuint isReflectID = glGetUniformLocation(phongShadingID, "isReflect");
		GLuint isRefractID = glGetUniformLocation(phongShadingID, "isRefract");
		GLuint isMixID = glGetUniformLocation(phongShadingID, "isShading");

		glUniform1i(numDirLoc, numDir);
		glUniform1i(numPointLoc, numPoint);
		glUniform1i(numSpotLoc, numSpot);
		glUniform3f(gAmbientLoc, globalAmbient.x, globalAmbient.y, globalAmbient.z);
		glUniform3f(fogColorLoc, 0.15f, 0.15f, 0.15f);
		glUniform1f(zFarLoc, zFar);
		glUniform1f(zNearLoc, zNear);
		glUniform1i(tex1Loc, 0);
		glUniform1i(tex2Loc, 1);

		GLuint cubeMapID[6] =
		{
			glGetUniformLocation(phongShadingID, "cube[0]"),
			glGetUniformLocation(phongShadingID, "cube[1]"),
			glGetUniformLocation(phongShadingID, "cube[2]"),
			glGetUniformLocation(phongShadingID, "cube[3]"),
			glGetUniformLocation(phongShadingID, "cube[4]"),
			glGetUniformLocation(phongShadingID, "cube[5]"),
		};

		if (isReflect == true) {
			glUniform1i(isReflectID, 1);
		}
		else if (isReflect == false) {
			glUniform1i(isReflectID, 0);
		}


		if (isRefract == true) {
			glUniform1i(isRefractID, 1);
		}
		else if (isRefract == false) {
			glUniform1i(isRefractID, 0);
		}


		if (isShading == true) {
			glUniform1i(isMixID, 1);
		}
		else if (isShading == false) {
			glUniform1i(isMixID, 0);
		}



		if (isPlanarMap == true)
		{
			glUniform1i(mapTypeLoc, 1);
		}
		else if (isCylin == true)
		{
			glUniform1i(mapTypeLoc, 2);
		}
		else if (isSph == true)
		{
			glUniform1i(mapTypeLoc, 3);
		}

		if (entityPos == true)
		{
			glUniform1i(entityModeLoc, 1);
		}
		else if (entityNor == true)
		{
			glUniform1i(entityModeLoc, 2);
		}

		if (isGpu == true)
		{
			glUniform1i(isGpuLoc, 1);
		}
		else if (isCpu == true)
		{
			glUniform1i(isGpuLoc, 0);
		}

		GLuint numLightLocation = glGetUniformLocation(currentProgramID, "numLight");
		glUniform1i(numLightLocation, numLights);

		glm::vec3 scale = it->GetScale();
		glm::vec3 color = it->GetColor();
		glm::mat4 objModel = it->GetObjectMatrix();
		GLuint rotationMatrixLocation = glGetUniformLocation(currentProgramID, "rotationMatrix");
		glUniformMatrix4fv(rotationMatrixLocation, 1, GL_FALSE, glm::value_ptr(it->GetRotationMatrix()));
		GLuint mLocation = glGetUniformLocation(currentProgramID, "model");
		GLuint vLocation = glGetUniformLocation(currentProgramID, "view");
		GLuint pLocation = glGetUniformLocation(currentProgramID, "proj");
		GLuint viewPosLocation = glGetUniformLocation(currentProgramID, "viewPos");

		if (count == 0)
		{
			objModel = glm::scale(objModel, glm::vec3(add_scale));
		}

		glUniformMatrix4fv(mLocation, 1, GL_FALSE, glm::value_ptr(objModel));
		glUniformMatrix4fv(vLocation, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(pLocation, 1, GL_FALSE, glm::value_ptr(proj));

		glm::vec3 camPos = cam->GetPosition();
		glUniform3f(viewPosLocation, camPos.x, camPos.y, camPos.z);


		GLuint objColor = glGetUniformLocation(currentProgramID, "objColor");
		glUniform3f(objColor, color.x, color.y, color.z);

		std::string lightName[] = { "pointLights", "dirLights", "spotLights" };
		int dir = 0, pt = 0, spt = 0;
		for (int i = 0; i < numLights; i++)
		{

			if (lightTypes[i] == LightType::L_DIR)
			{
				std::string dirLightIndex = "[" + std::to_string(dir) + "]";
				std::string dirLightInfo[] = { ".direction", ".ambient", ".diffuse", ".specular", ".color" };

				std::string dirLightLocations[] = {
					lightName[LightType::L_DIR] + dirLightIndex + dirLightInfo[0],
					lightName[LightType::L_DIR] + dirLightIndex + dirLightInfo[1],
					lightName[LightType::L_DIR] + dirLightIndex + dirLightInfo[2],
					lightName[LightType::L_DIR] + dirLightIndex + dirLightInfo[3],
					lightName[LightType::L_DIR] + dirLightIndex + dirLightInfo[4]
				};

				GLuint dirLightDirection = glGetUniformLocation(currentProgramID, dirLightLocations[0].c_str());
				GLuint dirAmbientLocation = glGetUniformLocation(currentProgramID, dirLightLocations[1].c_str());
				GLuint dirDiffuseLocation = glGetUniformLocation(currentProgramID, dirLightLocations[2].c_str());
				GLuint dirSpecLocation = glGetUniformLocation(currentProgramID, dirLightLocations[3].c_str());
				GLuint dirLightColor = glGetUniformLocation(currentProgramID, dirLightLocations[4].c_str());

				glUniform3f(dirLightDirection,
					-lightPosVector[i].x,
					-lightPosVector[i].y,
					-lightPosVector[i].z);

				glUniform3f(dirAmbientLocation, 0.05f, 0.05f, 0.05f);
				glUniform3f(dirDiffuseLocation, 0.8f, 0.8f, 0.8f);
				glUniform3f(dirSpecLocation, 1.0f, 1.0f, 1.0f);
				glUniform3f(dirLightColor, lightColorVector[i].x, lightColorVector[i].y, lightColorVector[i].z);

				dir++;
			}
			else if (lightTypes[i] == LightType::L_POINT)
			{
				std::string pointLightIndex = "[" + std::to_string(pt) + "]";
				std::string pointLightInfo[] = {
					".position",
					".ambient",
					".diffuse",
					".specular",
					".color",
					".constant",
					".linear",
					".quadratic"
				};

				std::string pointLightLocations[] = {
					lightName[LightType::L_POINT] + pointLightIndex + pointLightInfo[0],
					lightName[LightType::L_POINT] + pointLightIndex + pointLightInfo[1],
					lightName[LightType::L_POINT] + pointLightIndex + pointLightInfo[2],
					lightName[LightType::L_POINT] + pointLightIndex + pointLightInfo[3],
					lightName[LightType::L_POINT] + pointLightIndex + pointLightInfo[4],
					lightName[LightType::L_POINT] + pointLightIndex + pointLightInfo[5],
					lightName[LightType::L_POINT] + pointLightIndex + pointLightInfo[6],
					lightName[LightType::L_POINT] + pointLightIndex + pointLightInfo[7],
				};

				GLuint pointLightPosition = glGetUniformLocation(currentProgramID, pointLightLocations[0].c_str());
				GLuint pointAmbientLocation = glGetUniformLocation(currentProgramID, pointLightLocations[1].c_str());
				GLuint pointDiffuseLocation = glGetUniformLocation(currentProgramID, pointLightLocations[2].c_str());
				GLuint pointSpecLocation = glGetUniformLocation(currentProgramID, pointLightLocations[3].c_str());
				GLuint pointLightColor = glGetUniformLocation(currentProgramID, pointLightLocations[4].c_str());
				GLuint pointLightConstant = glGetUniformLocation(currentProgramID, pointLightLocations[5].c_str());
				GLuint pointLightLinear = glGetUniformLocation(currentProgramID, pointLightLocations[6].c_str());
				GLuint pointLightQuadratic = glGetUniformLocation(currentProgramID, pointLightLocations[7].c_str());

				glUniform3f(pointLightPosition,
					lightPosVector[i].x,
					lightPosVector[i].y,
					lightPosVector[i].z);

				glUniform3f(pointAmbientLocation, 0.05f, 0.05f, 0.05f);
				glUniform3f(pointDiffuseLocation, 0.8f, 0.8f, 0.8f);
				glUniform3f(pointSpecLocation, 1.0f, 1.0f, 1.0f);

				glUniform3f(pointLightColor, lightColorVector[i].x, lightColorVector[i].y, lightColorVector[i].z);
				glUniform1f(pointLightConstant, 1.0f);
				glUniform1f(pointLightLinear, 0.09f);
				glUniform1f(pointLightQuadratic, 0.032f);

				pt++;
			}
			else if (lightTypes[i] == LightType::L_SPOT)
			{
				std::string spotLightIndex = "[" + std::to_string(spt) + "]";
				std::string spotLightInfo[] = {
					".position",
					".direction",
					".ambient",
					".diffuse",
					".specular",
					".constant",
					".linear",
					".quadratic",
					".color",
					".cutOff",
					".outerCutOff",
					".fallOff"
				};

				std::string spotLightLocations[] = {
					lightName[LightType::L_SPOT] + spotLightIndex + spotLightInfo[0],
					lightName[LightType::L_SPOT] + spotLightIndex + spotLightInfo[1],
					lightName[LightType::L_SPOT] + spotLightIndex + spotLightInfo[2],
					lightName[LightType::L_SPOT] + spotLightIndex + spotLightInfo[3],
					lightName[LightType::L_SPOT] + spotLightIndex + spotLightInfo[4],
					lightName[LightType::L_SPOT] + spotLightIndex + spotLightInfo[5],
					lightName[LightType::L_SPOT] + spotLightIndex + spotLightInfo[6],
					lightName[LightType::L_SPOT] + spotLightIndex + spotLightInfo[7],
					lightName[LightType::L_SPOT] + spotLightIndex + spotLightInfo[8],
					lightName[LightType::L_SPOT] + spotLightIndex + spotLightInfo[9],
					lightName[LightType::L_SPOT] + spotLightIndex + spotLightInfo[10],
					lightName[LightType::L_SPOT] + spotLightIndex + spotLightInfo[11]
				};

				GLuint spotLightPosition = glGetUniformLocation(currentProgramID, spotLightLocations[0].c_str());
				GLuint spotLightDirection = glGetUniformLocation(currentProgramID, spotLightLocations[1].c_str());
				GLuint spotAmbientLocation = glGetUniformLocation(currentProgramID, spotLightLocations[2].c_str());
				GLuint spotDiffuseLocation = glGetUniformLocation(currentProgramID, spotLightLocations[3].c_str());
				GLuint spotSpecLocation = glGetUniformLocation(currentProgramID, spotLightLocations[4].c_str());
				GLuint spotConstantLocation = glGetUniformLocation(currentProgramID, spotLightLocations[5].c_str());
				GLuint spotLinearLocation = glGetUniformLocation(currentProgramID, spotLightLocations[6].c_str());
				GLuint spotQuadratLocation = glGetUniformLocation(currentProgramID, spotLightLocations[7].c_str());
				GLuint spotLightColor = glGetUniformLocation(currentProgramID, spotLightLocations[8].c_str());
				GLuint spotLightCutOff = glGetUniformLocation(currentProgramID, spotLightLocations[9].c_str());
				GLuint spotLightOuterCutOff = glGetUniformLocation(currentProgramID, spotLightLocations[10].c_str());
				GLuint spotLightfallOff = glGetUniformLocation(currentProgramID, spotLightLocations[11].c_str());

				glUniform3f(spotLightDirection,
					-lightPosVector[i].x,
					-lightPosVector[i].y,
					-lightPosVector[i].z);
				glUniform3f(spotLightPosition,
					lightPosVector[i].x,
					lightPosVector[i].y,
					lightPosVector[i].z);

				glUniform3f(spotAmbientLocation, 0.05f, 0.05f, 0.05f);
				glUniform3f(spotDiffuseLocation, 0.8f, 0.8f, 0.8f);
				glUniform3f(spotSpecLocation, 1.0f, 1.0f, 1.0f);

				glUniform3f(spotLightColor, lightColorVector[i].x, lightColorVector[i].y, lightColorVector[i].z);
				glUniform1f(spotLightCutOff, glm::cos(glm::radians(cutOff[spt])));
				glUniform1f(spotLightOuterCutOff, glm::cos(glm::radians(cutOff[spt] + outerCutOff[spt])));
				glUniform1f(spotConstantLocation, 1.0f);
				glUniform1f(spotLinearLocation, 0.09f);
				glUniform1f(spotQuadratLocation, 0.032f);
				glUniform1f(spotLightfallOff, fallOff[spt]);

				spt++;
			}

		}

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[count].vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[count].pureVn);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		if (dataChanged == true)
		{
			SetUpUV(it->GetOBJ(), buffers[count]);
			dataChanged = false;
		}
		glBindBuffer(GL_ARRAY_BUFFER, buffers[count].uv);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);


		if (showObj == true)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[count].idx);
			glDrawElements(GL_TRIANGLES, it->GetOBJ()->indexCount * 3, GL_UNSIGNED_INT, 0);
		}

		/****************************************************
		*
		*           Envrionment Rendering with fbo
		*
		***************************************************/

		if (isReflect == true || isRefract == true)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glUseProgram(phongShadingID);

			GLuint ratioLocation = glGetUniformLocation(phongShadingID, "inputRatio");
			GLuint mixRatioLocation = glGetUniformLocation(phongShadingID, "mixRatio");
			GLuint fresnelLocation = glGetUniformLocation(phongShadingID, "fresnel");

			glUniform1f(ratioLocation, ratio);
			glUniform1f(mixRatioLocation, mixRatio);
			glUniform1f(fresnelLocation, fresnel);

			for (int i = 0; i < 6; i++)
			{
				glActiveTexture(GL_TEXTURE10 + i);
				glBindTexture(GL_TEXTURE_2D, fbTexture[i]);
				glUniform1i(cubeMapID[i], 10 + i);
			}

			for (int i = 0; i < 6; i++)
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbTexture[i], 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glm::mat4 envProj = glm::perspective
				(
					glm::radians(90.0f),
					1.0f,
					0.1f,
					100.0f
				);
				glm::mat4 envView = fbCam[i]->GetViewMatrix();
				glm::mat4 envModel(1.0f);

				GLuint envMLocation = glGetUniformLocation(phongShadingID, "model");
				GLuint envVLocation = glGetUniformLocation(phongShadingID, "view");
				GLuint envPLocation = glGetUniformLocation(phongShadingID, "proj");

				glUniformMatrix4fv(envMLocation, 1, GL_FALSE, glm::value_ptr(envModel));
				glUniformMatrix4fv(envVLocation, 1, GL_FALSE, glm::value_ptr(envView));
				glUniformMatrix4fv(envPLocation, 1, GL_FALSE, glm::value_ptr(envProj));


					/****************************************************
		             *
		             *           Skybox Rendering
		             *
		             ***************************************************/

				glDisable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);
				glUseProgram(skyBoxProgramID);


				glm::mat4 skyboxView = glm::mat4(glm::mat3(envView));

				skyboxViewLocation = glGetUniformLocation(skyBoxProgramID, "view");
				skyboxProjectionLocation = glGetUniformLocation(skyBoxProgramID, "projection");
				glUniformMatrix4fv(skyboxViewLocation, 1, GL_FALSE, glm::value_ptr(skyboxView));
				glUniformMatrix4fv(skyboxProjectionLocation, 1, GL_FALSE, glm::value_ptr(envProj));

				glBindVertexArray(skyBoxVAO);
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				for (int i = 0; i < 6; i++)
				{
					glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO[i]);
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

					glBindBuffer(GL_ARRAY_BUFFER, skyBoxUV);
					glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, cubeTexture[i]);

					glUniform1i(glGetUniformLocation(skyBoxProgramID, "skybox"), 0);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyBoxIdx);
					glDrawElements(GL_TRIANGLES, skyBoxOBJ->indexCount * 3, GL_UNSIGNED_INT, 0);
				}

				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_TRUE);



				/****************************************************
				 *
				 *           Light Rendering
				 *
				 ***************************************************/


				glUseProgram(normalProgramID);
				cam->ControlView(0.0f, dt);

				GLuint normColorLocation = glGetUniformLocation(normalProgramID, "normColor");
				GLuint normalMLocation = glGetUniformLocation(normalProgramID, "model");
				GLuint normalVLocation = glGetUniformLocation(normalProgramID, "view");
				GLuint normalPLocation = glGetUniformLocation(normalProgramID, "proj");

				glm::vec3 lightPosVector[MAX_LIGHTS];
				for (int i = 0; i < numLights; i++)
				{
					float angle = i * (glm::radians(360.f / MAX_LIGHTS)) + rot;

					spheres->SetPosition(glm::vec3(cosf(angle) * orbitSize, 0.0f, sinf(angle) * orbitSize));
					glm::mat4 sphModel = spheres->GetObjectMatrix();

					
					sphModel = glm::scale(sphModel, glm::vec3(sphScale));

					glUniform3f(normColorLocation, lightColorVector[i].x, lightColorVector[i].y, lightColorVector[i].z);

					glUniformMatrix4fv(normalMLocation, 1, GL_FALSE, glm::value_ptr(sphModel));
					glUniformMatrix4fv(normalVLocation, 1, GL_FALSE, glm::value_ptr(envView));
					glUniformMatrix4fv(normalPLocation, 1, GL_FALSE, glm::value_ptr(envProj));

					lightPosVector[i] = spheres->GetPosition();

					glBindVertexArray(vao);
					glBindBuffer(GL_ARRAY_BUFFER, sphVbo);
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

					glBindBuffer(GL_ARRAY_BUFFER, sphVn);
					glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphIbo);
					glDrawElements(GL_TRIANGLES, spheres->GetOBJ()->indexCount * 3, GL_UNSIGNED_INT, 0);
				}
				

			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}


		/****************************************************
		*
		*           normal Rendering
		*
		***************************************************/
		glUseProgram(normalProgramID);
		OBJ* obj = it->GetOBJ();
		// Normal uniforms
		glUniformMatrix4fv(normalMLocation, 1, GL_FALSE, glm::value_ptr(objModel));
		glUniformMatrix4fv(normalVLocation, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(normalPLocation, 1, GL_FALSE, glm::value_ptr(proj));
		glEnableVertexAttribArray(0);
		glUniform3f(normColorLocation, 0.0f, 1.0f, 0.0f);

		if (showVn == true)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffers[count].vno);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glDrawArrays(GL_LINES, 0, (GLsizei)obj->vertexNormal.size());
		}

		//glUniform3f(normColorLocation, 0.0f, 1.0f, 1.0f);
		if (showFn == true)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffers[count].fno);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glDrawArrays(GL_LINES, 0, (GLsizei)obj->faceNormal.size());
		}
		count++;

	}

	/****************************************************
	*
	*           orbit  Rendering
	*
	***************************************************/

	glm::mat4 otransform = glm::mat4(1.0f);
	glm::mat4 oscaler = glm::mat4(1.0f);
	glm::mat4 orotation = glm::mat4(1.0f);

	glm::vec3 oscaling = glm::vec3(orbitSize);
	oscaler = glm::scale(oscaler, oscaling);
	orotation = glm::rotate(oscaler, glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));
	otransform = glm::translate(orotation, glm::vec3(0.0f, 0.0f, 0.0f));

	glm::mat4 omodel = otransform;

	glUniform3f(normColorLocation, 1.0f, 1.0f, 1.0f);
	glUniformMatrix4fv(normalMLocation, 1, GL_FALSE, glm::value_ptr(omodel));
	glUniformMatrix4fv(normalVLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(normalPLocation, 1, GL_FALSE, glm::value_ptr(proj));

	glBindBuffer(GL_ARRAY_BUFFER, orbitVbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_LINES, 0, (GLsizei)orbitVertSize);


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(currWindow);
	glfwPollEvents();

	return 0;
}

void SimpleScene::SetUpObjBuffer(OBJ* obj, ObjectBuffer& buffer)
{
	size_t verticesSize = obj->vertices.size() * sizeof(glm::vec3);
	// vertex buffer object
	glGenBuffers(1, &buffer.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
	glBufferData(GL_ARRAY_BUFFER, verticesSize, &obj->vertices[0], GL_STATIC_DRAW);

	size_t indicesSize = obj->indices.size() * sizeof(glm::ivec3);
	// index buffer object
	glGenBuffers(1, &buffer.idx);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.idx);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, &obj->indices[0], GL_STATIC_DRAW);

	size_t vnSize = obj->vertexNormal.size() * sizeof(glm::vec3);
	// vertex normal object.
	glGenBuffers(1, &buffer.vno);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.vno);
	glBufferData(GL_ARRAY_BUFFER, vnSize, &obj->vertexNormal[0], GL_DYNAMIC_DRAW);

	size_t fnSize = obj->faceNormal.size() * sizeof(glm::vec3);
	// face normal object
	glGenBuffers(1, &buffer.fno);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.fno);
	glBufferData(GL_ARRAY_BUFFER, fnSize, &obj->faceNormal[0], GL_DYNAMIC_DRAW);

	size_t pvnSize = obj->pureVertexNormal.size() * sizeof(glm::vec3);
	glGenBuffers(1, &buffer.pureVn);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.pureVn);
	glBufferData(GL_ARRAY_BUFFER, pvnSize, &obj->pureVertexNormal[0], GL_STATIC_DRAW);
}

void SimpleScene::SetUpUV(OBJ* obj, ObjectBuffer& buffer)
{
	size_t uvSize = obj->vertices.size() * sizeof(glm::vec2);
	glGenBuffers(1, &buffer.uv);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.uv);

	std::vector<glm::vec2> uvVector;
	if (entityPos == true)
	{
		uvVector = CalUv(obj->vertices);
	}
	else if (entityNor == true)
	{
		uvVector = CalUv(obj->pureVertexNormal);
	}

	glBufferData(GL_ARRAY_BUFFER, uvSize, uvVector.data(), GL_STATIC_DRAW);
}

std::vector<glm::vec2> SimpleScene::CalUv(const std::vector<glm::vec3>& entity)
{
	std::vector<glm::vec2> result;
	for (auto& it : entity)
	{
		glm::vec3 normEntity = glm::normalize(it);
		glm::vec3 absVec = glm::abs(normEntity);
		glm::vec2 uv = glm::vec2(0.0);
		if (isPlanarMap == true)
		{
			if (absVec.x >= absVec.y && absVec.x >= absVec.z)
			{
				(normEntity.x < 0.0) ? (uv.x = normEntity.z) : (uv.x = -normEntity.z);
				uv.y = normEntity.y;
			}
			else if (absVec.y >= absVec.x && absVec.y >= absVec.z)
			{
				(normEntity.y < 0.0) ? (uv.y = normEntity.z) : (uv.y = -normEntity.z);
				uv.x = normEntity.x;
			}
			else if (absVec.z >= absVec.y && absVec.z >= absVec.x)
			{
				(normEntity.z < 0.0) ? (uv.x = -normEntity.x) : (uv.x = normEntity.x);
				uv.y = normEntity.y;
			}

			result.push_back((uv + glm::vec2(1.0)) / 2.f);
		}
		else if (isCylin == true)
		{
			float theta = atan(normEntity.y / normEntity.x);
			float z = (normEntity.z - (-1.0f)) / 2.0f;
			uv = glm::vec2(theta / glm::radians(360.f), z);

			result.push_back((uv + glm::vec2(1.0)) / 2.f);
		}
		else if (isSph == true)
		{
			float theta = atan(normEntity.y / normEntity.x);
			float r = sqrt(normEntity.x * normEntity.x + normEntity.y * normEntity.y + normEntity.z * normEntity.z);
			float phi = acos(normEntity.z / r);

			uv = glm::vec2(theta / glm::radians(360.f), (glm::radians(180.f) - phi) / glm::radians(180.f));
			result.push_back((uv + glm::vec2(1.0)) / 2.f);
		}
	}

	return result;
}

#pragma warning(pop)