/*
Light.cpp
 *
 *  Created on: 2014-11-27
 *      Author: red
 */

#include "Light.hpp"

#include <cstring>

Light Light::applyTransformationMatrix(const glm::mat4& aTransformationMatrix)
{
	glm::vec4 eyePosition = aTransformationMatrix*mPosition;
	glm::vec4 eyeConeDirection = aTransformationMatrix*mConeDirection;
	glm::vec4 eyeHalfVector = aTransformationMatrix*mHalfVector;

	Light eyeCoordLight(*this);

	eyeCoordLight.mPosition = eyePosition;
	eyeCoordLight.mConeDirection = eyeConeDirection;
	eyeCoordLight.mHalfVector = eyeHalfVector;

	return eyeCoordLight;
}

Light::Light(
		GLint aIsEnabled,
		GLint aIsDirectional,
		GLint aIsSpot,
		glm::vec3 aAmbiant,
		glm::vec3 aColor,
		glm::vec3 aPosition,
		glm::vec3 aHalfVector,
		glm::vec3 aConeDirection,
		float aSpotCosCutoff,
		float aSpotExponent,
		float aConstantAttenuation,
		float aLinearAttenuation,
		float aQuadraticAttenuation
	) :
	mAmbiant(glm::vec4(aAmbiant,0.0f)),
	mColor(glm::vec4(aColor,0.0f)),
	mPosition(glm::vec4(aPosition,0.0f)),
	mHalfVector(glm::vec4(aHalfVector,0.0f)),
	mConeDirection(glm::vec4(aConeDirection,0.0f)),

	mIsEnabled(aIsEnabled),
	mIsSpot(aIsSpot),
	mIsDirectional(aIsDirectional),
	mSpotCosCutoff(aSpotCosCutoff),

	mSpotExponent(aSpotExponent),
	mConstantAttenuation(aConstantAttenuation),
	mLinearAttenuation(aLinearAttenuation),
	mQuadraticAttenuation(aQuadraticAttenuation)
{}

/*
 * glm::vec4 mAmbiant;
	glm::vec4 mColor;
	glm::vec4 mPosition;
	glm::vec4 mHalfVector;
	glm::vec4 mConeDirection;

	bool mIsEnabled;
	bool mIsSpot;
	bool mIsDirectional;
	float mSpotCosCutoff;

	float mSpotExponent;
	float mConstantAttenuation;
	float mLinearAttenuation;
	float mQuadraticAttenuation;
 */
