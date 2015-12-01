#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 vertexColor;
layout (location = 2) in vec3 vertexNormal;
layout (location = 3) in vec3 tangeant;
layout (location = 4) in vec3 biTangeant;
layout (location = 5) in vec2 textureCoordinate;

out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragEyeCoordPosition;
out vec3 fragBaseColor;

uniform mat4 perspectiveMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

void main()
{
	fragTexCoord = textureCoordinate.st;
	fragNormal = normalize(vec3(normalMatrix * vec4(vertexNormal, 0.0)));
	fragEyeCoordPosition = normalize((modelViewMatrix * vec4(position, 1.0)).xyz);
	fragBaseColor = vertexColor.rgb;

	gl_Position =  perspectiveMatrix * modelViewMatrix * vec4(position, 1.0);
}