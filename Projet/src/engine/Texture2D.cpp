/*
 * Texture.cpp
 *
 *  Created on: 2014-11-25
 *      Author: red
 */

#include "Texture2D.hpp"
#include "../Utils.hpp"

#include <iostream>

Texture2D::Texture2D(std::string aFilename, unsigned int aDefaultTextureUnit) :
	Texture(aDefaultTextureUnit)
{
	//TODO Prise en charge des mipmaps

	try
	{
		mImageSet.reset(glimg::loaders::stb::LoadFromFile(aFilename));
		mTextureID = glimg::CreateTexture(mImageSet.get(), 0);

		printOpenGLError();

		setRepeat(GL_REPEAT, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4 - 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		printOpenGLError();

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	catch(glimg::loaders::stb::StbLoaderException &e)
	{
		std::cerr << e.what() << std::endl;
	}
}
// Wrapper around a user-defined texture
Texture2D::Texture2D(GLuint aTextureID, unsigned int aDefaultTextureUnit) :
	Texture(aDefaultTextureUnit)
{
	mTextureID = aTextureID;
}
Texture2D::~Texture2D()
{
	//TODO FREE OGL TEXTURE RESOURCE
	//done in base classe...
}

void Texture2D::setRepeat(GLenum aRepeat_S, GLenum aRepeat_T)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, aRepeat_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, aRepeat_T);
}
