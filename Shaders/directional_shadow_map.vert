#version 330

// rendering everything with respect to a light source, so we don't need view etc.
layout (location = 0) in vec3 pos;

// convert into world space
uniform mat4 model;
// POV of the directional light, parallel projection 
uniform mat4 directionalLightTransform;

void main()
{
	gl_Position = directionalLightTransform * model * vec4(pos, 1.0);
}
