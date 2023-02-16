#include "Shader.h"


Shader::Shader()
{
	shaderID = 0;
	uniformModel = 0;
	uniformProjection = 0;
    uniformView = 0;
    pointLightCount = 0;
    spotLightCount = 0;

    //useless definitions so no warnings
    uniformDirectionalLight.uniformAmbientIntensity = -1;
    uniformDirectionalLight.uniformColour = -1;
    uniformDirectionalLight.uniformDirection = -1;
    uniformDirectionalLight.uniformDiffuseIntensity = -1;
    uniformSpecularIntensity = -1;
    uniformShininess = -1;
    uniformEyePosition = -1;
    uniformPointLightCount = -1;
    uniformSpotLightCount = -1;
    uniformTexture = -1;
    uniformDirectionalLightTransform = -1;
    uniformDirectionalShadowMap = -1;
    uniformFarPlane = -1;
    uniformOmniLightPos = -1;
    uniformShadowMapPassEnable = -1;
    
    for (size_t i = 0; i < 6; i++)
    {
        uniformLightMatrices[i] = -1;
    }
    for (size_t i = 0; i < MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS; i++)
    {
        uniformOmniShadowMap[i].uniformFarPlane = -1;
        uniformOmniShadowMap[i].uniformShadowMap = -1;
    }
    
    for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        uniformPointLight[i].uniformColour = -1;
        uniformPointLight[i].uniformAmbientIntensity = -1;
        uniformPointLight[i].uniformDiffuseIntensity = -1;

        uniformPointLight[i].uniformPosition = -1;
        uniformPointLight[i].uniformConstant = -1;
        uniformPointLight[i].uniformLinear = -1;
        uniformPointLight[i].uniformExponent = -1;
    }

    for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++)
    {
        uniformSpotLight[i].uniformColour = -1;
        uniformSpotLight[i].uniformAmbientIntensity = -1;
        uniformSpotLight[i].uniformDiffuseIntensity = -1;

        uniformSpotLight[i].uniformPosition = -1;
        uniformSpotLight[i].uniformConstant = -1;
        uniformSpotLight[i].uniformLinear = -1;
        uniformSpotLight[i].uniformExponent = -1;

        uniformSpotLight[i].uniformDirection = -1;
        uniformSpotLight[i].uniformEdge = -1;
    }
}

void Shader::createFromString(const char* vertexCode, const char* fragmentCode)
{
	compileShader(vertexCode, fragmentCode);

}
void Shader::createFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
    std::string vertexString = readFile(vertexLocation);
    std::string fragmentString = readFile(fragmentLocation);
    const char* vertexCode = vertexString.c_str();
    const char* fragmentCode = fragmentString.c_str();

    compileShader(vertexCode, fragmentCode);
}

void Shader::createFromFiles(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation)
{
    std::string vertexString = readFile(vertexLocation);
    std::string geometryString = readFile(geometryLocation);
    std::string fragmentString = readFile(fragmentLocation);
    const char* vertexCode = vertexString.c_str();
    const char* geometryCode = geometryString.c_str();
    const char* fragmentCode = fragmentString.c_str();

    compileShader(vertexCode, geometryCode, fragmentCode);
}

void Shader::validate()
{
    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glValidateProgram(shaderID);
    glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        printf("Error validating program: '%s'\n", eLog);
        return;
    }
}

