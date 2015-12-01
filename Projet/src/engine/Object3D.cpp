/*
 * Object3D.cpp
 *
 *  Created on: 2014-11-29
 *      Author: red
 */

#include "Object3D.hpp"

const char* Object3DInvalidFormatException::what() const _GLIBCXX_USE_NOEXCEPT
{
	return "Error : The vertex format is invalid! Must be sizeof(vertex) == sizeof(normals) == sizeof(colors) == sizeof(texCoord)";
}

Object3D::~Object3D()
{
	// TODO Auto-generated destructor stub
	//Nothing to do because of smart pointers
}


Object3D::Object3D(std::string aFilePath) // load from OBJ
{
	std::vector<vertex> vertexBuffer;
	std::vector<normal> normalBuffer;
	std::vector<texCoord2D> texCoordBuffer;
	if(!loadOBJ(aFilePath.c_str(), vertexBuffer, texCoordBuffer, normalBuffer))
	{
		throw Object3DCannotLoadModel();
	}

	std::vector<colorRGBA> colorBuffer(vertexBuffer.size(), colorRGBA(0xFF, 0xFF, 0xFF, 0xFF)); // Un beau light gray pour la couleur par defaut des OBJ
	buildMesh(vertexBuffer, normalBuffer, colorBuffer, texCoordBuffer);
}

void Object3D::buildMesh(std::vector<vertex>& aVertexBuffer, std::vector<normal>& aNormalBuffer, std::vector<colorRGBA>& aColorBuffer ,std::vector<texCoord2D>& aTexCoordBuffer)
{
	glmesh::AttributeList attribs;
	//First attribute is attribute index 0, a vec3 of floats. POSITION
	attribs.push_back(glmesh::AttribDesc(0, 3, glmesh::VDT_SINGLE_FLOAT, glmesh::ADT_FLOAT));
	//Second attribute is attribute index 1, a vec4 of normalized, unsigned bytes. COLOR
	attribs.push_back(glmesh::AttribDesc(1, 4, glmesh::VDT_UNSIGN_BYTE, glmesh::ADT_NORM_FLOAT));
	//Third attribute is index 2, a vec3 for the normal
	attribs.push_back(glmesh::AttribDesc(2, 3, glmesh::VDT_SINGLE_FLOAT, glmesh::ADT_FLOAT));
	//Third attribute is index 3, a vec3 for the tangeant
	attribs.push_back(glmesh::AttribDesc(3, 3, glmesh::VDT_SINGLE_FLOAT, glmesh::ADT_FLOAT));
	//Third attribute is index 4, a vec3 for the bitangeant
	attribs.push_back(glmesh::AttribDesc(4, 3, glmesh::VDT_SINGLE_FLOAT, glmesh::ADT_FLOAT));
	//Fifth attribute is the texture coordinate
	attribs.push_back(glmesh::AttribDesc(5, 2, glmesh::VDT_SINGLE_FLOAT, glmesh::ADT_FLOAT));

	//attribs is no longer necessary; all of the info is stored in vfmt.
	glmesh::VertexFormat vfmt(attribs);

	size_t vertexBufferSize = aVertexBuffer.size();
	if(vertexBufferSize != aNormalBuffer.size() || vertexBufferSize != aColorBuffer.size() || vertexBufferSize != aTexCoordBuffer.size())
	{
		throw Object3DInvalidFormatException();
	}

	//Now, use a CpuDataWriter to create our buffer object.
	glmesh::CpuDataWriter writer(vfmt, aVertexBuffer.size()); //4 is just a hint; it's non-binding.
	for(unsigned int i = 0; i < vertexBufferSize; ++i)
	{
		writer.Attrib(aVertexBuffer[i]); //Pos
		writer.Attrib(aColorBuffer[i]);  //Color
		writer.Attrib(aNormalBuffer[i]); //Normal
		writer.Attrib(glm::vec3(0.0f));  //Tangeant
		writer.Attrib(glm::vec3(0.0f));  //Bitangeant
		writer.Attrib(aTexCoordBuffer[i]);
	}

	GLuint bufferObject = writer.TransferToBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	//We're done with writer now.
	//Create a set of mesh variant VAOs. We will have two:
	//one for position+color, and one for just position.
	GLuint vaos[2];
	glGenVertexArrays(2, vaos);
	//Must bind the buffer, so that the VAOs can store the buffer.
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject);
	//Has both attributes.
	glBindVertexArray(vaos[0]);
	vfmt.BindAttributes(0);
	//Has only attribute index 0.
	glBindVertexArray(vaos[1]);
	vfmt.BindAttribute(0, 0); //Just one attribute.
	//Done with the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glmesh::MeshVariantMap variants;
	variants["all"] = vaos[0];
	variants["pos-only"] = vaos[1];
	//Create the rendering command, as a X-element triangle (sizeof aVertexBuffer ).
	glmesh::RenderCmdList cmdList;
	cmdList.DrawArrays(GL_TRIANGLES, 0, aVertexBuffer.size());
	//Build the mesh and return it.
	std::vector<GLuint> buffers(1, bufferObject);
	mMeshPtr.reset(new glmesh::Mesh(buffers, vaos[0], cmdList, variants));

}

