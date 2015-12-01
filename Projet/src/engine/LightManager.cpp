/*
 * LightManager.cpp
 *
 *  Created on: 2014-11-28
 *      Author: red
 */

#include <cstring>
#include <iostream>
#include <cstdlib>

#include "LightManager.hpp"

LightManager::~LightManager()
{
	glDeleteBuffers(1, &mUniformBufferIndex);
}

Light* LightManager::buildNewPointLight(
		glm::vec3 aPosition,
		glm::vec3 aColor,
		glm::vec3 aAmbiant,
		float aConstantAttenuation,
		float aLinearAttenuation,
		float aQuadraticAttenuation
		)
{
	GLint IsDirectional = GL_FALSE;
	GLint IsSpot = GL_FALSE;
	glm::vec3 Ambiant = aAmbiant;
	glm::vec3 Color = aColor;
	glm::vec3 Position = aPosition;
	glm::vec3 HalfVector = glm::vec3(0.0f);
	glm::vec3 ConeDirection = glm::vec3(0.0f);
	float SpotCosCutoff(0.0f);
	float SpotExponent(0.0f);
	float ConstantAttenuation = aConstantAttenuation;
	float LinearAttenuation = aLinearAttenuation;
	float QuadraticAttenuation = aQuadraticAttenuation;

	return new Light(
			GL_TRUE,
			IsDirectional,
			IsSpot,
			Ambiant,
			Color,
			Position,
			HalfVector,
			ConeDirection,
			SpotCosCutoff,
			SpotExponent,
			ConstantAttenuation,
			LinearAttenuation,
			QuadraticAttenuation
			);
}

Light* LightManager::buildNewSpotLight(
		glm::vec3 aPosition,
		glm::vec3 aColor,
		glm::vec3 aAmbiant,
		glm::vec3 aConeDirection,
		float aSpotCosCutoff,
		float aSpotExponent,
		float aConstantAttenuation,
		float aLinearAttenuation,
		float aQuadraticAttenuation
		)
{
	GLint IsDirectional = GL_FALSE;
	GLint IsSpot = GL_TRUE;
	glm::vec3 Ambiant = aAmbiant;
	glm::vec3 Color = aColor;
	glm::vec3 Position = aPosition;
	glm::vec3 HalfVector = glm::vec3(0.0f);
	glm::vec3 ConeDirection = aConeDirection;
	float SpotCosCutoff = aSpotCosCutoff;
	float SpotExponent = aSpotExponent;
	float ConstantAttenuation = aConstantAttenuation;
	float LinearAttenuation = aLinearAttenuation;
	float QuadraticAttenuation = aQuadraticAttenuation;

	return new Light(
			GL_TRUE,
			IsDirectional,
			IsSpot,
			Ambiant,
			Color,
			Position,
			HalfVector,
			ConeDirection,
			SpotCosCutoff,
			SpotExponent,
			ConstantAttenuation,
			LinearAttenuation,
			QuadraticAttenuation
			);
}

Light* LightManager::buildNewDirectionalLight(
		glm::vec3 aColor,
		glm::vec3 aAmbiant,
		glm::vec3 aDirection,
		glm::vec3 aHalfVector
		)
{
	GLint IsDirectional = GL_TRUE;
	GLint IsSpot = GL_FALSE;
	glm::vec3 Ambiant = aAmbiant;
	glm::vec3 Color = aColor;
	glm::vec3 Position = glm::normalize(aDirection); //As this will serve as light direction
	glm::vec3 HalfVector = aHalfVector;
	glm::vec3 ConeDirection;
	float SpotCosCutoff(0.0f);
	float SpotExponent(0.0f);
	float ConstantAttenuation(1.0f);
	float LinearAttenuation(0.0f);
	float QuadraticAttenuation(0.0f);

	return new Light(
			GL_TRUE,
			IsDirectional,
			IsSpot,
			Ambiant,
			Color,
			Position,
			HalfVector,
			ConeDirection,
			SpotCosCutoff,
			SpotExponent,
			ConstantAttenuation,
			LinearAttenuation,
			QuadraticAttenuation
			);
}

void LightManager::initializeUniformLightBloc(ShaderProgram& aProgram, std::string aUniformLightBlocName)
{
	mLightUniformLightBlocName = aUniformLightBlocName;

	try
	{
		aProgram.loadUniformBloc(aUniformLightBlocName);
		mUniformBlockIndex = aProgram.getUniformBloc(aUniformLightBlocName);
		mUniformBlockBindingIndex = ShaderProgram::getNextGlobalUniformBlockBindingPointAvailable();
		glUniformBlockBinding(aProgram.getProgram(), mUniformBlockIndex, mUniformBlockBindingIndex);

		glGenBuffers(1, &mUniformBufferIndex);
		glBindBuffer(GL_UNIFORM_BUFFER, mUniformBufferIndex);

		//Init the buffer with zero everywhere
		size_t uniformBufferSize = sizeof(Light)*mMaxLights;
		char* zeroBuf = new char[uniformBufferSize]();

		glBufferData(GL_UNIFORM_BUFFER, uniformBufferSize, zeroBuf, GL_DYNAMIC_DRAW);

		delete[] zeroBuf;

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, mUniformBlockBindingIndex, mUniformBufferIndex, 0, uniformBufferSize);
	}
	catch(ShaderProgramUniformNotLoadedException &e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch(ShaderProgramUniformNotFoundException &e)
	{
		std::cerr << e.what() << std::endl;
	}

}

void LightManager::uploadLightToShader(ShaderProgram& aProgram, const Light& aLight, unsigned int aLightIndex)
{
	glBindBuffer(GL_UNIFORM_BUFFER, mUniformBufferIndex);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Light)*aLightIndex, sizeof(Light), &aLight);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightManager::deactivateLight(ShaderProgram& aProgram, unsigned int aLightIndex)
{
	char* zeroBuf = new char[sizeof(Light)]();
	glBindBuffer(GL_UNIFORM_BUFFER, mUniformBufferIndex);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Light)*aLightIndex, sizeof(Light), zeroBuf);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	delete[] zeroBuf;
}