std::string Shader::readFile(const char* fileLocation)
{
    std::string content;
    std::ifstream fileStream(fileLocation, std::ios::in);

    if (!fileStream.is_open())
    {
        printf("Failed to read %s ! File does not exist.", fileLocation);
        return "";
    }

    std::string line = "";
    while (!fileStream.eof())
    {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}
void Shader::compileShader(const char* vertexCode, const char* fragmentCode)
{
    // reserve space on GPU
    shaderID = glCreateProgram();
    if (!shaderID)
    {
        printf("Error creating shader program!\n");
        return;
    }

    // value, where the code for the shader starts and the type  (vertex shader or fragment)
    addShader(shaderID, vertexCode, GL_VERTEX_SHADER);
    addShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

    compileProgram();
}

void Shader::compileShader(const char* vertexCode, const char* geometryCode, const char* fragmentCode)
{
    // reserve space on GPU
    shaderID = glCreateProgram();
    if (!shaderID)
    {
        printf("Error creating shader program!\n");
        return;
    }

    // value, where the code for the shader starts and the type  (vertex shader or fragment)
    addShader(shaderID, vertexCode, GL_VERTEX_SHADER);
    addShader(shaderID, geometryCode, GL_GEOMETRY_SHADER);
    addShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

    compileProgram();
}
void Shader::compileProgram()
{
    // result code and log
    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    // create the executables on the graphics card
    glLinkProgram(shaderID);
    // get unformation about program link status
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        printf("Error linking program: %s \n", eLog);
        return;
    }

    //validate();

    //takes 2, the shader and the name of the variable in the program
    uniformModel = glGetUniformLocation(shaderID, "model");
    uniformProjection = glGetUniformLocation(shaderID, "projection");
    uniformView = glGetUniformLocation(shaderID, "view");
    uniformDirectionalLight.uniformColour = glGetUniformLocation(shaderID, "directionalLight.base.colour");
    uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.base.ambientIntensity");
    uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
    uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.base.diffuseIntensity");
    uniformSpecularIntensity = glGetUniformLocation(shaderID, "material.specularIntensity");
    uniformShininess = glGetUniformLocation(shaderID, "material.shininess");
    uniformEyePosition = glGetUniformLocation(shaderID, "eyePosition");

    uniformPointLightCount = glGetUniformLocation(shaderID, "pointLightCount");
    for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        // print the index to the string
        char locBuff[100] = { '\0' };

        //get the uniforms
        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.colour", i);
        uniformPointLight[i].uniformColour = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.ambientIntensity", i);
        uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.diffuseIntensity", i);
        uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].position", i);
        uniformPointLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].constant", i);
        uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].linear", i);
        uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].exponent", i);
        uniformPointLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);
    }

    // for the spot lights
    uniformSpotLightCount = glGetUniformLocation(shaderID, "spotLightCount");

    for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++)
    {
        // must go to base twice because spot light contains a point light
        char locBuff[100] = { '\0' };

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.colour", i);
        uniformSpotLight[i].uniformColour = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.ambientIntensity", i);
        uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.diffuseIntensity", i);
        uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.position", i);
        uniformSpotLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.constant", i);
        uniformSpotLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.linear", i);
        uniformSpotLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.exponent", i);
        uniformSpotLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].direction", i);
        uniformSpotLight[i].uniformDirection = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].edge", i);
        uniformSpotLight[i].uniformEdge = glGetUniformLocation(shaderID, locBuff);
    }

    for (size_t i = 0; i < MAX_SPOT_LIGHTS + MAX_POINT_LIGHTS; i++)
    {
        char locBuff[100] = { '\0' };

        snprintf(locBuff, sizeof(locBuff), "omniShadowMaps[%d].shadowMap", i);
        uniformOmniShadowMap[i].uniformShadowMap = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "omniShadowMaps[%d].farPlane", i);
        uniformOmniShadowMap[i].uniformFarPlane = glGetUniformLocation(shaderID, locBuff);
    }

    uniformDirectionalLightTransform = glGetUniformLocation(shaderID, "directionalLightTransform");
    uniformTexture = glGetUniformLocation(shaderID, "theTexture");
    uniformDirectionalShadowMap = glGetUniformLocation(shaderID, "directionalShadowMap");

    // for the omnidirectional shadowmap
    uniformOmniLightPos = glGetUniformLocation(shaderID, "lightPos");
    uniformFarPlane = glGetUniformLocation(shaderID, "farPlane");


    // go through the array and get the array
    for (size_t i = 0; i < 6; i++)
    {
        char locBuff[100] = { '\0' };

        snprintf(locBuff, sizeof(locBuff), "lightMatrices[%d]", i);
        uniformLightMatrices[i] = glGetUniformLocation(shaderID, locBuff);
    }
    uniformShadowMapPassEnable = glGetUniformLocation(shaderID, "shadowMapPassEnable");
    uniformInstancedDraws = glGetUniformLocation(shaderID, "instancedDraws");
    uniformFogEnable = glGetUniformLocation(shaderID, "fogEnable");
    uniformTeleportStrength = glGetUniformLocation(shaderID, "teleportStrength");
}
GLuint Shader::getProjectionLocation()
{
    return uniformProjection;
}
GLuint Shader::getModelLocation()
{
    return uniformModel;
}

GLuint Shader::getViewLocation()
{
    return uniformView;
}

GLuint Shader::getAmbientIntensityLocation()
{
    return uniformDirectionalLight.uniformAmbientIntensity;
}
GLuint Shader::getAmbientColourLocation()
{
    return uniformDirectionalLight.uniformColour;
}

GLuint Shader::getDiffuseIntensityLocation()
{
    return uniformDirectionalLight.uniformDiffuseIntensity;
}
GLuint Shader::getDirectionLocation()
{
    return uniformDirectionalLight.uniformDirection;
}

GLuint Shader::getSpecularIntensityLocation()
{
    return uniformSpecularIntensity;
}
GLuint Shader::getShininessLocation()
{
    return uniformShininess;
}

GLuint Shader::getEyePositionLocation()
{
    return uniformEyePosition;
}

