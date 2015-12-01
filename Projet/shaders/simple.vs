#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 vertexColor;
layout (location = 2) in vec3 vertexNormal;
layout (location = 3) in vec3 tangeant;
layout (location = 4) in vec3 biTangeant;
layout (location = 5) in vec2 textureCoordinate;

out vec2 fragTexCoord;

void main()
{
	fragTexCoord = textureCoordinate.st;
	gl_Position =  vec4(position, 1.0);
}