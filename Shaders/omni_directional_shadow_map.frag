#version 330
in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
	// get the distance between the fragment and the light position
	float my_distance = length(fragPos.xyz - lightPos);
	my_distance = my_distance/farPlane;
	gl_FragDepth = my_distance;
}