GLuint Shader::getOmniLightPosLocation()
{
    return uniformOmniLightPos;
}

GLuint Shader::getFarPlaneLocation()
{
    return uniformFarPlane;
}

GLuint Shader::getUniformShadowMapPassEnableLocation()
{
    return uniformShadowMapPassEnable;
}

GLuint Shader::getInstancedDrawsLocation()
{
    return this->uniformInstancedDraws;
}

GLuint Shader::getFogEnableLocation()
{
    return this->uniformFogEnable;
}

GLuint Shader::getTeleportStrengthLocation()
{
    return this->uniformTeleportStrength;
}

void Shader::setDirectionalLight(DirectionalLight* dLight)
{
    dLight->useLight(uniformDirectionalLight.uniformAmbientIntensity,
        uniformDirectionalLight.uniformColour,
        uniformDirectionalLight.uniformDiffuseIntensity, uniformDirectionalLight.uniformDirection);
}

void Shader::setPointLights(PointLight* pLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset)
{
    if (lightCount > MAX_POINT_LIGHTS) lightCount = MAX_POINT_LIGHTS;
    
    glUniform1i(uniformPointLightCount, lightCount);

    for (size_t i = 0; i < lightCount; i++)
    {
        pLight[i].useLight(uniformPointLight[i].uniformAmbientIntensity, uniformPointLight[i].uniformColour,
            uniformPointLight[i].uniformDiffuseIntensity, uniformPointLight[i].uniformPosition,
            uniformPointLight[i].uniformConstant, uniformPointLight[i].uniformLinear, uniformPointLight[i].uniformExponent);

        // add i to the ENUM in order to get to the texture unit we want
        // plus the offset from the texture
        pLight[i].getShadowMap()->read(GL_TEXTURE0 + textureUnit + i);
        // for point lights offset will be 0, but for spot lights will be pointlightcount
        glUniform1i(uniformOmniShadowMap[i + offset].uniformShadowMap, textureUnit + i);
        glUniform1f(uniformOmniShadowMap[i + offset].uniformFarPlane, pLight[i].getFarPlane());
    }
}

void Shader::setSpotLights(SpotLight* sLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset)
{
    if (lightCount > MAX_SPOT_LIGHTS) lightCount = MAX_SPOT_LIGHTS;

    glUniform1i(uniformSpotLightCount, lightCount);

    for (size_t i = 0; i < lightCount; i++)
    {
        sLight[i].useLight(uniformSpotLight[i].uniformAmbientIntensity, uniformSpotLight[i].uniformColour,
            uniformSpotLight[i].uniformDiffuseIntensity, uniformSpotLight[i].uniformPosition, uniformSpotLight[i].uniformDirection,
            uniformSpotLight[i].uniformConstant, uniformSpotLight[i].uniformLinear, uniformSpotLight[i].uniformExponent,
            uniformSpotLight[i].uniformEdge);

        sLight[i].getShadowMap()->read(GL_TEXTURE0 + textureUnit + i);
        glUniform1i(uniformOmniShadowMap[i + offset].uniformShadowMap, textureUnit + i);
        glUniform1f(uniformOmniShadowMap[i + offset].uniformFarPlane, sLight[i].getFarPlane());
    }
}


void Shader::setTexture(GLuint textureUnit)
{
    glUniform1i(uniformTexture, textureUnit);
}

void Shader::setDirectionalShadowMap(GLuint textureUnit)
{
    glUniform1i(uniformDirectionalShadowMap, textureUnit);
}

void Shader::setDirectionalLightTransform(glm::mat4* lTransform)
{
    glUniformMatrix4fv(uniformDirectionalLightTransform, 1, GL_FALSE, glm::value_ptr(*lTransform));
}

void Shader::setOmniLightMatrices(std::vector<glm::mat4> lightMatrices)
{
    for (size_t i = 0; i < 6; i++)
    {
        glUniformMatrix4fv(uniformLightMatrices[i], 1, GL_FALSE, glm::value_ptr(lightMatrices[i]));
    }
}
 void Shader::useShader()
{
    glUseProgram(shaderID);
}

void Shader::clearShader()
{
    if (shaderID != 0)
    {
        glDeleteProgram(shaderID);
        shaderID = 0;
    }
    uniformModel = 0;
    uniformProjection = 0;
}
void Shader::addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1]{};
    theCode[0] = shaderCode;

    GLint codeLength[1]{};
    codeLength[0] = strlen(shaderCode);

    //shader to modify code, number of files to pass, array to code and size
    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    // error checking
    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        printf("Error compiling the %d shader: %s \n", shaderType, eLog);
        return;
    }

    glAttachShader(theProgram, theShader);
}


Shader::~Shader()
{
    clearShader();
}