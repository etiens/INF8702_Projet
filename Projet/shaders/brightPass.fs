#version 330

in vec2 fragTexCoord;

out vec4 outputColor;

uniform sampler2D textureSampler;

const float luminance = 0.4;
const float middleGrey = 0.5;
const float threshold = 0.4;
const float minimumLevel = 0.7;

void main(){

	 vec3 color = texture(textureSampler, fragTexCoord).rgb;
	 
	 color -= minimumLevel;
	 
	 if(color.x > 0.0 || color.y > 0.0 || color.z > 0.0 ){
	 	color += minimumLevel;
	 	color *= (middleGrey/luminance);
 	 }else{
 	 	color = vec3(0.0);
 	 }
	 
	 outputColor = vec4(color, 1.0);
}