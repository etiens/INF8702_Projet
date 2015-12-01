#version 330

in vec2 fragTexCoord;

out vec4 outputColor;

uniform sampler2D textureSampler1;
uniform sampler2D textureSampler2;
uniform sampler2D textureSampler3;
uniform bool showScene;
uniform bool showBloom;
uniform bool showGodRays;

void main()
{	
	if(showScene)
		outputColor += vec4(texture(textureSampler1, fragTexCoord).rgb, 1.0f);
	if(showBloom)
		outputColor += vec4(texture(textureSampler2, fragTexCoord).rgb, 1.0f) * 0.6;
	if(showGodRays)
		outputColor += vec4(texture(textureSampler3, fragTexCoord).rgb, 1.0f);
}