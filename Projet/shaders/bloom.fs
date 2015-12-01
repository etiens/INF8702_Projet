#version 330

in vec2 fragTexCoord;

out vec4 outputColor;

uniform sampler2D textureSampler;




void main(){
	
	vec3 color;
 	vec3 gaussianBlur[3] = vec3[]( 
       vec3(0.061,0.242, 0.061),
       vec3(0.242,0.383,0.242),
       vec3(0.061,0.242,0.061)
       );


	 vec2 offset[9] = vec2[](
	 	   vec2(-0.006,-0.006),
	       vec2(0.0, -0.006),
	       vec2(+0.006,-0.006),
	       
	       vec2(-0.006,0.0),
	       vec2(0.0, 0.0),
	       vec2(+0.006,0.0),
	
	       vec2(-0.006,+0.006),
	       vec2(0.0, -0.006),
	       vec2(+0.006,+0.006)
	       );
	 float norm = 0.0;
	 for(int i = 0; i < 3; ++i)
	 {
		  norm += gaussianBlur[i].x;
		  norm += gaussianBlur[i].y;
		  norm += gaussianBlur[i].z;
	 }
	 for(int i = 0; i < 3; ++i)
	 {
		  gaussianBlur[i].x /= norm;
		  gaussianBlur[i].y /= norm;
		  gaussianBlur[i].z /= norm;
	 }
	 color += texture(textureSampler, fragTexCoord + offset[0]).rgb * gaussianBlur[0].x;
	 color += texture(textureSampler, fragTexCoord + offset[1]).rgb * gaussianBlur[0].y;
	 color += texture(textureSampler, fragTexCoord + offset[2]).rgb * gaussianBlur[0].z;
	 color += texture(textureSampler, fragTexCoord + offset[3]).rgb * gaussianBlur[1].x;
	 color += texture(textureSampler, fragTexCoord + offset[4]).rgb * gaussianBlur[1].y;
	 color += texture(textureSampler, fragTexCoord + offset[5]).rgb * gaussianBlur[1].z;
	 color += texture(textureSampler, fragTexCoord + offset[6]).rgb * gaussianBlur[2].x;
	 color += texture(textureSampler, fragTexCoord + offset[7]).rgb * gaussianBlur[2].y;
	 color += texture(textureSampler, fragTexCoord + offset[8]).rgb * gaussianBlur[2].z;
	 
	
	 
	 outputColor = vec4(color, 1.0); 
}