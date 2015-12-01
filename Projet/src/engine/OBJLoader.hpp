/*
 * OBJLoader.hpp
 *
 *  Created on: 2014-12-01
 *      Author: red
 */

#pragma once

#include "../Utils.hpp"

bool loadOBJ(
		const char * path,
		std::vector<vertex> & out_vertices,
		std::vector<texCoord2D> & out_uvs,
		std::vector<normal> & out_normals
);
