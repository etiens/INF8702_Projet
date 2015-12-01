/*
 * Texture.hpp
 *
 *  Created on: 2014-11-25
 *      Author: red
 */

#pragma once

#include <SFML/Window.hpp>
#include <string>
#include <memory>

#include <glload/gl_all.h>
#include <glimg/glimg.h>

#include "Texture.hpp"
#include "ShaderProgram.hpp"

class Texture2D : public Texture
{
public:
	Texture2D(std::string aFilename, unsigned int aDefaultTextureUnit);
	Texture2D(GLuint aTextureID, unsigned int aDefaultTextureUnit);
	virtual ~Texture2D();

	void setRepeat(GLenum aRepeat_S, GLenum aRepeat_T);

	inline virtual GLenum getTextureType()
	{ return GL_TEXTURE_2D; }

private:
	Texture2D();
};
