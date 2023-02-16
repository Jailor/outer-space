#include "UtilityFunctions.h"


void calcAverageNormals(unsigned int* indices, unsigned int indiceCount,
    GLfloat* vertices, unsigned int verticeCount,
    unsigned int vLength, unsigned int normalOffset)
{
    //caluclate value for normals using interpolation
    for (size_t i = 0; i < indiceCount; i += 3)
    {
        unsigned int in0 = indices[i] * vLength; //jump to the value in vertices 
        unsigned int in1 = indices[i + 1] * vLength;
        unsigned int in2 = indices[i + 2] * vLength;
        // normal vector using cross product
        glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1],
            vertices[in1 + 2] - vertices[in0 + 2]);
        glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1],
            vertices[in2 + 2] - vertices[in0 + 2]);
        glm::vec3 normal = glm::cross(v1, v2);
        normal = glm::normalize(normal);

        // save on typing
        in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
        vertices[in0] += normal.x;
        vertices[in0 + 1] += normal.y;
        vertices[in0 + 2] += normal.z;

        vertices[in1] += normal.x;
        vertices[in1 + 1] += normal.y;
        vertices[in1 + 2] += normal.z;

        vertices[in2] += normal.x;
        vertices[in2 + 1] += normal.y;
        vertices[in2 + 2] += normal.z;
    }

    for (size_t i = 0; i < verticeCount / vLength; i++)
    {
        //normalize the values
        unsigned int normOffset = i * vLength + normalOffset;
        glm::vec3 vec(vertices[normOffset], vertices[normOffset + 1], vertices[normOffset + 2]);
        vec = glm::normalize(vec);
        vertices[normOffset] = vec.x;
        vertices[normOffset + 1] = vec.y;
        vertices[normOffset + 2] = vec.z;
    }
}

void createObjects(std::vector<Mesh*> &meshList)
{
    //all the combinations
    unsigned int indices[] = {
        0,3,1,
        1,3,2,
        2,3,0,
        0,1,2
    };

    //points that make up the triangle
    // in OpenGL, 0,0 is at the centre of the screen. Vertex array defines point position as xyz coordinates
    // z coordinate is zero for some because we want it to be flat. -1, -1, 0 is bottom left corner.
    // u and v are the coordinates for the textures, from 0 to 1 starting from bottom left corner
    
    GLfloat vertices[] = {
        //   x       y      z    u    v         nx   ny    nz
            -1.0f, -1.0f, -0.6f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,        //base 1
            0.0f, -1.0f, 1.0f,  0.5f, 0.0f,   0.0f, 0.0f, 0.0f,  //background
            1.0f, -1.0f, -0.6f,  1.0f, 0.0f,   0.0f, 0.0f, 0.0f,  //base 2
            0.0f, 1.0f, 0.0f,  0.5f, 1.0f,    0.0f, 0.0f, 0.0f    //top
    };

    const int indicesLength = sizeof(indices) / sizeof(indices[0]);
    const int verticesLength = sizeof(vertices) / sizeof(vertices[0]);

    // Add a new object to our scene, a floor.
    unsigned int floorIndices[] =
    {
        0,2,1,
        1,2,3
    };

    // MOVE THIS
    GLfloat floorSize;
    floorSize = 15.0f;

    // 4 vertices for a square
    // Texture samples are the 4 corners of our image. The normals are straight up from
    // the surface for each of them, thus using flat shading

    GLfloat floorVertices[] =
    {
        -floorSize, 0.0f, -floorSize,    0.0f, 0.0f,   0.0f, -1.0f, 0.0f,  //top left
        floorSize, 0.0f, -floorSize,    10.0f, 0.0f,  0.0f, -1.0f, 0.0f,  //top right
        -floorSize, 0.0f, floorSize,    0.0f, 10.0f,  0.0f, -1.0f, 0.0f,  //bottom left
        floorSize, 0.0f, floorSize,    10.0f, 10.0f,  0.0f, -1.0f, 0.0f   //bottom right
    };

    const int floorIndicesLength = sizeof(floorIndices) / sizeof(floorIndices[0]);
    const int floorVerticesLength = sizeof(floorVertices) / sizeof(floorVertices[0]);


    // length of a row and how far along you are in the row
    calcAverageNormals(indices, indicesLength, vertices,
        verticesLength, 8, 5);


    Mesh* obj1 = new Mesh();                    // was hardcoded
    obj1->createMesh(vertices, indices, verticesLength, indicesLength);
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->createMesh(vertices, indices, verticesLength, indicesLength);
    meshList.push_back(obj2);

    Mesh* obj3 = new Mesh();
    obj3->createMesh(floorVertices, floorIndices, floorVerticesLength, floorIndicesLength);
    meshList.push_back(obj3);
}

void createShaders(std::vector<Shader>& shaderList, Shader& directionalShadowShader, Shader& omniShadowShader)
{
    Shader* shader1 = new Shader();
    shader1->createFromFiles(mainVertexShader, mainFragmentShader);
    shaderList.push_back(*shader1);

    directionalShadowShader = Shader();
    directionalShadowShader.createFromFiles(directionalVertexShader,
        directionalFragmentShader);
    omniShadowShader = Shader();
    omniShadowShader.createFromFiles(omnidVertexShader, omniGeometryShader, omniFragmentShader);
}

glm::mat4* generateRing(glm::vec3 translateVec, unsigned int amount)
{
    glm::mat4* modelMatrices = new glm::mat4[amount];
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
    float radius = 30.0f;
    float offset = 5.0f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, translateVec);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.1 and 0.5f
        float scale = static_cast<float>((rand() % 50) / 100.0 + 0.1f);
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = static_cast<float>((rand() % 360));
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }
    return modelMatrices;
}

void setupInstancedDraws(Model rock, unsigned int amount, glm::mat4* modelMatricesInstanced) {
    // configure instanced array
    // -------------------------
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatricesInstanced[0], GL_STATIC_DRAW);

    // set transformation matrices as an instance vertex attribute (with divisor 1)
    // note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
    // normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
    // -----------------------------------------------------------------------------------------------------------------------------------
    for (unsigned int i = 0; i < rock.getMeshes().size(); i++)
    {
        std::vector<Mesh*> meshes = rock.getMeshes();
        unsigned int VAO = meshes[i]->getVAO();
        glBindVertexArray(VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }
}

void setupSkybox(SkyboxNames scene_enum, Skybox& skybox) {
    std::vector<std::string> skyboxFaces;
    std::string scene_name = "Textures/Skybox/";
    switch (scene_enum)
    {
    case SkyboxNames::NIGHT_SCENE:
        scene_name.append("purplenebula");
        break;
    case SkyboxNames::SPACE_SCENE:
        scene_name.append("stratosphere");
        break;
    default:
        throw std::invalid_argument("Current enum does not cover this skybox texture case in setupSkybox(). Exiting!");
        break;
    }

    //setup skybox
   // order is important: 
   // pos-x, neg-x (right, left)
   // pos-y, neg-y (up , down)
   // pos-z, neg-z (back, front)
    skyboxFaces.push_back(scene_name + "_rt.tga");
    skyboxFaces.push_back(scene_name + "_lf.tga");
    skyboxFaces.push_back(scene_name + "_up.tga");
    skyboxFaces.push_back(scene_name + "_dn.tga");
    skyboxFaces.push_back(scene_name + "_bk.tga");
    skyboxFaces.push_back(scene_name + "_ft.tga");
    skybox = Skybox(skyboxFaces);

}
