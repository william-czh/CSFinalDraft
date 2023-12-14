#include "Surface.hpp"
#include "Shader.hpp"
#include <glm/gtc/noise.hpp> // For glm::perlin
#include <glm/gtc/constants.hpp> // For glm::pi
#include <glm/gtc/matrix_transform.hpp> // For glm::lookAt, glm::ortho
#include <glm/gtc/noise.hpp> // For glm::perlin
#include <atlas/utils/GUI.hpp>
#include <iostream>

// Define vertex colors for the surface (Grass Green).
// Each row represents a vertex's color in RGB format.
GLfloat Surface::vertexColors[][3] = {
    {0.0f, 0.79f, 0.0f}, // Vertex 0: Green
    {0.0f, 0.62f, 0.0f}, // Vertex 1: Green
    {0.0f, 0.85f, 0.0f}, // Vertex 2: Green
    {0.0f, 0.49f, 0.0f}  // Vertex 3: Green
};

// Define texture coordinates for the surface.
// Each row represents a vertex's texture coordinates (u, v).
GLfloat Surface::texCoords[][2] = {
    {0.0f, 0.0f}, // Vertex 0: (u=0, v=0)
    {1.0f, 0.0f}, // Vertex 1: (u=1, v=0)
    {0.0f, 1.0f}, // Vertex 2: (u=0, v=1)
    {1.0f, 1.0f}  // Vertex 3: (u=1, v=1)
};

// Define vertex positions for the surface.
// Each row represents a vertex's position in 3D space (x, y, z).
float edgeSize = 10.0f;
GLfloat Surface::vertexPos[][3] = {
    {-edgeSize, 0.0f, -edgeSize}, // Vertex 0
    {edgeSize, 0.0f, -edgeSize},  // Vertex 1
    {-edgeSize, 0.0f, edgeSize},  // Vertex 2
    {edgeSize, 0.0f, edgeSize}    // Vertex 3
};

// Define vertex normals for the surface.
// Each row represents a vertex's normal vector in 3D space (x, y, z).
GLfloat Surface::vertexNorm[][3] = {
    {0.0f, 1.0f, 0.0f}, // Vertex 0: Normal (0, 1, 0)
    {0.0f, 1.0f, 0.0f}, // Vertex 1: Normal (0, 1, 0)
    {0.0f, 1.0f, 0.0f}, // Vertex 2: Normal (0, 1, 0)
    {0.0f, 1.0f, 0.0f}  // Vertex 3: Normal (0, 1, 0)
};

