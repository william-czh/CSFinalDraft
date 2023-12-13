#include "Surface.hpp"
#include "Shader.hpp"
#include <glm/gtc/noise.hpp> // For glm::perlin
#include <glm/gtc/constants.hpp> // For glm::pi
#include <glm/gtc/matrix_transform.hpp> // For glm::lookAt, glm::ortho
#include <glm/gtc/noise.hpp> // For glm::perlin
#include <atlas/utils/GUI.hpp>

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
    for (int i = 0; i < 4; ++i)
    {
        float x = vertexPos[i][0];
        float z = vertexPos[i][2];
        float noise = glm::perlin(glm::vec2(x, z) * 0.1f); // Adjust the scale as needed
        vertexPos[i][1] = noise * 0.2f; // Adjust the amplitude as needed
    }

    // Buffer and set vertex attribute pointers for position, normal, color
    bufferAndSetAttribute(m_PosBuff, 0, 3, Surface::vertexPos);
    bufferAndSetAttribute(m_NormBuff, 1, 3, Surface::vertexNorm);
    bufferAndSetAttribute(m_ColBuff, 2, 3, Surface::vertexColors);

    // Buffer and set texture coordinates
    bufferAndSetTextureCoords(m_TexCoordBuff, 3, 2, Surface::texCoords);

    glBindVertexArray(0);

    // Load shaders and compile/link them
    loadAndCompileShaders();
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

    mShaders[0].disableShaders();
}

// Helper function to buffer and set vertex attribute pointers.
void Surface::bufferAndSetAttribute(GLuint buffer, GLuint index, GLint size, const GLfloat data[][3])
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, size * sizeof(GLfloat), (GLvoid *)0);
}

// Helper function to buffer and set texture coordinates.
void Surface::bufferAndSetTextureCoords(GLuint buffer, GLuint index, GLint size, const GLfloat data[][2])
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), data, GL_STATIC_DRAW);
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