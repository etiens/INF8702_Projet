/*
 * Light.hpp
 *
 *  Created on: 2014-11-27
 *      Author: red
 */

#pragma once

#include <glload/gl_all.h>
#include <glm/glm.hpp>

/* In fragment shader, corresponding is :
	const uint MAX_LIGHTS = 10;
	layout(std140) uniform LightProperties
	{
		vec3 ambiant;
		bool isEnabled;
		vec3 color;  //Because its dumb to have a separate color for specular AND diffuse....like, who use that?!
		bool isSpot;
		vec3 position; //If isDirection, this is the direction of the light
		bool isDirectional;
		vec3 halfVector; //for directional light highlight
		float spotCosCutoff;
		vec3 coneDirection;
		float spotExponent;
		float constantAttenuation;
		float linearAttenuation;
		float quadraticAttenuation;
	} Lights[MAX_LIGHTS];
 */

struct Light
{
	//Because LightManager is a Light builder
	friend class LightManager;

public:
	Light applyTransformationMatrix(const glm::mat4& aTransformationMatrix); //Useful to transform light in eye space
	glm::vec4 getPos() const
	{ return mPosition; }
private:
	Light();

	Light(
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
	) ;

	//This layout (symmetirc to the shader's struct and aligned) permits to use std140 uniform blocs
	glm::vec4 mAmbiant;
	glm::vec4 mColor;
	glm::vec4 mPosition;
	glm::vec4 mHalfVector;
	glm::vec4 mConeDirection;

	GLint mIsEnabled;
	GLint mIsSpot;
	GLint mIsDirectional;
	float mSpotCosCutoff;

	float mSpotExponent;
	float mConstantAttenuation;
	float mLinearAttenuation;
	float mQuadraticAttenuation;

};