Surface::Surface()
{
    // Generate OpenGL buffers and bind vertex array
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_PosBuff);
    glGenBuffers(1, &m_NormBuff);
    glGenBuffers(1, &m_ColBuff);
    glGenBuffers(1, &m_TexCoordBuff);

    glBindVertexArray(m_VAO);

    // Calculate Perlin noise for vertex positions
    // for (int i = 0; i < 4; ++i)
    // {
    //     float x = vertexPos[i][0];
    //     float z = vertexPos[i][2];
    //     float noise = glm::perlin(glm::vec2(x, z) * 0.1f); // Adjust the scale as needed
    //     vertexPos[i][1] = noise * 0.2f; // Adjust the amplitude as needed
    // }

    // Buffer and set vertex attribute pointers for position, normal, color
    bufferAndSetAttribute(m_PosBuff, 0, 3, Surface::vertexPos, 12);
    bufferAndSetAttribute(m_NormBuff, 1, 3, Surface::vertexNorm, 12);
    bufferAndSetAttribute(m_ColBuff, 2, 3, Surface::vertexColors, 12);

    // Buffer and set texture coordinates
    bufferAndSetTextureCoords(m_TexCoordBuff, 3, 2, Surface::texCoords, 8);

    glBindVertexArray(0);

    //-------------------------------------------------------------------------

    // Generate snowballs. 
    glGenVertexArrays(1, &m_Snowball_VAO);
    glGenBuffers(1, &m_Snowball_PosBuff);
    glGenBuffers(1, &m_Snowball_NormBuff);
    glGenBuffers(1, &m_Snowball_ColBuff);
    glGenBuffers(1, &m_Snowball_TexCoordBuff);

    std::vector<glm::vec3> snowball_vertexColors_vec;
    std::vector<glm::vec2> snowball_texCoords_vec;
    std::vector<glm::vec3> snowball_vertexPos_vec;
    std::vector<glm::vec3> snowball_vertexNorm_vec;

    std::vector<glm::vec3> positions;
    float domeRadius = 4.0f;
    float domeHeight = 3.0f;
    int numLayers = 15; // Number of layers

    // Generate dome-like top with layered circles
    for (int layer = 0; layer < numLayers; ++layer) {
        float layerHeight = domeHeight * (static_cast<float>(layer) / numLayers);
        float layerRadius = domeRadius * glm::sqrt(1.0f - glm::pow(layerHeight / domeHeight, 2.0f));

        for (float angle = 0.0f; angle < glm::two_pi<float>(); angle += 0.1f) {
            float x = layerRadius * glm::cos(angle);
            float z = layerRadius * glm::sin(angle);
            float y = layerHeight;

            positions.push_back(glm::vec3(x, y, z));
        }
    }

    for (glm::vec3 position : positions) {
        Cube cubeData = populateCubeData(position, 0.5f);
        snowball_vertexColors_vec.insert(snowball_vertexColors_vec.end(), cubeData.vertexColors.begin(), cubeData.vertexColors.end());
        snowball_texCoords_vec.insert(snowball_texCoords_vec.end(), cubeData.texCoords.begin(), cubeData.texCoords.end());
        snowball_vertexPos_vec.insert(snowball_vertexPos_vec.end(), cubeData.vertexPos.begin(), cubeData.vertexPos.end());
        snowball_vertexNorm_vec.insert(snowball_vertexNorm_vec.end(), cubeData.vertexNorm.begin(), cubeData.vertexNorm.end());
    }

    GLfloat snowball_vertexColors[snowball_vertexColors_vec.size()][3];
    GLfloat snowball_texCoords[snowball_texCoords_vec.size()][2];
    GLfloat snowball_vertexPos[snowball_vertexPos_vec.size()][3];
    GLfloat snowball_vertexNorm[snowball_vertexNorm_vec.size()][3];

    // Convert vectors to static arrays.
    for (int i = 0; i < snowball_vertexColors_vec.size(); ++i) {
        snowball_vertexColors[i][0] = snowball_vertexColors_vec[i].x;
        snowball_vertexColors[i][1] = snowball_vertexColors_vec[i].y;
        snowball_vertexColors[i][2] = snowball_vertexColors_vec[i].z;
    }

    for (int i = 0; i < snowball_texCoords_vec.size(); ++i) {
        snowball_texCoords[i][0] = snowball_texCoords_vec[i].x;
        snowball_texCoords[i][1] = snowball_texCoords_vec[i].y;
    }

    for (int i = 0; i < snowball_vertexPos_vec.size(); ++i) {
        snowball_vertexPos[i][0] = snowball_vertexPos_vec[i].x;
        snowball_vertexPos[i][1] = snowball_vertexPos_vec[i].y;
        snowball_vertexPos[i][2] = snowball_vertexPos_vec[i].z;
    }

    for (int i = 0; i < snowball_vertexNorm_vec.size(); ++i) {
        snowball_vertexNorm[i][0] = snowball_vertexNorm_vec[i].x;
        snowball_vertexNorm[i][1] = snowball_vertexNorm_vec[i].y;
        snowball_vertexNorm[i][2] = snowball_vertexNorm_vec[i].z;
    }

    std::cout << "snowball_vertexColors_vec.size(): " << snowball_vertexColors_vec.size() << std::endl;

    glBindVertexArray(m_Snowball_VAO);
    bufferAndSetAttribute(m_Snowball_PosBuff, 0, 3, snowball_vertexPos, snowball_vertexPos_vec.size() * 3);
    bufferAndSetAttribute(m_Snowball_NormBuff, 1, 3, snowball_vertexNorm, snowball_vertexNorm_vec.size() * 3);
    bufferAndSetAttribute(m_Snowball_ColBuff, 2, 3, snowball_vertexColors, snowball_vertexColors_vec.size() * 3);
    bufferAndSetTextureCoords(m_Snowball_TexCoordBuff, 3, 2, snowball_texCoords, snowball_texCoords_vec.size() * 2);

    glBindVertexArray(0);

    // Load shaders and compile/link them
    loadAndCompileShaders();
}

Surface::Cube Surface::populateCubeData(glm::vec3 center, float length) {
    Cube cubeData;

    float x = center.x; float y = center.y; float z = center.z;
    float rad = length / 2.0f;

    glm::vec3 ful = glm::vec3(-rad+x,  rad+y,  rad+z);
    glm::vec3 fur = glm::vec3( rad+x,  rad+y,  rad+z);
    glm::vec3 fdl = glm::vec3(-rad+x, -rad+y,  rad+z);
    glm::vec3 fdr = glm::vec3( rad+x, -rad+y,  rad+z);
    glm::vec3 bul = glm::vec3(-rad+x,  rad+y, -rad+z);
    glm::vec3 bur = glm::vec3( rad+x,  rad+y, -rad+z);
    glm::vec3 bdl = glm::vec3(-rad+x, -rad+y, -rad+z);
    glm::vec3 bdr = glm::vec3( rad+x, -rad+y, -rad+z);

    // Order: upper left, upper right, lower left, lower right
    makeFace(ful, fur, fdl, fdr, cubeData);  // front face
    makeFace(bur, bul, bdr, bdl, cubeData);  // back face
    makeFace(bul, bur, ful, fur, cubeData);  // top face
    makeFace(bdr, bdl, fdr, fdl, cubeData);  // bottom face
    makeFace(bul, ful, bdl, fdl, cubeData);  // left face
    makeFace(fur, bur, fdr, bdr, cubeData);  // right face

    return cubeData;
}