Object3D::Object3D(std::vector<vertex>& aVertexBuffer, std::vector<normal>& aNormalBuffer, std::vector<colorRGBA>& aColorBuffer, std::vector<texCoord2D>& aTexCoordBuffer)
{
	buildMesh(aVertexBuffer, aNormalBuffer, aColorBuffer, aTexCoordBuffer);
}

void Object3D::draw()
{
	mMeshPtr->Render();
}


void Object3D::createPlane()
{
	std::vector<vertex> vertexBuffer;
	std::vector<normal> normalBuffer;
	std::vector<texCoord2D> texCoordBuffer;

	//First triangle
	vertexBuffer.push_back(vertex(-1.0f,-1.0f, 0.0f)); //Pos
	normalBuffer.push_back(normal(0.0f, 0.0f, 1.0f));
	texCoordBuffer.push_back(texCoord2D(0.0f, 0.0f));  //texCoord

	vertexBuffer.push_back(vertex(1.0f,-1.0f, 0.0f)); //Pos
	normalBuffer.push_back(normal(0.0f, 0.0f, 1.0f));
	texCoordBuffer.push_back(texCoord2D(1.0f, 0.0f));  //texCoord

	vertexBuffer.push_back(vertex(-1.0f,1.0f, 0.0f)); //Pos
	normalBuffer.push_back(normal(0.0f, 0.0f, 1.0f));
	texCoordBuffer.push_back(texCoord2D(0.0f, 1.0f));  //texCoord

	//Second triangle
	vertexBuffer.push_back(vertex(-1.0f,1.0f, 0.0f)); //Pos
	normalBuffer.push_back(normal(0.0f, 0.0f, 1.0f));
	texCoordBuffer.push_back(texCoord2D(0.0f, 1.0f));  //texC

	vertexBuffer.push_back(vertex(1.0f,-1.0f, 0.0f)); //Pos
	normalBuffer.push_back(normal(0.0f, 0.0f, 1.0f));
	texCoordBuffer.push_back(texCoord2D(1.0f, 0.0f));  //texCoord

	vertexBuffer.push_back(vertex(1.0f,1.0f, 0.0f)); //Pos
	normalBuffer.push_back(normal(0.0f, 0.0f, 1.0f));
	texCoordBuffer.push_back(texCoord2D(1.0f, 1.0f));  //texCoord

	std::vector<colorRGBA> colorBuffer(vertexBuffer.size(), colorRGBA(0xAA, 0xAA, 0xAA, 0xFF)); // Un beau light gray pour la couleur par defaut
	buildMesh(vertexBuffer, normalBuffer, colorBuffer, texCoordBuffer);
}
