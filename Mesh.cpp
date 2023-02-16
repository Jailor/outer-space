#include "Mesh.h"

Mesh::Mesh()
{
	VAO = 0;
	VBO = 0;
	IBO = 0; 
	indexCount = 0;
}

void Mesh::createMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
	indexCount = numOfIndices;

    // define the space for 1 vertex array, then bind the vertex array
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // define the indexed buffer accordingly
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0])* numOfIndices, indices, GL_STATIC_DRAW);


    //define the space and allocate space for vbo, then bind it
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //bind our triangle to the buffer, static means that we won't modify the vertice corrdinates (gl gynamic does that) 
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0])* numOfVertices, vertices, GL_STATIC_DRAW);

    // location of attribute, 3 xyz values to pass, data type, if we want it normalised or not, stride in order to keep more
    // data int the same array like colour isntead of using multiple VBOs, start index
    // we stride with 8 so we know where the next value starts
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0])*8, 0);
    //enable the drawing array from the lcoation/index used above
    glEnableVertexAttribArray(0);

    // layout with attribute 1 will be our textures coordinates
    // onyl 2 fields needed, stride will be 8 as well, start at 3 for texture offset
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 3));
    glEnableVertexAttribArray(1);

    // for the normals, start at offset of 5
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 5));
    glEnableVertexAttribArray(2);

    //undoing the bindings from above
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::renderMesh()
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    //gl lines - do not color middle, gl triangles - fill in middle, start in array and how many points to draw
    //gl draw elements replaces gl draw lines
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    //revert to default
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::clearMesh()
{
    if (IBO != 0)
    {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }

    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    indexCount = 0;
}
GLuint Mesh::getVAO()
{
    return this->VAO;
}
GLsizei Mesh::getIndexCount()
{
    return this->indexCount;
}
Mesh::~Mesh()
{
    clearMesh();
}