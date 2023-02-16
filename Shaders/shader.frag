// Fragment shader - handles the fragments (information about each pixel)
// usually only has one output value, will assume that color is the output value

#version 330

in vec4 vCol; 
in vec2 texCoord;
in vec3 normal;  //adding "flat in vec3 normal" has the effect of picking just 1 vertex and nor interpolating
in vec4 fragPos;
in vec4 directionalLightSpacePos;

out vec4 colour;

// should match the one in the project
const int MAX_POINT_LIGHTS = 5;
const int MAX_SPOT_LIGHTS = 3;

struct Light
{
	vec3 colour;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight 
{
	Light base; // common light stuff
	vec3 direction;
};

struct PointLight 
{
	Light base; // common light stuff
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct SpotLight
{
	PointLight base;
	vec3 direction;
	float edge;
};

// struct for the omni shadow map, has a samplerCube
// instead of a sampler2D
struct OmniShadowMap
{
	samplerCube shadowMap;
	float farPlane;
};

struct Material
{
	float specularIntensity;
	float shininess;
};
struct FogParameters
{
	vec3 color;
	float linearStart;
	float linearEnd;
	float density;
	
	int equation;
	bool isEnabled;
};
FogParameters fogParams;
in vec4 ioEyeSpacePosition;


uniform int shadowMapPassEnable;
uniform int instancedDraws;
uniform int fogEnable;
uniform float teleportStrength;

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS]; //declare the max nr of point lights
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

// shadow maps for both the point lights and the spot lights
uniform OmniShadowMap omniShadowMaps[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

uniform sampler2D theTexture;  //sampler connected to our texture
uniform sampler2D directionalShadowMap; // sampler connected to the shadow map

uniform Material material;

uniform vec3 eyePosition; //camera positon


// different samples for the sampler
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

// works for the spotlights as well
float calcPointShadowFactor(PointLight light, int shadowIndex)
{
	// get the frag to light vector
	vec3 fragToLight = fragPos.xyz - light.position;
	float currentDepth = length(fragToLight);
	
	float shadow = 0.0;
	float bias   = 2.5; //was 0.15
	int samples  = 20;
	// modify disk radius based on the distance so we have less pixelation when we get close
	float viewDistance = length(eyePosition - fragPos.xyz);
	float diskRadius = (1.0 + (viewDistance / omniShadowMaps[shadowIndex].farPlane)) / 25.0;
	for(int i = 0; i < samples; ++i)
	{
		// get the closest point from the sampling disk
		float closestDepth = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
		// multiply by far plane to undo division  done by geometric shader
		closestDepth *= omniShadowMaps[shadowIndex].farPlane;
		// if the current depth is bigger than closest, we are in shadow
		if(currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	// divide by the number of samples
	shadow /= float(samples);  
	
	return shadow;
}

float calcDirectionalShadowFactor(DirectionalLight light)
{
		
	// get the closest position to out light. We don't need z from the projectionCoords
	// because it is a parallel projection with this directional light
	///float closest = texture(directionalSahdowMap, projectionCoords.xy).r;
	///float current = projectionCoords.z;
	// right in front of you is 0, 1 is farther away. A hard cut-off in this way.
	///float shadow = current > closest ? 1.0 : 1.0;


	// convert to normalized device coordinates (-1 to 1)
	vec3 projCoords = directionalLightSpacePos.xyz / directionalLightSpacePos.w;
	// now we need to convert it to a (0 - 1) range because that's how the shadow map works
	projCoords = (projCoords * 0.5) + 0.5;
	
	float current = projCoords.z;
	
	// add a bias to help mitigate shadow acne
	vec3 normal = normalize(normal);
	vec3 lightDir = normalize(light.direction);
	
	// range between 0.05 and 0.005, using the angle/dot product between the normal and light direction
	float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.00005);

	// initialize the shadow for averaging out the 9 pixels around it
	float shadow = 0.0;
	// find how big our texture size
	vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);
	// iterate thru the x and y values of the 3x3 block
	// the bigger the for loop, the larger the performance hit, it squares
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			// depth of current pixel
			float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
			// use current - bias for comparing (shadow acne)
			// add it to the shadow and then divide by the number of pixels
			shadow += current - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= 9.0;
	
	// if the point being lit is beyond the far plane, we will not
	// see a shadow. If the far plane does not extend that far, we don't care about the shadow
	// even if that shadow may exist
	if(projCoords.z > 1.0)
	{
		shadow = 0.0;
	}									
	
	return shadow;
}

// calculate common lighting values like ambient, diffuse, specular
vec4 calcLightByDirection(Light light, vec3 direction, float shadowFactor)
{
	vec4 ambientColour = vec4 (light.colour, 1.0f)* light.ambientIntensity;
	
	// dot product that gives something similar to cos, bigger means more light hitting the object
	float diffuseFactor = max (dot(normalize(normal), normalize(direction)), 0.0f);
	//first two handle the intensity of the light in general while the last one makes sure it is different depending on the angle to the light direction
	vec4 diffuseColour = vec4(light.colour, 1.0f) * light.diffuseIntensity*diffuseFactor;

	vec4 specularColour = vec4(0,0,0,0);
	if(diffuseFactor > 0.0f)
	{
		// vector from fragment location to eye
		vec3 fragToEye = normalize(eyePosition - fragPos.xyz);
		// reflected light vector mirrored from the normal
		vec3 reflectedVertex = normalize( reflect(direction, normalize(normal)));
		float specularFactor = dot (fragToEye, reflectedVertex);
		// if towards our viewer, apply specular light
		if(specularFactor > 0.0f)
		{
			specularFactor = pow (specularFactor, material.shininess);
			specularColour = vec4(light.colour * material.specularIntensity * specularFactor, 1.0f);
		}
	}
	// add the lights toghether to create the effect
	// max shadow factor will result in no light
	return (ambientColour +  (1.0 - shadowFactor) * (diffuseColour + specularColour));
}

// uses the same mechanism as calcLightByDirection but passes in our (only) directional light
vec4 calcDirectionalLight()
{
	float shadowFactor = calcDirectionalShadowFactor(directionalLight); 
	return vec4(calcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor).xyz, 1.0f);
}

vec4 calcPointLight(PointLight pLight, int shadowIndex)
{
	// get the vector from our point light to the fragment
	vec3 direction = fragPos.xyz - pLight.position;
	float my_distance = length(direction);

	direction = normalize(direction);
	
	vec4 colour;
	if(shadowMapPassEnable ==  1)
	{
		// get the shadow factor
		float shadowFactor = calcPointShadowFactor(pLight, shadowIndex);
		// calculate it as if it was just a directional light
		colour = calcLightByDirection(pLight.base, direction, shadowFactor);
	}
	else colour = calcLightByDirection(pLight.base, direction, 0.0);
	
	// now modify it so that distance is taken into account
	float attenuation = pLight.exponent * my_distance * my_distance +
							pLight.linear * my_distance +
							pLight.constant;
	// add it to the total colour
	return colour/ attenuation;
}

vec4 calcSpotLight(SpotLight sLight, int shadowIndex)
{
	// get the vector from spot light to fragment normalized
	vec3 rayDirection = normalize(fragPos.xyz - sLight.base.position);
	// use the dot prodoct to get the angle to know if
	// it lies within the cone of our spotlight direction
	float slFactor = dot(rayDirection, sLight.direction);

	// if we are inside (compare cos values)
	if(slFactor > sLight.edge)
	{
		// calculate the colour based on its point light attributes
		vec4 colour = calcPointLight(sLight.base,shadowIndex);
		// rescale to allow for spot light edges to fade
		return colour * (1.0f - (1.0f - slFactor)*(1.0f/(1.0f - sLight.edge)));
	}else
	{
		return vec4(0,0,0,0);
	}
}



vec4 calcPointLights()
{
	//initialize the total colour, for every point light we modify it
	vec4 totalColour = vec4(0,0,0,0);
	for(int i = 0; i < pointLightCount; i++)
	{
		// add it to the total colour and pass i as the index as point lights are first
		totalColour += calcPointLight(pointLights[i],i);
	}
	return totalColour;
}

vec4 calcSpotLights()
{
	//initialize the total colour, for every spot light we modify it
	vec4 totalColour = vec4(0,0,0,0);
	for(int i = 1; i < spotLightCount; i++)
	{
		// add it to the total colour and pass i + pointLightCount
		totalColour += calcSpotLight(spotLights[i],i + pointLightCount);
	}
	return totalColour;
} 

// flashlight on 0
vec4 calcFlashlight()
{
	return calcSpotLight(spotLights[0],0 + pointLightCount);
} 


float getFogFactor(FogParameters params, float fogCoordinate)
{
	float result = 0.0;
	if(params.equation == 0)
	{
		float fogLength = params.linearEnd - params.linearStart;
		result = (params.linearEnd - fogCoordinate) / fogLength;
	}
	else if(params.equation == 1) {
		result = exp(-params.density * fogCoordinate);
	}
	else if(params.equation == 2) {
		result = exp(-pow(params.density * fogCoordinate, 2.0));
	}
	result = 1.0 - clamp(result, 0.0, 1.0);
	return result;
}

vec4 calcColor(){
	vec4 finalColour;
	
	fogParams.equation = 0;
	fogParams.linearStart = 25.0f; // were 10.0f and 100.0f
	fogParams.linearEnd = 150.0f;
	fogParams.color = vec3(0,0,0);
	fogParams.density = 0.015f;

	if(instancedDraws != 1){
		finalColour = calcDirectionalLight();
		finalColour += calcPointLights();
		finalColour += calcSpotLights();
	}
	else 
	{
		finalColour = vec4 (directionalLight.base.colour, 1.0f)* directionalLight.base.ambientIntensity *2;
	}

	if(fogEnable == 1)
    {
      float fogCoordinate = abs(ioEyeSpacePosition.z / ioEyeSpacePosition.w);
      finalColour = mix(finalColour, vec4(fogParams.color, 1.0), getFogFactor(fogParams, fogCoordinate));
    }
	// flashlight not affected by fog
	finalColour += calcFlashlight();

	if(teleportStrength > 0.01){
		finalColour = mix(finalColour, vec4(6,10,10,1.0), teleportStrength);
	}

	return finalColour;
}

void main()
{
	// our final colour is the sum from all the lights
	vec4 finalColour = calcColor();
	//colour = vec4((texture(theTexture, texCoord) * finalColour).xyz, 1.0f); //colour = vCol; /colour = texture(theTexture, texCoord) * vCol
	colour = (texture(theTexture, texCoord) * finalColour);
}