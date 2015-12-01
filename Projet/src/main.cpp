//============================================================================
// Name        : Particles_fun.cpp
// Author      : Lrouge
// Version     :
// Copyright   : This program is free
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <ctime>

#include <SFML/Window.hpp>

#include <glload/gl_all.h>
#include <glload/gl_load.hpp>
#include <glutil/Debug.h>
#include <glutil/MatrixStack.h>
#include <glutil/MousePoles.h>
#include <glmesh/glmesh.h>

#include "engine/ShaderProgram.hpp"
#include "engine/Texture2D.hpp"
#include "engine/LightManager.hpp"
#include "engine/Light.hpp"
#include "engine/Object3D.hpp"

#include "Utils.hpp"

#define DEBUG

glutil::MatrixStack perspectiveMatrixStack;
glutil::MatrixStack modelMatrixStack;
glutil::MatrixStack viewMatrixStack;

//Unique_ptr
std::unique_ptr<ShaderProgram> sceneShaderProgram;
std::unique_ptr<ShaderProgram> bloomProgram;
std::unique_ptr<ShaderProgram> brightProgram;
std::unique_ptr<ShaderProgram> postprocessProgram;
std::unique_ptr<ShaderProgram> debugProgram;
std::unique_ptr<ShaderProgram> additionProgram;
std::unique_ptr<ShaderProgram> godRaysProgram;
std::unique_ptr<Object3D> cube;
std::unique_ptr<Object3D> monkey;
std::unique_ptr<Object3D> floorPlane;
std::unique_ptr<Object3D> postProcessPlane;
std::unique_ptr<Object3D> pillar;
std::unique_ptr<Object3D> sphere;
std::unique_ptr<Light> spotLight;
std::unique_ptr<Light> directionalLight;
const unsigned int POINT_LIGHT_NB = 5;
std::unique_ptr<Light> pointLights[POINT_LIGHT_NB];
bool pointLightEnabled[POINT_LIGHT_NB];
std::shared_ptr<Texture2D> brickTexture;
std::shared_ptr<Texture2D> grassTexture;
std::shared_ptr<Texture2D> woodTexture;
std::shared_ptr<Texture2D> checkerTexture;
std::shared_ptr<Texture2D> sceneRenderTexture;
std::shared_ptr<Texture2D> brightRenderTexture;
std::shared_ptr<Texture2D> bloomRenderTexture;
std::shared_ptr<Texture2D> additionRenderTexture;
std::shared_ptr<Texture2D> godRaysRenderTexture;

const unsigned int MAX_LIGHT = 10;
LightManager lightManager(MAX_LIGHT);

bool globalDebugLight = false;
bool globalBloom = true;
bool globalGodRays = true;
bool globalScene = true;

void initShaders()
{
	std::cout << "Loading shaders..." << std::endl;

	sceneShaderProgram.reset(new ShaderProgram("shaders/basic.vs", "shaders/basic.fs"));
	bloomProgram.reset(new ShaderProgram("shaders/simple.vs", "shaders/bloom.fs"));
	brightProgram.reset(new ShaderProgram("shaders/simple.vs", "shaders/brightPass.fs"));
	additionProgram.reset(new ShaderProgram("shaders/simple.vs", "shaders/addition.fs"));
	postprocessProgram.reset(new ShaderProgram("shaders/simple.vs", "shaders/postprocess.fs"));
	debugProgram.reset(new ShaderProgram("shaders/debug.vs", "shaders/debug.fs"));
	godRaysProgram.reset(new ShaderProgram("shaders/simple.vs", "shaders/godRays.fs"));
	printOpenGLError();
	try{
		//Vertex shader uniforms
		sceneShaderProgram->loadUniform("perspectiveMatrix");
		sceneShaderProgram->loadUniform("modelViewMatrix");
		sceneShaderProgram->loadUniform("normalMatrix");

		//Fragment shader uniforms
		sceneShaderProgram->loadUniform("EyeDirection");
		sceneShaderProgram->loadUniform("Shininess");
		sceneShaderProgram->loadUniform("Strenght");
		sceneShaderProgram->loadUniform("baseColorFactor");
		sceneShaderProgram->loadUniform("textureSampler");

		//Fragment shader uniforms
		bloomProgram->loadUniform("textureSampler");

		brightProgram->loadUniform("textureSampler");

		postprocessProgram->loadUniform("textureSampler");

		additionProgram->loadUniform("textureSampler1");
		additionProgram->loadUniform("textureSampler2");
		additionProgram->loadUniform("textureSampler3");
		additionProgram->loadUniform("showScene");
		additionProgram->loadUniform("showBloom");
		additionProgram->loadUniform("showGodRays");

		debugProgram->loadUniform("perspectiveMatrix");
		debugProgram->loadUniform("modelViewMatrix");

		godRaysProgram->loadUniform("textureSampler");
		//godRaysProgram->loadUniform("lightPosition");

		printOpenGLError();
	}
	catch (ShaderProgramUniformNotFoundException& e)
	{
		std::cerr << e.what() << std::endl;
	}
	std::cout << "Loading shaders done!" << std::endl;
}

