#version 330

layout (location = 0) in vec3 position;

uniform mat4 perspectiveMatrix;
uniform mat4 modelViewMatrix;

void main()
{
	gl_Position =  perspectiveMatrix * modelViewMatrix * vec4(position, 1.0);
}