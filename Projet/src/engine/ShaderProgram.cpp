/*
 * ShaderProgram.cpp
 *
 *  Created on: 2014-11-23
 *      Author: red
 */

#include <fstream>
#include <sstream>

#include "ShaderProgram.hpp"
#include "../Utils.hpp"

//Guaranted to be at least 1024, so reserver memory at least...
std::list<GLuint> ShaderProgram::mGlobalUniformBlockBindingPointAvailable(1024);

//////////////////////
//EXCEPTIONS
ShaderProgramUniformNotFoundException::ShaderProgramUniformNotFoundException(std::string aUniformName)
{
	aWhat = std::string("Error :: Uniform ") + aUniformName + std::string(" was not found! Maybe it was trunked down due to optimisation...");
}
const char* ShaderProgramUniformNotFoundException::what() const _GLIBCXX_USE_NOEXCEPT
{
	return aWhat.c_str();
}

ShaderProgramUniformNotLoadedException::ShaderProgramUniformNotLoadedException(std::string aUniformName)
{
	aWhat = std::string("Error :: Cannot load uniform ") + aUniformName;
}
const char* ShaderProgramUniformNotLoadedException::what() const _GLIBCXX_USE_NOEXCEPT
{
	return aWhat.c_str();
}

ShaderProgramUniformMapEmptyException::ShaderProgramUniformMapEmptyException()
{
	aWhat = std::string("Error :: Uniform std::map is empty. Have you loaded some uniforms?");
}
const char* ShaderProgramUniformMapEmptyException::what() const _GLIBCXX_USE_NOEXCEPT
{
	return aWhat.c_str();
}


//////////////////////
//ShaderProg
ShaderProgram::ShaderProgram(std::string aVertexShaderPath, std::string aFragmentShaderPath)
{
	//std::cout << "Loading/compiling shaders...\n";

	//Load shaders into unique_ptr like struct (destroy shaders when out of scope)
	std::cout << "Compiling vertex shader : " << aVertexShaderPath << "..." << std::endl;
 	mVertexShader.reset(loadShaderFromFile(GL_VERTEX_SHADER,
			aVertexShaderPath));
 	std::cout << "Compiling vertex shader : " << aVertexShaderPath << " done!" << std::endl;

	printOpenGLError();

	std::cout << "Compiling fragment shader : " << aFragmentShaderPath << "..." << std::endl;
	mFragmentShader.reset(loadShaderFromFile(GL_FRAGMENT_SHADER,
			aFragmentShaderPath));
	std::cout << "Compiling fragment shader : " << aFragmentShaderPath << " done!" << std::endl;
	printOpenGLError();

	try
	{
		std::cout << "Linking shaders..." << std::endl;
		mProgram.reset(glutil::LinkProgram(mVertexShader, mFragmentShader));
		std::cout << "Linking shaders done!" << std::endl;
		printOpenGLError();
	} catch (glutil::CompileLinkException &e)
	{
		std::cerr << e.what() << std::endl;
	}
}

ShaderProgram::~ShaderProgram()
{
	//FREE ALL OPENGL ressources (the program, the shaders, etc)
	//Nothing to do because its in unique_ptr like struct ;)
}

void ShaderProgram::loadUniformBloc(std::string aUniformName)
{
	GLint uniformLoc = glGetUniformBlockIndex(mProgram,
			aUniformName.c_str());
	if (uniformLoc == (GLint)GL_INVALID_INDEX)
	{
		throw ShaderProgramUniformNotFoundException(aUniformName);
	}

	mUniformBlocMap[aUniformName] = uniformLoc;
}

GLint ShaderProgram::getUniformBloc(std::string aUniformName) const
{
	if(mUniformBlocMap.empty())
	{
		throw ShaderProgramUniformMapEmptyException();
	}
	auto it = mUniformBlocMap.find(aUniformName);
	if( mUniformBlocMap.find(aUniformName) == mUniformBlocMap.end())
	{
		throw ShaderProgramUniformNotLoadedException(aUniformName);
	}
	return it->second;
}

void ShaderProgram::loadUniform(std::string aUniformName)
{
	//std::cout << "Bindings static uniforms..." << std::endl;
	glUseProgram(getProgram());
	GLint uniformLoc = glGetUniformLocation(mProgram,
			aUniformName.c_str());
	if (uniformLoc == (GLint)GL_INVALID_INDEX)
	{
		throw ShaderProgramUniformNotFoundException(aUniformName);
	}

	mUniformMap[aUniformName] = uniformLoc;
	glUseProgram(0);
}

GLint ShaderProgram::getUniform(std::string aUniformName) const
{
	if(mUniformMap.empty())
	{
		throw ShaderProgramUniformMapEmptyException();
	}
	auto it = mUniformMap.find(aUniformName);
	if( mUniformMap.find(aUniformName) == mUniformMap.end())
	{
		throw ShaderProgramUniformNotLoadedException(aUniformName);
	}
	return it->second;
}

GLuint ShaderProgram::getProgram() const
{
	return (GLuint)mProgram;
}


GLuint ShaderProgram::loadShaderFromFile(GLenum aShaderType, const std::string aFilePath)
{
	std::ifstream shaderFile(aFilePath.c_str());
	if (shaderFile.good())
	{
		std::stringstream buffer;
		buffer << shaderFile.rdbuf();
		std::string shaderFileString = buffer.str();

		shaderFile.close();

		try
		{
			return glutil::CompileShader(aShaderType, shaderFileString);
			printOpenGLError();
		} catch (glutil::CompileLinkException &e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
	else
	{
		std::cerr << "Error : Cannot load vertex shader file!!" << std::endl;
		exit(-1);
	}
	return -1;
}

void ShaderProgram::initGlobalUniformBlock()
{
	GLint result;
	glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &result);
	std::cout << "Info : Max uniform block binding point available : " << result << std::endl;

	//Lol code de bouette yolo
	mGlobalUniformBlockBindingPointAvailable.resize(result);
	auto it = mGlobalUniformBlockBindingPointAvailable.begin();
	for(int i = 0; i < result; ++i)
	{
		*(it++) = i;
	}
}

GLuint ShaderProgram::getNextGlobalUniformBlockBindingPointAvailable()
{
	if(mGlobalUniformBlockBindingPointAvailable.empty())
	{ throw ShaderProgramUniformBlockBindingPointExhausted(); }
	GLuint index = mGlobalUniformBlockBindingPointAvailable.front();
	mGlobalUniformBlockBindingPointAvailable.pop_front();
	return index;
}

void ShaderProgram::freeGlobalUniformBlockBindingPoint(GLuint aBindingPoint)
{
	mGlobalUniformBlockBindingPointAvailable.emplace_front(aBindingPoint);
}
