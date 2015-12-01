/*
 * LightManager.hpp
 *
 *  Created on: 2014-11-28
 *      Author: red
 */

#pragma once

#include <memory>
#include <string>

#include <glload/gl_all.h>
#include <glm/glm.hpp>

#include "Light.hpp"
#include "ShaderProgram.hpp"

class LightManager
{
public:
	//!TODO make singleton

	LightManager(size_t aMaxLights) :
		mLightUniformLightBlocName(""),
		mMaxLights(aMaxLights),
		mUniformBlockIndex(-1),
		mUniformBlockBindingIndex(-1),
		mUniformBufferIndex(0)
	{}
	virtual ~LightManager();

	Light* buildNewPointLight(
			glm::vec3 aPosition,
			glm::vec3 aColor,
			glm::vec3 aAmbiant,
			float aConstantAttenuation = 1.0f,
			float aLinearAttenuation = 0.0f,
			float aQuadraticAttenuation = 0.0f
			);

	Light* buildNewSpotLight(
			glm::vec3 aPosition,
			glm::vec3 aColor,
			glm::vec3 aAmbiant,
			glm::vec3 aConeDirection,
			float aSpotCosCutoff,
			float aSpotExponent,
			float aConstantAttenuation = 1.0f,
			float aLinearAttenuation = 0.0f,
			float aQuadraticAttenuation = 0.0f
			);

	Light* buildNewDirectionalLight(
			glm::vec3 aColor,
			glm::vec3 aAmbiant,
			glm::vec3 aDirection,
			glm::vec3 aHalfVector
			);


	inline void setUniformLightBlocName(std::string aUniformLightBlocName)
	{ mLightUniformLightBlocName = aUniformLightBlocName; }

	void initializeUniformLightBloc(ShaderProgram& aProgram, std::string aUniformLightBlocName);

	inline size_t getMaxLights()
	{ return mMaxLights;}

	void uploadLightToShader(ShaderProgram& aProgram, const Light& aLight, unsigned int aLightIndex);
	void deactivateLight(ShaderProgram& aProgram, unsigned int aLightIndex);

private:

	std::string mLightUniformLightBlocName;
	size_t mMaxLights;

	GLint mUniformBlockIndex;
	GLint mUniformBlockBindingIndex;
	GLuint mUniformBufferIndex;
};
