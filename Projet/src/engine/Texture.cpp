/*
 * Texture.cpp
 *
 *  Created on: 2014-11-25
 *      Author: red
 */

#include "Texture.hpp"
#include "../Utils.hpp"

Texture::Texture(unsigned int aDefaultTextureUnit) :
	mTextureID(0),
	mImageSet(),
	mTextureUnit(aDefaultTextureUnit)
{
}
Texture::~Texture()
{
	glDeleteTextures(1, &mTextureID);
	printOpenGLError();
}

void Texture::bind()
{
	glActiveTexture( GL_TEXTURE0 + getTextureUnitIndex() );
	printOpenGLError();
	glEnable(GL_TEXTURE_2D);
	printOpenGLError();
	glBindTexture(getTextureType(), mTextureID);
	printOpenGLError();
}

void Texture::unBind()
{
//	glActiveTexture(GL_TEXTURE0 + getTextureUnitIndex());
//	printOpenGLError();
//	glDisable(GL_TEXTURE_2D);
//	printOpenGLError();
	glBindTexture(getTextureType(), 0);
	printOpenGLError();
}

GLuint Texture::getTextureID()
{
	return mTextureID;
}
