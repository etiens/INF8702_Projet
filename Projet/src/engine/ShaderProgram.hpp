/*
 * ShaderProgram.h
 *
 *  Created on: 2014-11-23
 *      Author: red
 */

#pragma once

#include <map>
#include <string>
#include <exception>
#include <list>

#include <glload/gl_all.h>
#include <glutil/Shader.h>

class ShaderProgramUniformNotFoundException : std::exception
{
public:
	ShaderProgramUniformNotFoundException(std::string aUniformName);
	virtual const char* what() const _GLIBCXX_USE_NOEXCEPT;
private:
	std::string aWhat;
};

class ShaderProgramUniformNotLoadedException : std::exception
{
public:
	ShaderProgramUniformNotLoadedException(std::string aUniformName);
	virtual const char* what() const _GLIBCXX_USE_NOEXCEPT;
private:
	std::string aWhat;
};

class ShaderProgramUniformMapEmptyException : std::exception
{
public:
	ShaderProgramUniformMapEmptyException();
	virtual const char* what() const _GLIBCXX_USE_NOEXCEPT;
private:
	std::string aWhat;
};

class ShaderProgramUniformBlockBindingPointExhausted : std::exception
{
	virtual const char* what() const _GLIBCXX_USE_NOEXCEPT
	{ return "Error : no more Uniform block binding point available!!"; }
};

class ShaderProgram
{
public:
	ShaderProgram(std::string aVertexShaderPath, std::string aFragmentShaderPath);
	virtual ~ShaderProgram();

	void loadUniform(std::string aUniformName);
	GLint getUniform(std::string aUniformName) const;

	void loadUniformBloc(std::string aUniformName);
	GLint getUniformBloc(std::string aUniformBlockName) const;

	GLuint getProgram() const;

	static void initGlobalUniformBlock();
	static GLuint getNextGlobalUniformBlockBindingPointAvailable();

	static void freeGlobalUniformBlockBindingPoint(GLuint aBindingPoint);

private:
	GLuint loadShaderFromFile(GLenum aShaderType, const std::string aFilePath);
	ShaderProgram();

	glutil::UniqueShader mFragmentShader;
	glutil::UniqueShader mVertexShader;
	glutil::UniqueProgram mProgram;

	typedef std::string UniformName;
	std::map<UniformName, GLint> mUniformMap;

	typedef std::string UniformBlocName;
	std::map<UniformBlocName, GLint> mUniformBlocMap;

	static std::list<GLuint> mGlobalUniformBlockBindingPointAvailable;
};