void Surface::makeFace(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight, Surface::Cube &cubeData) {
    // Vertex positions
    cubeData.vertexPos.push_back(topLeft);
    cubeData.vertexPos.push_back(bottomLeft);
    cubeData.vertexPos.push_back(bottomRight);
    cubeData.vertexPos.push_back(bottomRight);
    cubeData.vertexPos.push_back(topRight);
    cubeData.vertexPos.push_back(topLeft);

    // Vertex normals
    glm::vec3 normal = glm::normalize(glm::cross(topRight - topLeft, bottomLeft - topLeft));
    cubeData.vertexNorm.push_back(normal);
    cubeData.vertexNorm.push_back(normal);
    cubeData.vertexNorm.push_back(normal);
    cubeData.vertexNorm.push_back(normal);
    cubeData.vertexNorm.push_back(normal);
    cubeData.vertexNorm.push_back(normal);

    // Vertex colors
    cubeData.vertexColors.push_back(glm::vec3(0.95f, 0.96f, 0.94f));
    cubeData.vertexColors.push_back(glm::vec3(0.95f, 0.96f, 0.94f));
    cubeData.vertexColors.push_back(glm::vec3(0.95f, 0.96f, 0.94f));
    cubeData.vertexColors.push_back(glm::vec3(0.95f, 0.96f, 0.94f));
    cubeData.vertexColors.push_back(glm::vec3(0.95f, 0.96f, 0.94f));
    cubeData.vertexColors.push_back(glm::vec3(0.95f, 0.96f, 0.94f));

    // Texture coordinates
    cubeData.texCoords.push_back(glm::vec2(0.0f, 1.0f));
    cubeData.texCoords.push_back(glm::vec2(0.0f, 0.0f));
    cubeData.texCoords.push_back(glm::vec2(1.0f, 0.0f));
    cubeData.texCoords.push_back(glm::vec2(1.0f, 0.0f));
    cubeData.texCoords.push_back(glm::vec2(1.0f, 1.0f));
    cubeData.texCoords.push_back(glm::vec2(0.0f, 1.0f));
}

// Atlas Util: Renders the surface using the specified projection and view matrices.
void Surface::renderGeometry(const atlas::math::Matrix4 &projection, const atlas::math::Matrix4 &view)
{
    mShaders[0].enableShaders();

    const glm::mat4 mViewProj = projection * view * mModel;
    const GLint mViewProj_UNIFORMLOC = glGetUniformLocation(mShaders[0].getShaderProgram(), "ModelViewProjection");
    glUniformMatrix4fv(mViewProj_UNIFORMLOC, 1, GL_FALSE, &mViewProj[0][0]);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindVertexArray(m_Snowball_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36000);
    glBindVertexArray(0);

    mShaders[0].disableShaders();
}

// Helper function to buffer and set vertex attribute pointers.
void Surface::bufferAndSetAttribute(GLuint buffer, GLuint index, GLint size, const GLfloat data[][3], GLint dataSize)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, dataSize * sizeof(GLfloat), data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, size * sizeof(GLfloat), (GLvoid *)0);
}

// Helper function to buffer and set texture coordinates.
void Surface::bufferAndSetTextureCoords(GLuint buffer, GLuint index, GLint size, const GLfloat data[][2], GLint dataSize)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, dataSize * sizeof(GLfloat), data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, size * sizeof(GLfloat), (GLvoid *)0);
}

// Load and compile shaders.
void Surface::loadAndCompileShaders()
{
    std::vector<atlas::gl::ShaderUnit> shaderUnits
    {
        atlas::gl::ShaderUnit(generated::Shader::getShaderDirectory() + "/scene.vert", GL_VERTEX_SHADER),
        atlas::gl::ShaderUnit(generated::Shader::getShaderDirectory() + "/scene.frag", GL_FRAGMENT_SHADER)
    };

    mShaders.push_back(atlas::gl::Shader(shaderUnits));

    mShaders[0].compileShaders();
    mShaders[0].linkShaders();
}