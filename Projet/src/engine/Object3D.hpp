/*
 * Object3D.hpp
 *
 *  Created on: 2014-11-29
 *      Author: red
 */

#pragma once

#include <memory>
#include <vector>
#include <exception>

#include <glload/gl_all.h>
#include <glm/glm.hpp>
#include <glmesh/glmesh.h>

#include "Texture2D.hpp"
#include "../Utils.hpp"
#include "OBJLoader.hpp"

class Object3DInvalidFormatException : std::exception
{
	const char* what() const _GLIBCXX_USE_NOEXCEPT;
};

class Object3DCannotLoadModel : std::exception
{
	const char* what() const _GLIBCXX_USE_NOEXCEPT
	{ return "Error : Cannot load obj model"; }
};

class Object3D
{
public:

	Object3D(std::vector<vertex>& aVertexBuffer, std::vector<normal>& aNormalBuffer, std::vector<colorRGBA>& aColorBuffer, std::vector<texCoord2D>& aTexCoordBuffer);
	Object3D(std::string aFilePath); // load from OBJ
	Object3D()
	{}

	void createPlane(size_t aXGridGranularity, size_t aYGridGranularity)
	{ mMeshPtr.reset(glmesh::gen::GroundPlane(aXGridGranularity, aYGridGranularity, true)); }
	void createCube(std::vector<colorRGBA> &colorVector)
	{ mMeshPtr.reset(glmesh::gen::Cube(colorVector)); }
	void createSphere(size_t aXGridGranularity, size_t aYGridGranularity)
	{ mMeshPtr.reset(glmesh::gen::UnitSphere(aXGridGranularity, aYGridGranularity)); }
	void createPlane();


	virtual ~Object3D();

	void draw();

private:

	void buildMesh(std::vector<vertex>& aVertexBuffer, std::vector<normal>& aNormalBuffer, std::vector<colorRGBA>& aColorBuffer, std::vector<texCoord2D>& aTexCoordBuffer);

	std::unique_ptr<glmesh::Mesh> mMeshPtr;
};
