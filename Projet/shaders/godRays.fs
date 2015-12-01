#version 330

in vec2 fragTexCoord;

out vec4 outputColor;

uniform sampler2D textureSampler;
uniform vec2 lightPosition;

void main(){

    int nbSamples = 128;
    float intensity = 0.02;
    float decay = 0.96875;
    
    //vec2 direction = lightPosition - fragTexCoord;
    vec2 direction = vec2(0.5) - fragTexCoord;
    vec2 textCoord = fragTexCoord;
    
    direction /= nbSamples;
    
    vec3 color = texture(textureSampler, textCoord).rgb;
    
    for(int samples = 0; samples < nbSamples; samples++)
    {
        color += texture(textureSampler, textCoord).rgb * intensity;
        intensity *= decay;
        textCoord += direction;
    }
    
    outputColor =  vec4(color, 1.0);
}