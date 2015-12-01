/*
 * Utils.hpp
 *
 *  Created on: 2014-11-30
 *      Author: red
 */

#pragma once

#include <glm/glm.hpp>

#include <string>

/////////////////
//Defines utilises pour le projet

#define PI 3.14159265358979323846264338327950288

typedef glm::vec3 vertex;
typedef glm::vec3 normal;
typedef glm::vec3 texCoord3D;
typedef glm::vec2 texCoord2D;

typedef glm::detail::tvec4<unsigned char> colorRGBA;


#define printOpenGLError() printOglError(__FILE__, __LINE__)

int printOglError(std::string, int line);
