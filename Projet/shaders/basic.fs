#version 330

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragEyeCoordPosition;
in vec3 fragBaseColor;

out vec4 outputColor;

const int MAX_LIGHTS = 10;

struct LightProperties
{
	vec4 ambiant;
	vec4 color;  
	vec4 position; //If isDirection, this is the direction of the light
	vec4 halfVector; //for directional light highlight
	vec4 coneDirection;

	bool isEnabled;
	bool isSpot;
	bool isDirectional;
	float spotCosCutoff;

	float spotExponent;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};

layout(std140) uniform LightUniformBlock
{
	LightProperties Lights[MAX_LIGHTS];
};

uniform vec3 EyeDirection;

//Material properties
uniform float Shininess;
uniform float Strenght;

uniform float baseColorFactor; //1 : take baseColor for base color. 0, take texture color for base color.
uniform sampler2D textureSampler;

void main()
{
	vec3 ambiantLight = vec3(0.0); //Ambiant color
	vec3 scatteredLight = vec3(0.0); //Diffuse color
	vec3 reflectedLight = vec3(0.0); //Specular color

	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		if(!Lights[i].isEnabled)
		{
			continue;	
		}

		vec3 halfVector;
		vec3 lightDirection = Lights[i].position.xyz;
		float attenuation = 1.0;

		if(!Lights[i].isDirectional)
		{
			lightDirection = lightDirection - fragEyeCoordPosition;
			float lightDistance = length(lightDirection);
			lightDirection = lightDirection/lightDistance;
			
			attenuation =
				1.0/(Lights[i].constantAttenuation +
					Lights[i].linearAttenuation * lightDistance +
					Lights[i].quadraticAttenuation * lightDistance * lightDistance
				);
			
			if(Lights[i].isSpot) 
			{
				float spotCos = dot(lightDirection, -Lights[i].coneDirection.xyz);
				if(spotCos < Lights[i].spotCosCutoff)
				{
					attenuation = 0.0f;
				}
				else
				{
					attenuation *= pow(spotCos, Lights[i].spotExponent);
				}
			}

			halfVector =  normalize(lightDirection + EyeDirection);
		}
		else // (Lights[i].isDirectional == true)
		{
			halfVector = Lights[i].halfVector.xyz;
		}		
		
		float diffuse = max(0.0, dot(fragNormal, lightDirection));
		float specular = max(0.0, dot(fragNormal, halfVector));

		if(diffuse == 0.0)
		{
			specular = 0.0;
		}
		else
		{
			specular = pow(specular, Shininess) * Strenght;
		}

		//No attenuation on ambiant light
		scatteredLight += Lights[i].ambiant.rgb + Lights[i].color.rgb * diffuse * attenuation;
		reflectedLight += Lights[i].color.rgb * specular * attenuation;
	}

	vec4 textureColor = texture(textureSampler, fragTexCoord);
	vec4 baseColorAdjusted = (baseColorFactor*vec4(fragBaseColor, 1.0)) + ((1.0 - baseColorFactor)*textureColor);
	vec3 illuminationContrib = min(baseColorAdjusted.rgb * scatteredLight + reflectedLight, vec3(1.0));
	outputColor = vec4(illuminationContrib, baseColorAdjusted.a);
}