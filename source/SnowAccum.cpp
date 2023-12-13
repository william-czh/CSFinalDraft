#include "SnowAccum.hpp"
#include "Shader.hpp"
#include "SnowScene.hpp"
#include <atlas/utils/Application.hpp>
#include <atlas/utils/GUI.hpp>
#include "Asset.hpp"
#include <stb/stb_image.h>
#include <glm/gtc/type_ptr.hpp>

SnowAccum::SnowAccum() :
    m_snowAccum(true)
{    
    // Define parameters for creating the snow surface.
    int k = 50;                // Number of divisions.
    float gradX = 20.0f / k;    // Spacing along the X-axis.
    float gradZ = 20.0f / k;    // Spacing along the Z-axis.

    // Generate vertices for the snow surface.
    for(int i = 0; i <= k; ++i)
    {
        GLfloat z = -10.0f + i * gradZ;
        for(int j = 0; j <= k; ++j)
        {
            GLfloat x = -10.0f + j * gradX;
            m_alphaPos.push_back(glm::vec4(x, 0.005f, z, 0.0f)); // Vertex position (x, y, z, alpha).
            mNormals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));      // Vertex normal (for shading).
            m_TexCoords.push_back(glm::vec2((float)(j + 1) / (k + 2), (float)(i + 1) / (k + 2))); // Texture coordinates.
        }        
    }

    // Generate indices for rendering triangles.
    for(int i = 0; i <= k - 1; ++i)
    {
        for(int j = 0; j <= k; ++j)
        {
            mIndices.push_back((k + 1) * i + j);
            mIndices.push_back((k + 1) * (i + 1) + j);
        }
        mIndices.push_back(0xFFFFFFFF); // Restart primitive.
    }

    int idxOffset = m_alphaPos.size();
    
    // Create vertices for the side of the snow accumulation.

    // Along the negative Z-axis side.
    for(int i = 0; i <= k; ++i)
    {
        GLfloat x = -10.0f + i * gradX;
        m_alphaPos.push_back(glm::vec4(x, 0.005f, -10.0f, 1.0f)); // Vertex position with higher alpha (solid).
        mNormals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));        // Normal pointing in the negative Z-axis.
        m_TexCoords.push_back(glm::vec2((float)(i + 1) / (k + 2), 0.0f));
    }     

    // Along the positive X-axis side.
    for(int i = 0; i <= k; ++i)
    {
        GLfloat z = -10.0f + i * gradZ;
        m_alphaPos.push_back(glm::vec4(10.0f, 0.005f, z, 1.0f)); // Vertex position with higher alpha (solid).
        mNormals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));        // Normal pointing in the positive X-axis.
        m_TexCoords.push_back(glm::vec2(1.0f, (float)(i + 1) / (k + 2)));
    } 

    // Along the positive Z-axis side.
    for(int i = 0; i <= k; ++i)
    {
        GLfloat x = 10.0f - i * gradX;
        m_alphaPos.push_back(glm::vec4(x, 0.005f, 10.0f, 1.0f)); // Vertex position with higher alpha (solid).
        mNormals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));        // Normal pointing in the positive Z-axis.
        m_TexCoords.push_back(glm::vec2((float)(k + 1 - i) / (k + 2), 1.0f));
    } 

    // Along the negative X-axis side.
    for(int i = 0; i <= k; ++i)
    {
        GLfloat z = 10.0f - i * gradZ;
        m_alphaPos.push_back(glm::vec4(-10.0f, 0.005f, z, 1.0f)); // Vertex position with higher alpha (solid).
        mNormals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));        // Normal pointing in the negative X-axis.
        m_TexCoords.push_back(glm::vec2(0.0f, (float)(k + 1 - i) / (k + 2)));
    } 
    
    mIndices.push_back(0xFFFFFFFF); // Restart primitive.
    
    // Define OpenGL buffers and generate vertex array.
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_AlphaBuffPos);
    glGenBuffers(1, &mNormBuff);
    glGenBuffers(1, &mTexCoordBuff);
    glGenBuffers(1, &m_IdxBuff);    

    glBindVertexArray(m_VAO);            

    // Bind and buffer vertex positions with alpha values.
	glBindBuffer(GL_ARRAY_BUFFER, m_AlphaBuffPos);
	glBufferData(GL_ARRAY_BUFFER, 4 * m_alphaPos.size() * sizeof(GLfloat), m_alphaPos.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

    // Bind and buffer vertex normals.
    glBindBuffer(GL_ARRAY_BUFFER, mNormBuff);
	glBufferData(GL_ARRAY_BUFFER, 3 * mNormals.size() * sizeof(GLfloat), mNormals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    // Bind and buffer texture coordinates.
    glBindBuffer(GL_ARRAY_BUFFER, mTexCoordBuff);
	glBufferData(GL_ARRAY_BUFFER, 2 * m_TexCoords.size() * sizeof(GLfloat), m_TexCoords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    // Bind and buffer indices for rendering.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IdxBuff);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLuint), mIndices.data(), GL_STATIC_DRAW);
       
    glBindVertexArray(0);
    
    // Load and configure normal map texture.
    int imgW, imgH, imgColor;    
    stbi_set_flip_vertically_on_load(1);    
    stbi_uc* image = stbi_load((generated::Asset::getAssetDirectory() + "/snow_normal_map.jpg").c_str(), &imgW, &imgH, &imgColor, 3);
    stbi_set_flip_vertically_on_load(0);
    
    glGenTextures(1, &m_NormTexID);
    glActiveTexture(GL_TEXTURE2);    
    glBindTexture(GL_TEXTURE_2D, m_NormTexID);
    
    if(imgColor == 3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgW, imgH, 0, GL_RGB, GL_UNSIGNED_BYTE, image);        
    }
    else if(imgColor == 4)
    {   
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgW, imgH, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);        
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);        

    stbi_image_free(image);
    
    // Load and compile shaders.
    std::vector<atlas::gl::ShaderUnit> su
    {
        atlas::gl::ShaderUnit(generated::Shader::getShaderDirectory() + "/SnowAccum.vert", GL_VERTEX_SHADER),
        atlas::gl::ShaderUnit(generated::Shader::getShaderDirectory() + "/SnowAccum.frag", GL_FRAGMENT_SHADER)
    };
    
    mShaders.push_back(atlas::gl::Shader(su));
    
    mShaders[0].compileShaders();
    mShaders[0].linkShaders(); 
}


