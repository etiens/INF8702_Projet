/*
 * Texture.hpp
 *
 *  Created on: 2014-11-25
 *      Author: red
 */

#pragma once

#include <string>
#include <memory>

#include <glload/gl_all.h>
#include <glimg/glimg.h>

#include "ShaderProgram.hpp"

class Texture
{
public:
	Texture(unsigned int aDefaultTextureUnit);
	virtual ~Texture();

	void bind();
	void unBind();

	GLuint getTextureID();

	virtual GLenum getTextureType() = 0;

	inline void setTextureUnit(unsigned int aTextureUnit)
	{ mTextureID = aTextureUnit; }

	inline unsigned int getTextureUnitIndex()
	{ return mTextureUnit; }

protected:
	GLuint mTextureID;
	std::unique_ptr<glimg::ImageSet> mImageSet;
	unsigned int mTextureUnit;
};
