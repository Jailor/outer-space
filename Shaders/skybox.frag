#version 330

in vec3 texCoords;

out vec4 colour;

uniform samplerCube skybox;

uniform float teleportStrength;

void main()
{
	if(teleportStrength > 0.01){
		colour = mix(texture(skybox, texCoords), vec4(6,10,10,1.0), teleportStrength);
	}
	else colour = texture(skybox, texCoords);
}