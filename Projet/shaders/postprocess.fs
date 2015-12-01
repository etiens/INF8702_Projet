#version 330

in vec2 fragTexCoord;

out vec4 outputColor;

uniform sampler2D textureSampler;

void main()
{	
	outputColor = vec4(texture(textureSampler, fragTexCoord).rgb, 1.0f);
}