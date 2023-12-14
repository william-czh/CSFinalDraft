#include "Snowball.hpp"
#include "Shader.hpp"
#include <glm/gtc/noise.hpp> // For glm::perlin
#include <glm/gtc/constants.hpp> // For glm::pi
#include <glm/gtc/matrix_transform.hpp> // For glm::lookAt, glm::ortho
#include <glm/gtc/noise.hpp> // For glm::perlin
#include <atlas/utils/GUI.hpp>

// Define vertex colors for the snowball (Snow White).
// Each row represents a vertex's color in RGB format.
GLfloat Snowball::vertexColors[][3] = {
    {243.0f / 255.0f, 245.0f / 255.0f, 240.0f / 255.0f}, // Vertex 0: Snow White
    {243.0f / 255.0f, 245.0f / 255.0f, 240.0f / 255.0f}, // Vertex 1: Snow White
    {243.0f / 255.0f, 245.0f / 255.0f, 240.0f / 255.0f}, // Vertex 2: Snow White
    {243.0f / 255.0f, 245.0f / 255.0f, 240.0f / 255.0f}  // Vertex 3: Snow White
};

// Define texture coordinates for the snowball.
// Each row represents a vertex's texture coordinates (u, v).
GLfloat Snowball::texCoords[][2] = {
    {0.0f, 0.0f}, // Vertex 0: (u=0, v=0)
    {1.0f, 0.0f}, // Vertex 1: (u=1, v=0)
    {0.0f, 1.0f}, // Vertex 2: (u=0, v=1)
    {1.0f, 1.0f}  // Vertex 3: (u=1, v=1)
};

// Define vertex positions for the snowball.
// Each row represents a vertex's position in 3D space (x, y, z).
float edgeSize = 10.0f;
GLfloat Snowball::vertexPos[][3] = {
    {-edgeSize, 5.0f, -edgeSize}, // Vertex 0
    {edgeSize, 5.0f, -edgeSize},  // Vertex 1
    {-edgeSize, 5.0f, edgeSize},  // Vertex 2
    {edgeSize, 5.0f, edgeSize}    // Vertex 3
};

// Define vertex normals for the snowball.
// Each row represents a vertex's normal vector in 3D space (x, y, z).
GLfloat Snowball::vertexNorm[][3] = {
    {0.0f, 1.0f, 0.0f}, // Vertex 0: Normal (0, 1, 0)
    {0.0f, 1.0f, 0.0f}, // Vertex 1: Normal (0, 1, 0)
    {0.0f, 1.0f, 0.0f}, // Vertex 2: Normal (0, 1, 0)
    {0.0f, 1.0f, 0.0f}  // Vertex 3: Normal (0, 1, 0)
};

Snowball::Snowball()
{
    // Generate OpenGL buffers and bind vertex array
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_PosBuff);
    glGenBuffers(1, &m_NormBuff);
    glGenBuffers(1, &m_ColBuff);
    glGenBuffers(1, &m_TexCoordBuff);

    glBindVertexArray(m_VAO);

    // Calculate Perlin noise for vertex positions
    for (int i = 0; i < 4; ++i)
    {
        float x = vertexPos[i][0];
        float z = vertexPos[i][2];
        float noise = glm::perlin(glm::vec2(x, z) * 0.1f); // Adjust the scale as needed
        vertexPos[i][1] = noise * 0.2f; // Adjust the amplitude as needed
    }

    // Buffer and set vertex attribute pointers for position, normal, color
    bufferAndSetAttribute(m_PosBuff, 0, 3, Snowball::vertexPos);
    bufferAndSetAttribute(m_NormBuff, 1, 3, Snowball::vertexNorm);
    bufferAndSetAttribute(m_ColBuff, 2, 3, Snowball::vertexColors);

    // Buffer and set texture coordinates
    bufferAndSetTextureCoords(m_TexCoordBuff, 3, 2, Snowball::texCoords);

    glBindVertexArray(0);

    // Load shaders and compile/link them
    loadAndCompileShaders();
}

// Atlas Util: Renders the snowball using the specified projection and view matrices.
void Snowball::renderGeometry(const atlas::math::Matrix4 &projection, const atlas::math::Matrix4 &view)
{
    mShaders[0].enableShaders();

    const glm::mat4 mViewProj = projection * view * mModel;
    const GLint mViewProj_UNIFORMLOC = glGetUniformLocation(mShaders[0].getShaderProgram(), "ModelViewProjection");
    glUniformMatrix4fv(mViewProj_UNIFORMLOC, 1, GL_FALSE, &mViewProj[0][0]);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    mShaders[0].disableShaders();
}

// Helper function to buffer and set vertex attribute pointers.
void Snowball::bufferAndSetAttribute(GLuint buffer, GLuint index, GLint size, const GLfloat data[][3])
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, size * sizeof(GLfloat), (GLvoid *)0);
}

// Helper function to buffer and set texture coordinates.
void Snowball::bufferAndSetTextureCoords(GLuint buffer, GLuint index, GLint size, const GLfloat data[][2])
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, size * sizeof(GLfloat), (GLvoid *)0);
}

// Load and compile shaders.
void Snowball::loadAndCompileShaders()
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