/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SimpleScene.h
Purpose: To create some object with OBJ file reader.
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_2
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 9/21/2021
End Header --------------------------------------------------------*/
#ifndef SIMPLE_SCENE_H
#define SIMPLE_SCENE_H
#include <glm/detail/type_vec3.hpp>

#include "../Scene.h"
#include <vector>

struct OBJ;
struct Texture;
class Camera;
class Object;

class SimpleScene : public Scene
{
public:
	SimpleScene(GLFWwindow* window, int windowWidth, int windowHeight);
	~SimpleScene();
	void SetupNanoGUI(GLFWwindow* pWwindow) override;
	void InitMembers();
	void CleanUp() override;
	int Init() override;
	void LoadAllShaders() override;
	int Display(float dt) override;
	void DisplaySetting();

	void SelectMode();




private:
	int windowWidth, windowHeight;
	float zNear, zFar;
	// program IDs.
	GLuint phongShadingID;
	GLuint phongLightingID;
	GLuint blinShadingID;
	GLuint normalProgramID;
	GLuint currentProgramID;
	GLuint skyBoxProgramID;

	GLuint vao;

	struct ObjectBuffer
	{
		GLuint      vbo = 0;				// vertex buffer object.
		GLuint		vno = 0;				// vertex normal object.
		GLuint		fno = 0;				// face normal object;
		GLuint      cbf = 0;				// color buffer.
		GLuint      idx = 0;				// index buffer.
		GLuint		pureVn = 0;			    // normal buffer for lighting
		GLuint		pureFn = 0;
		GLuint		uv = 0;
	};

	void SetUpObjBuffer(OBJ* obj, ObjectBuffer& buffer);
	void SetUpUV(OBJ* obj, ObjectBuffer& buffer);
	std::vector<glm::vec2> CalUv(const std::vector<glm::vec3>& entity);
	enum LightType { L_EMPTY = -1, L_POINT = 0, L_DIR, L_SPOT };

	ObjectBuffer* buffers;

	GLuint sphVbo;
	GLuint sphIbo;
	GLuint sphVn;
	Object* spheres;

	GLuint orbitVbo;
	size_t orbitVertSize;

	GLuint texture1;
	GLuint texture2;

	Camera* cam;
	Camera* fbCam[6];

	//skybox

	typedef enum {FRONT = 0, RIGHT, BACK, LEFT, TOP, BOTTOM} SKYBOX;

	GLuint skyBoxVBO[6];
	GLuint skyBoxUV;
	GLuint skyBoxVAO;
	GLuint skyBoxIdx;
	GLuint cubeTexture[6];


	//Environment

	GLuint fbo;
	GLuint rbo;
	GLuint fbTexture[6];


	// ImGui control panel

	bool isFill;
	bool showFn;
	bool showVn;
	bool showObj;
	bool isPlanarMap;
	bool isCylin;
	bool isSph;
	bool entityPos;
	bool entityNor;
	bool isCpu;
	bool isGpu;
};


#endif