static const unsigned int checkerData[] =
{
		0xFFFFFFFF, 0xBBBBBBFF, 0xFFFFFFFF, 0xBBBBBBFF,
		0xBBBBBBFF, 0xFFFFFFFF, 0xBBBBBBFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xBBBBBBFF, 0xFFFFFFFF, 0xBBBBBBFF,
		0xBBBBBBFF, 0xFFFFFFFF, 0xBBBBBBFF, 0xFFFFFFFF
};
void createEmptyTexture(GLuint textureID){

	glBindTexture( GL_TEXTURE_2D, textureID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );



}
void initTextures()
{
	std::cout << "Init textures..." << std::endl;

	brickTexture.reset(new Texture2D("textures/bricks.jpg", 0));
	grassTexture.reset(new Texture2D("textures/grass.jpg", 0));
	woodTexture.reset(new Texture2D("textures/wood.jpg", 0));
	brickTexture->setRepeat(GL_REPEAT, GL_REPEAT);
	grassTexture->setRepeat(GL_REPEAT, GL_REPEAT);
	woodTexture->setRepeat(GL_REPEAT, GL_REPEAT);

	// Creating an empty texture for render
	GLuint sceneRenderTextureID;
	glGenTextures( 1, &sceneRenderTextureID );
	createEmptyTexture(sceneRenderTextureID);
	sceneRenderTexture.reset(new Texture2D(sceneRenderTextureID, 1));
	glBindTexture(GL_TEXTURE_2D, 0);

	// Creating an empty texture for bright pass
	GLuint brightRenderTextureID;
	glGenTextures( 1, &brightRenderTextureID );
	createEmptyTexture(brightRenderTextureID);
	brightRenderTexture.reset(new Texture2D(brightRenderTextureID, 2));
	glBindTexture(GL_TEXTURE_2D, 0);

	// Creating an empty texture for bloom
	GLuint bloomRenderTextureID;
	glGenTextures( 1, &bloomRenderTextureID );
	createEmptyTexture(bloomRenderTextureID);
	bloomRenderTexture.reset(new Texture2D(bloomRenderTextureID, 3));
	glBindTexture(GL_TEXTURE_2D, 0);

	// Creating an empty texture for addition
	GLuint additionRenderTextureID;
	glGenTextures( 1, &additionRenderTextureID );
	createEmptyTexture(additionRenderTextureID);
	additionRenderTexture.reset(new Texture2D(additionRenderTextureID, 4));
	glBindTexture(GL_TEXTURE_2D, 0);

	// Creating an empty texture for god rays
	GLuint godRaysRenderTextureID;
	glGenTextures( 1, &godRaysRenderTextureID );
	createEmptyTexture(godRaysRenderTextureID);
	godRaysRenderTexture.reset(new Texture2D(godRaysRenderTextureID, 5));
	glBindTexture(GL_TEXTURE_2D, 0);

	//Create checker texture
	GLuint checkerTextureID;
	glGenTextures( 1, &checkerTextureID );
	glBindTexture( GL_TEXTURE_2D, checkerTextureID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 4, 4, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, checkerData );

	checkerTexture.reset(new Texture2D(checkerTextureID, 6));
	glBindTexture(GL_TEXTURE_2D, 0);

	std::cout << "Init texture done!" << std::endl;
}

