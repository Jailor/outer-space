// Vertex shader - modify/scale the coordinates
// version of GLSL, location is a 1-to-1 with the index from the triangle function, in means input value
// gl position is something not defined explicitly but implicitly, represent the final position of object,
// is an output value to give to the fragment shader. Can scale for example by 0.4 in x and y.
// 
// We clamp the vertex colour to (0-1) because of the negative values


#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;  //normal value
layout (location = 3) in mat4 instanceMatrix;

out vec4 vCol;
out vec2 texCoord; //texture coordinates to use
out vec3 normal;   //flat will simplify the normal calculation
//change to vec3
out vec4 fragPos;  //position of fragment in the world
out vec4 directionalLightSpacePos; // where the position of the fragment is with reagrd to the light
out vec4 ioEyeSpacePosition;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 directionalLightTransform;

uniform int instancedDraws;

void main()
{
	if(instancedDraws != 1) gl_Position = projection * view * model * vec4(pos, 1.0);
	else gl_Position = projection * view * model *instanceMatrix* vec4(pos, 1.0);

	directionalLightSpacePos = directionalLightTransform * model * vec4(pos, 1.0);

	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
	texCoord = tex;

	// because the fragment works in a different coordinate system and we don't need position
	// transpose inverse because if we scale in only 1 direction it all gets bad
	normal = mat3(transpose(inverse(model))) * norm;

	// multiply with model so we know where it is in the world
	fragPos = model* vec4(pos, 1.0f); 
	ioEyeSpacePosition = view * model * vec4(pos, 1.0);
}