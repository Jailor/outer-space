// if already been defined, don't include it again
#ifndef COMMONVALS
#define COMMONVALS

// WILL RUN SLOW ON INTEGRATED GRAPHICS IF SET TO TRUE
const bool ENABLE_XWING = true;

enum class SkyboxNames { NIGHT_SCENE, SPACE_SCENE};
enum class TeleportLocations { MAIN_SCENE, MOON_SCENE, STAR_TREK_SCENE, SATURN_SCENE};

const float FLASHLIGHT_OFFSET = 2.00f; // 3.0f and disable the other lights to show omnidirectional shadow maps

const int  MAX_POINT_LIGHTS = 5;
const int MAX_SPOT_LIGHTS = 3;
const float toRadians = 3.14159265f / 180.0f;



// shader locations
static const char* mainVertexShader = "Shaders/shader.vert";
static const char* mainFragmentShader = "Shaders/shader.frag";
static const char* directionalVertexShader = "Shaders/directional_shadow_map.vert";
static const char* directionalFragmentShader = "Shaders/directional_shadow_map.frag";
static const char* omnidVertexShader = "Shaders/omni_directional_shadow_map.vert";
static const char* omniGeometryShader = "Shaders/omni_directional_shadow_map.geom";
static const char* omniFragmentShader = "Shaders/omni_directional_shadow_map.frag";
static const char* skyboxVertexShader = "Shaders/skybox.vert";
static const char* skyboxFragmentShader = "Shaders/skybox.frag";
static const char* instanceVertexShader = "Shaders/instance.vert";
static const char* instanceFragmentShader = "Shaders/instance.frag";
// include stb
#pragma warning(push, 0)
#include "stb_image.h"
#pragma warning(pop)
#endif