void initMesh()
{
	std::cout << "Building mesh..." << std::endl;

	cube.reset(new Object3D("models/cube.obj"));

	monkey.reset(new Object3D("models/suzanne.obj"));

	pillar.reset(new Object3D("models/pillar.obj"));

	floorPlane.reset(new Object3D());
	floorPlane->createPlane();

	postProcessPlane.reset(new Object3D());
	postProcessPlane->createPlane();

	sphere.reset(new Object3D());
	sphere->createSphere(5,5);

	std::cout << "Building mesh done!" << std::endl;

}

bool checkFBOStatus(GLenum status){
	// On v�rifie les erreurs � la suite de la cr�ation du FBO
	bool returnValue = false;

	switch( status )
	{
	case GL_FRAMEBUFFER_COMPLETE:
		returnValue = true;
	  break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
	  std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
	  break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cerr << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl;
	  break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl;
	  break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT" << std::endl;
	  break;
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT" << std::endl;
	  break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << std::endl;
	  break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << std::endl;
	  break;
	default:
		std::cerr << "ERREUR INCONNUE" << std::endl;
	}
	return returnValue;
}


void createFBO(GLuint frameBufferID, GLuint renderBufferDepthID, GLuint textureID){

	std::cout << "Init FBO..." << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
	// Bind the texture to the FBO
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureID, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferDepthID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
	//-------------------------
	//Attach depth buffer to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferDepthID);

	checkFBOStatus(glCheckFramebufferStatus( GL_FRAMEBUFFER ));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

GLuint sceneFrameBuffer;
GLuint sceneBufferDepth;
GLuint brightFrameBuffer;
GLuint brightBufferDepth;
GLuint bloomFrameBuffer;
GLuint bloomBufferDepth;
GLuint additionFrameBuffer;
GLuint additionBufferDepth;
GLuint godRaysFrameBuffer;
GLuint godRaysBufferDepth;
void initFBOs(){
	glGenFramebuffers(1, &sceneFrameBuffer);
	glGenRenderbuffers(1, &sceneBufferDepth);
	createFBO(sceneFrameBuffer, sceneBufferDepth, sceneRenderTexture->getTextureID());

	glGenFramebuffers(1, &brightFrameBuffer);
	glGenRenderbuffers(1, &brightBufferDepth);
	createFBO(brightFrameBuffer, brightBufferDepth, brightRenderTexture->getTextureID());

	glGenFramebuffers(1, &bloomFrameBuffer);
	glGenRenderbuffers(1, &bloomBufferDepth);
	createFBO(bloomFrameBuffer, bloomBufferDepth, bloomRenderTexture->getTextureID());

	glGenFramebuffers(1, &additionFrameBuffer);
	glGenRenderbuffers(1, &additionBufferDepth);
	createFBO(additionFrameBuffer, additionBufferDepth, additionRenderTexture->getTextureID());

	glGenFramebuffers(1, &godRaysFrameBuffer);
	glGenRenderbuffers(1, &godRaysBufferDepth);
	createFBO(godRaysFrameBuffer, godRaysBufferDepth, godRaysRenderTexture->getTextureID());

	printOpenGLError();
	std::cout << "Init FBO done!" << std::endl;

}
glutil::ViewPole *cameraViewPole;

void initCameraViewPole()
{
	glutil::ViewData viewData;
	viewData.targetPos = glm::vec3(0.0f, 0.0f, 0.0f);
	viewData.orient = glm::fquat(glm::vec3(PI/2.7f, 0.0f, 0.0f));
	viewData.radius = 5.0f;
	viewData.degSpinRotation = 0.0f;

	glutil::ViewScale viewScale;
	viewScale.minRadius = 1.0f;
	viewScale.maxRadius = 100.0f;
	viewScale.largeRadiusDelta = 1.0f;
	viewScale.smallRadiusDelta = 0.5f;
	viewScale.largePosOffset = 5.0f;
	viewScale.smallPosOffset = 0.5f;
	viewScale.rotationScale = 0.5f;

	cameraViewPole = new glutil::ViewPole(viewData, viewScale,
			glutil::MB_LEFT_BTN, false);
}

void initLight()
{
	std::cout << "Init lights..." << std::endl;
	std::cout << "Number of lights used :" << lightManager.getMaxLights() << std::endl;

	glUseProgram(sceneShaderProgram->getProgram());

	glUniform3f(sceneShaderProgram->getUniform("EyeDirection"), 0.0f, 0.0f, 1.0f);
	glUniform1f(sceneShaderProgram->getUniform("Shininess"), 2000.0f);
	glUniform1f(sceneShaderProgram->getUniform("Strenght"), 1.0f);

	//Put this to 1 if you want base color, 0 if you want texture, and a value between for a mix
	glUniform1f(sceneShaderProgram->getUniform("baseColorFactor"), 0.0f);

	//glUseProgram(0);

	lightManager.initializeUniformLightBloc(*sceneShaderProgram, "LightUniformBlock");


	glm::vec3 directionalLightDir = glm::vec3(0.2f, 0.2f, 0.4f);
	glm::vec3 halfVector = glm::reflect(directionalLightDir, glm::vec3(0.0f, 1.0f, 0.0f));
	directionalLight.reset(lightManager.buildNewDirectionalLight(
			directionalLightDir,
			glm::vec3(0.2f, 0.2f, 0.2f),
			glm::vec3(0.0f, -1.0f, -1.0f),
			halfVector
			));


//	spotLight.reset(lightManager.buildNewSpotLight(
//			glm::vec3(0.0f, 0.0f, 1.0f),
//			glm::vec3(0.8f, 0.8f, 0.8f),
//			glm::vec3(0.1f),
//			glm::vec3(0.0f, 0.0f, -1.0f),
//			0.995f,
//			0.1f,
//			1.0f,
//			0.5f,
//			0.4f
//			));

	pointLights[0].reset(lightManager.buildNewPointLight(
			glm::vec3(0.0f, 3.0f, -2.0f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			glm::vec3(0.1f),
			1.0f,
			0.0f,
			0.0f
			));


	pointLights[1].reset(lightManager.buildNewPointLight(
				glm::vec3(5.0f, 3.0f, -2.0f),
				glm::vec3(0.7f, 0.3f, 0.3f),
				glm::vec3(0.1f),
				1.0f,
				0.0f,
				0.0f
				));


	pointLights[2].reset(lightManager.buildNewPointLight(
				glm::vec3(3.0f, 5.0f, -2.0f),
				glm::vec3(0.3f, 0.7f, 0.3f),
				glm::vec3(0.1f),
				1.0f,
				0.0f,
				0.0f
				));


	pointLights[3].reset(lightManager.buildNewPointLight(
				glm::vec3(10.0f, 10.0f, -5.0f),
				glm::vec3(0.9f, 0.9f, 0.9f),
				glm::vec3(0.1f),
				1.0f,
				0.0f,
				0.0f
				));

	pointLights[4].reset(lightManager.buildNewPointLight(
			glm::vec3(-10.0f, 10.0f, -2.0f),
			glm::vec3(0.3f, 0.3f, 0.9f),
			glm::vec3(0.1f),
			1.0f,
			0.0f,
			0.0f
			));

	for(int i = 0; i < POINT_LIGHT_NB; ++i)
	{
		pointLightEnabled[i] = false;
	}
	pointLightEnabled[3] = true;

	std::cout << "Init lights done!" << std::endl;
}

void init()
{
	//Init shaders must be done first, we need uniform locations everywhere...
	ShaderProgram::initGlobalUniformBlock();
	printOpenGLError();
	initShaders();
	printOpenGLError();
	initLight();
	printOpenGLError();
	initTextures();
	printOpenGLError();
	initMesh();
	printOpenGLError();
	initFBOs();
	printOpenGLError();

	//The camera viewpole is the rotating camera
	initCameraViewPole();

	//Set matrix with something...note that modelView is replaced later with the CameraPole
	perspectiveMatrixStack.Perspective(80, 800.0f/600.0f, 0.1, 100);

	modelMatrixStack.SetIdentity();
	viewMatrixStack.SetIdentity();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	printOpenGLError();
}


void debugDrawLight(const Light& light)
{
	glUseProgram(debugProgram->getProgram());
	printOpenGLError();

	{
		//PushStack pushes the current matrix and pops it in the destructor.
		glutil::PushStack pushM(modelMatrixStack);
		glutil::PushStack pushV(viewMatrixStack);

		glm::vec4 lightPos(light.getPos());
		modelMatrixStack.Translate(lightPos.x, lightPos.y, lightPos.z);
		modelMatrixStack.Scale(0.3f);

		viewMatrixStack.ApplyMatrix(cameraViewPole->CalcMatrix());

		try
		{
			glUniformMatrix4fv(debugProgram->getUniform("perspectiveMatrix"), 1, GL_FALSE,
					glm::value_ptr(perspectiveMatrixStack.Top()));
			printOpenGLError();

			glUniformMatrix4fv(debugProgram->getUniform("modelViewMatrix"), 1, GL_FALSE,
					glm::value_ptr(viewMatrixStack.Top() * modelMatrixStack.Top()));
			printOpenGLError();


		}
		catch(ShaderProgramUniformNotLoadedException &e)
		{
			std::cerr << e.what() << std::endl;
		}
		// Render into FBO
		sphere->draw(); //Same as "all", because that's what we used for the main VAO.
		printOpenGLError();

	}
	glUseProgram(0);
}

void adjustLightPosition(bool debugPrint)
{ //best code ever, give me an award plz
	if(pointLights)
	{
		glm::mat4 pointLightTransformation;

		for(int i = 0; i < POINT_LIGHT_NB; ++i)
		{
			if(pointLightEnabled[i])
			{
				lightManager.uploadLightToShader(
					*sceneShaderProgram,
					pointLights[i]->applyTransformationMatrix(pointLightTransformation),
					i
					);

				if(debugPrint)
				{
					debugDrawLight(pointLights[i]->applyTransformationMatrix(pointLightTransformation));
				}
			}
			else
			{
				lightManager.deactivateLight(*sceneShaderProgram, i);
			}
		}
	}

	if(directionalLight)
	{
		glm::mat4 directionalLightTransformation;

		lightManager.uploadLightToShader(
			*sceneShaderProgram,
			directionalLight->applyTransformationMatrix(directionalLightTransformation),
			POINT_LIGHT_NB
			);

		if(debugPrint)
		{
			debugDrawLight(directionalLight->applyTransformationMatrix(directionalLightTransformation));
		}
	}

	if(spotLight)
	{
		glm::mat4 spotLightTransformation;

		lightManager.uploadLightToShader(
			*sceneShaderProgram,
			spotLight->applyTransformationMatrix(spotLightTransformation),
			POINT_LIGHT_NB+1
			);

		if(debugPrint)
		{
			debugDrawLight(spotLight->applyTransformationMatrix(spotLightTransformation));
		}
	}
}

void renderScene()
{
	adjustLightPosition(globalDebugLight);

	glUseProgram(sceneShaderProgram->getProgram());
	printOpenGLError();

	//Render the monkey
	{
		//PushStack pushes the current matrix and pops it in the destructor.
		glutil::PushStack pushM(modelMatrixStack);
		glutil::PushStack pushV(viewMatrixStack);

		viewMatrixStack.ApplyMatrix(cameraViewPole->CalcMatrix());

		// Calculate the normal matrix
		glm::mat4 invertedModel = glm::inverse(modelMatrixStack.Top());
		glm::mat4 normalMatrix = glm::transpose(invertedModel);

		try
		{
			glUniformMatrix4fv(sceneShaderProgram->getUniform("perspectiveMatrix"), 1, GL_FALSE,
					glm::value_ptr(perspectiveMatrixStack.Top()));
			printOpenGLError();

			glUniformMatrix4fv(sceneShaderProgram->getUniform("modelViewMatrix"), 1, GL_FALSE,
					glm::value_ptr(viewMatrixStack.Top() * modelMatrixStack.Top()));
			printOpenGLError();

			glUniformMatrix4fv(sceneShaderProgram->getUniform("normalMatrix"), 1, GL_FALSE,
					glm::value_ptr(normalMatrix));
			printOpenGLError();

			//Put this to 1 if you want base color, 0 if you want texture, and a value between for a mix
			glUniform1f(sceneShaderProgram->getUniform("baseColorFactor"), 0.0f);
			printOpenGLError();

			brickTexture->bind();
			printOpenGLError();

			glUniform1i(sceneShaderProgram->getUniform("textureSampler"), brickTexture->getTextureUnitIndex());
			printOpenGLError();
		}
		catch(ShaderProgramUniformNotLoadedException &e)
		{
			std::cerr << e.what() << std::endl;
		}
		// Render into FBO
		monkey->draw(); //Same as "all", because that's what we used for the main VAO.
		printOpenGLError();

		brickTexture->unBind();
		printOpenGLError();
	}

	//Render the pillar
	{
		//PushStack pushes the current matrix and pops it in the destructor.
		glutil::PushStack pushM(modelMatrixStack);
		glutil::PushStack pushV(viewMatrixStack);

		viewMatrixStack.ApplyMatrix(cameraViewPole->CalcMatrix());

		modelMatrixStack.Translate(3.0f, 0.01f, -3.0f);

		// Calculate the normal matrix
		glm::mat4 invertedModel = glm::inverse(modelMatrixStack.Top());
		glm::mat4 normalMatrix = glm::transpose(invertedModel);

		try
		{
			glUniformMatrix4fv(sceneShaderProgram->getUniform("perspectiveMatrix"), 1, GL_FALSE,
					glm::value_ptr(perspectiveMatrixStack.Top()));
			printOpenGLError();

			glUniformMatrix4fv(sceneShaderProgram->getUniform("modelViewMatrix"), 1, GL_FALSE,
					glm::value_ptr(viewMatrixStack.Top() * modelMatrixStack.Top()));
			printOpenGLError();

			glUniformMatrix4fv(sceneShaderProgram->getUniform("normalMatrix"), 1, GL_FALSE,
					glm::value_ptr(normalMatrix));
			printOpenGLError();

			//Put this to 1 if you want base color, 0 if you want texture, and a value between for a mix
			glUniform1f(sceneShaderProgram->getUniform("baseColorFactor"), 0.0f);
			printOpenGLError();

			woodTexture->bind();
			printOpenGLError();

			glUniform1i(sceneShaderProgram->getUniform("textureSampler"), woodTexture->getTextureUnitIndex());
			printOpenGLError();
		}
		catch(ShaderProgramUniformNotLoadedException &e)
		{
			std::cerr << e.what() << std::endl;
		}
		// Render into FBO
		pillar->draw(); //Same as "all", because that's what we used for the main VAO.
		printOpenGLError();

		woodTexture->unBind();
		printOpenGLError();
	}

	//Render the floor
	{
		//PushStack pushes the current matrix and pops it in the destructor.
		glutil::PushStack pushM(modelMatrixStack);
		glutil::PushStack pushV(viewMatrixStack);

		viewMatrixStack.ApplyMatrix(cameraViewPole->CalcMatrix());

		modelMatrixStack.Translate(0.0f, -1.0f, 0.0f);
		modelMatrixStack.RotateX(90.0f);
		modelMatrixStack.Scale(20.0f);

		// Calculate the normal matrix
		glm::mat4 invertedModel = glm::inverse(modelMatrixStack.Top());
		glm::mat4 normalMatrix = glm::transpose(invertedModel);

		try
		{
			glUniformMatrix4fv(sceneShaderProgram->getUniform("perspectiveMatrix"), 1, GL_FALSE,
					glm::value_ptr(perspectiveMatrixStack.Top()));
			printOpenGLError();

			glUniformMatrix4fv(sceneShaderProgram->getUniform("modelViewMatrix"), 1, GL_FALSE,
					glm::value_ptr(viewMatrixStack.Top() * modelMatrixStack.Top()));
			printOpenGLError();

			glUniformMatrix4fv(sceneShaderProgram->getUniform("normalMatrix"), 1, GL_FALSE,
					glm::value_ptr(normalMatrix));
			printOpenGLError();

			//Put this to 1 if you want base color, 0 if you want texture, and a value between for a mix
			glUniform1f(sceneShaderProgram->getUniform("baseColorFactor"), 0.0f);
			printOpenGLError();

			grassTexture->bind();
			printOpenGLError();

			glUniform1i(sceneShaderProgram->getUniform("textureSampler"), grassTexture->getTextureUnitIndex());
			printOpenGLError();
		}
		catch(ShaderProgramUniformNotLoadedException &e)
		{
			std::cerr << e.what() << std::endl;
		}

		floorPlane->draw(); //Same as "all", because that's what we used for the main VAO.
		printOpenGLError();

		grassTexture->unBind();
		printOpenGLError();
	}

}

void clearBuffer() {

	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

// Second render pass with bright shader
// This shader will only keep parts that are very bright
void brightPass()
{

	glUseProgram(brightProgram->getProgram());
	glUniform1i(brightProgram->getUniform("textureSampler"), sceneRenderTexture->getTextureUnitIndex());

	glBindFramebuffer(GL_FRAMEBUFFER, brightFrameBuffer);
	clearBuffer();
	brightRenderTexture->bind();
	postProcessPlane->draw();
}

// Third render pass with bloom shader
// This pass takes the result of the bright pass
// and blurs it, creating a blooming effect
void bloomPass()
{
	glUseProgram(bloomProgram->getProgram());
	glUniform1i(bloomProgram->getUniform("textureSampler"), brightRenderTexture->getTextureUnitIndex());

	glBindFramebuffer(GL_FRAMEBUFFER, bloomFrameBuffer);
	clearBuffer();
	bloomRenderTexture->bind();
	postProcessPlane->draw();
}

void godRaysPass(){
	//Second render pass with bloom shader
	glUseProgram(godRaysProgram->getProgram());
	glUniform1i(godRaysProgram->getUniform("textureSampler"), brightRenderTexture->getTextureUnitIndex());
	// Assign position to world-to-screen coordinates for a light (pointLight 3? It's the brightest)
	glm::vec2 position;
	// Uncomment line 117
	// Uncomment next line
	// Uncomment line 16 from godRays.fs
	// delete line 17 from godRays.fs
	//glUniform2f(godRaysProgram->getUniform("lightPosition"), position.x, position.y);


	glBindFramebuffer(GL_FRAMEBUFFER, godRaysFrameBuffer);
	clearBuffer();
	//Draw the postprocessing plane into the FBO
	godRaysRenderTexture->bind();
	postProcessPlane->draw();
}

void additionPass()
{
	//Second render pass with bloom shader
	glUseProgram(additionProgram->getProgram());
	glUniform1i(additionProgram->getUniform("textureSampler1"), sceneRenderTexture->getTextureUnitIndex());
	glUniform1i(additionProgram->getUniform("textureSampler2"), bloomRenderTexture->getTextureUnitIndex());
	glUniform1i(additionProgram->getUniform("textureSampler3"), godRaysRenderTexture->getTextureUnitIndex());
	glUniform1i(additionProgram->getUniform("showScene"), globalScene);
	glUniform1i(additionProgram->getUniform("showBloom"), globalBloom);
	glUniform1i(additionProgram->getUniform("showGodRays"), globalGodRays);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	clearBuffer();
	//Draw the postprocessing plane into the FBO
	additionRenderTexture->bind();
	postProcessPlane->draw();
	additionRenderTexture->unBind();
}

void postProcessing()
{
	brightPass();
	bloomPass();
	godRaysPass();
	additionPass();
}

double  delta = 0;
double  currentTime = clock();
double FPS = 0;
void calculateFPS(){
	delta += clock() - currentTime;
	currentTime = clock();
    FPS++;
    if (delta/CLOCKS_PER_SEC > 1){
    	std::cout << FPS << std::endl;
    	delta = 0;
    	FPS = 0;
    }
}
//Called to update the display.
void display()
{

	// This little code will output the current FPS for the program.

	//calculateFPS();

	if (globalBloom || globalGodRays){
		// Render scene into FBO
		glBindFramebuffer(GL_FRAMEBUFFER, sceneFrameBuffer);
	}else{
		// Render scene directly to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	clearBuffer();
	glViewport(0,0,800,600);

	sceneRenderTexture->bind();
	renderScene();
	sceneRenderTexture->unBind();

	if (globalBloom || globalGodRays){
		postProcessing();
	}

}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

int main(int argc, char** argv)
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 3;

	sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default,
			settings);
	settings = window.getSettings();

	std::cout << "depth bits:" << settings.depthBits << std::endl;
	std::cout << "stencil bits:" << settings.stencilBits << std::endl;
	std::cout << "antialiasing level:" << settings.antialiasingLevel
			<< std::endl;
	std::cout << "version:" << settings.majorVersion << "."
			<< settings.minorVersion << std::endl;

	bool running = true;

	//Loading opengl functions
	if (!glload::LoadFunctions())
	{
		return -1;
	}

	printOpenGLError();

	//Initialize stuff
	try
	{
		//or at least try...
		init();
		printOpenGLError();
	} catch (...)
	{
		std::cerr << "Ouups! init() went wrong!" << std::endl;
		exit(-1);
	}

	//Main loop
	while (running)
	{

		sf::Event event;

		// while there are pending events...
		while (window.pollEvent(event))
		{
			// check the type of the event...

			if (event.type == sf::Event::Closed) // window closed
			{
				window.close();
				exit(0);
			}
			else if (event.type == sf::Event::TextEntered) // key pressed
			{
				if (event.text.unicode < 128)
				{
					std::cout << "ASCII character typed: "
							<< static_cast<char>(event.text.unicode)
							<< std::endl;
					cameraViewPole->CharPress(
							static_cast<char>(event.text.unicode));
				}
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if(event.key.code == sf::Keyboard::L)
				{
					globalDebugLight = !globalDebugLight;
				}
				if(event.key.code == sf::Keyboard::Num1)
				{
					pointLightEnabled[0] = !pointLightEnabled[0];
				}
				if(event.key.code == sf::Keyboard::Num2)
				{
					pointLightEnabled[1] = !pointLightEnabled[1];
				}
				if(event.key.code == sf::Keyboard::Num3)
				{
					pointLightEnabled[2] = !pointLightEnabled[2];
				}
				if(event.key.code == sf::Keyboard::Num4)
				{
					pointLightEnabled[3] = !pointLightEnabled[3];
				}
				if(event.key.code == sf::Keyboard::Num5)
				{
					pointLightEnabled[4] = !pointLightEnabled[4];
				}
				if(event.key.code == sf::Keyboard::B)
				{
					globalBloom = !globalBloom;
				}
				if(event.key.code == sf::Keyboard::G)
				{
					globalGodRays = !globalGodRays;
				}
				if(event.key.code == sf::Keyboard::H)
				{
					globalScene = !globalScene;
				}

			}
			else if (event.type == sf::Event::MouseWheelMoved)
			{

				std::cout << "wheel movement: " << event.mouseWheel.delta
						<< std::endl;

				int direction = (event.mouseWheel.delta > 0) ? 1 : -1;

				int modifier = glm::abs(event.mouseWheel.delta);
				cameraViewPole->MouseWheel(direction, modifier,
						glm::ivec2(0, 0)); //Not sure why ivec2 is needed....doesn't do anything
			}
			else if (event.type == sf::Event::MouseMoved)
			{
				if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					std::cout << "Mouse mouved with left button clicked" << std::endl;
					std::cout << "new mouse x: " << event.mouseMove.x << std::endl;
					std::cout << "new mouse y: " << event.mouseMove.y << std::endl;

					cameraViewPole->MouseMove(glm::ivec2(event.mouseMove.x, event.mouseMove.y));
				}
			}else if (event.type == sf::Event::MouseButtonPressed)
			{
			    if (event.mouseButton.button == sf::Mouse::Left)
			    {
			        std::cout << "the left button was pressed" << std::endl;
			        cameraViewPole->MouseClick(glutil::MB_LEFT_BTN, true, 0x0, glm::ivec2(event.mouseMove.x, event.mouseMove.y));
			    }
			}else if (event.type == sf::Event::MouseButtonReleased)
			{
			    if (event.mouseButton.button == sf::Mouse::Left)
			    {
			        std::cout << "the left button was released" << std::endl;
			        //cameraViewPole->MouseClick(glutil::MB_LEFT_BTN, false, 0x0, glm::ivec2(event.mouseMove.x, event.mouseMove.y));
			    }
			}
		}

		try
		{
			display();
		}
		catch(...)
		{
			std::cerr << "An exception has occured in display()" << std::endl;
			exit(-1);
		}

		window.display();
	}

	delete cameraViewPole;
	return 0;
}