//Deconstructor
SnowAccum::~SnowAccum()
{
}


void SnowAccum::renderGeometry(atlas::math::Matrix4 const &projection, atlas::math::Matrix4 const &view)
{
    // Enable the shaders.
    mShaders[0].enableShaders();

    // Calculate the Model-View-Projection matrix.
    glm::mat4 m_ViewProj = projection * view * mModel;
    const GLint mViewProj_UNIFORMLOC = glGetUniformLocation(mShaders[0].getShaderProgram(), "ModelViewProjection");
    glUniformMatrix4fv(mViewProj_UNIFORMLOC, 1, GL_FALSE, &m_ViewProj[0][0]);

    // Set up the sky matrix for rendering the snow accumulation.
    glm::mat4 viewSky = glm::lookAt(glm::vec3(0.0f, 15.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 projSky = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 100.0f);
    glm::mat4 matOffset = glm::mat4(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f);
    glm::mat4 matSky = matOffset * projSky * viewSky * mModel;
    const GLint matSky_UNILOC = glGetUniformLocation(mShaders[0].getShaderProgram(), "SkyMatrix");
    glUniformMatrix4fv(matSky_UNILOC, 1, GL_FALSE, &matSky[0][0]);

    // Set texture uniforms for snow accumulation and normal map.
    glActiveTexture(GL_TEXTURE1);
    GLint sceneSnowAccum_UNILOC = glGetUniformLocation(mShaders[0].getShaderProgram(), "SnowMap");
    GLint sceneToggleAccum_UNILOC = glGetUniformLocation(mShaders[0].getShaderProgram(), "UseSnowMap");
    glUniform1i(sceneSnowAccum_UNILOC, 1);
    glUniform1i(sceneToggleAccum_UNILOC, m_snowAccum);
    glBindTexture(GL_TEXTURE_2D, ((SnowScene *)atlas::utils::Application::getInstance().getCurrentScene())->getSnowFall().getSnowDepthTexture());

    glActiveTexture(GL_TEXTURE2);
    GLint sceneNorm_UNILOC = glGetUniformLocation(mShaders[0].getShaderProgram(), "NormalMap");
    GLint sceneToggleNorm_UNILOC = glGetUniformLocation(mShaders[0].getShaderProgram(), "UseNormalMap");
    glUniform1i(sceneNorm_UNILOC, 2);
    glUniform1i(sceneToggleNorm_UNILOC, true);
    glBindTexture(GL_TEXTURE_2D, m_NormTexID);

    // Set camera and light positions.
    GLint sceneCamPos_UNILOC = glGetUniformLocation(mShaders[0].getShaderProgram(), "CameraPosition");
    glUniform3fv(sceneCamPos_UNILOC, 1, value_ptr(((SnowScene *)atlas::utils::Application::getInstance().getCurrentScene())->getCameraPosition()));

    GLint sceneLightPos_UNILOC = glGetUniformLocation(mShaders[0].getShaderProgram(), "LightPosition");
    glUniform3fv(sceneLightPos_UNILOC, 1, value_ptr(((SnowScene *)atlas::utils::Application::getInstance().getCurrentScene())->getLightPosition()));

    // Enable primitive restart and bind vertex array.
    glPrimitiveRestartIndex(0xFFFFFFFF);
    glEnable(GL_PRIMITIVE_RESTART);
    glBindVertexArray(m_VAO);

    // Draw the elements using triangle strips.
    glDrawElements(GL_TRIANGLE_STRIP, mIndices.size(), GL_UNSIGNED_INT, (void *)0);

    glBindVertexArray(0);

    // Disable primitive restart and shaders.
    glDisable(GL_PRIMITIVE_RESTART);
    mShaders[0].disableShaders();
}

void SnowAccum::updateGeometry(atlas::core::Time<> const &t)
{
    // Initialize normals with zero vectors.
    mNormals = std::vector<glm::vec3>(m_alphaPos.size(), glm::vec3(0.0, 0.0, 0.0));

    bool flip = false;
    for (int i = 0; i < mIndices.size() - 2; ++i)
    {
        int idx = mIndices[i];
        int idx2 = mIndices[i + 1];
        int idx3 = mIndices[i + 2];

        if (idx == 0xFFFFFFFF || idx2 == 0xFFFFFFFF || idx3 == 0xFFFFFFFF)
        {
            flip = false;
            continue;
        }

        // Retrieve vertices a, b, and c from m_alphaPos based on indices.
        glm::vec3 a(m_alphaPos[idx]);
        glm::vec3 b(m_alphaPos[idx2]);
        glm::vec3 c(m_alphaPos[idx3]);

        // Calculate normal for the triangle formed by a, b, and c.
        glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));

        // Update normals for each vertex.
        mNormals[idx] += normal;
        mNormals[idx2] += normal;
        mNormals[idx3] += normal;

        // If flip is true, reverse the order of vertices.
        if (flip)
        {
            std::swap(mNormals[idx], mNormals[idx2]);
        }

        // Toggle flip for the next iteration.
        flip = !flip;
    }

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_AlphaBuffPos);

    // Update the vertex buffer data with the new alpha positions.
    glBufferData(GL_ARRAY_BUFFER, 4 * m_alphaPos.size() * sizeof(GLfloat), m_alphaPos.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void SnowAccum::drawGui()
{
    ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiSetCond_FirstUseEver);

    // Create an ImGui window for snow accumulation options.
    ImGui::Begin("Snow Accumulation Options");
    ImGui::Checkbox("Toggle Snow Accumulation", &m_snowAccum);
    ImGui::End();
}


void SnowAccum::refreshNearestVert(glm::vec3 const &query)
{
    // Define the maximum distance for updating alpha positions.
    float distanceMax = 1.0;
    
    // Define a small constant k.
    float k = 0.005f;
    
    int i = 0;
    
    // Iterate through the alpha positions.
    for (glm::vec4 &alphaPos : m_alphaPos)
    {
        // Break the loop if we've processed all vertices.
        if (i >= 51 * 51)
        {
            break;
        }
        
        ++i;

        // Calculate the Euclidean distance between the query point and alpha position.
        float dist = glm::length(query - glm::vec3(alphaPos));

        // Calculate the amount to increase alpha based on the distance.
        // Ensure that the amount does not exceed 0.005f.
        float amount = dist > distanceMax ? 0.0f : std::min(0.005f, k / dist);

        // Update the alpha position's w component.
        alphaPos.w += amount;

        // If alpha position's w component exceeds 1.0, increase the y component.
        if (alphaPos.w >= 1.0f)
        {
            alphaPos.y += 0.3f * amount;
        }
    }
}
