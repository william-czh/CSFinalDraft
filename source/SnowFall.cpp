#include "SnowFall.hpp"
#include "Shader.hpp"
#include <atlas/utils/GUI.hpp>

SnowFall::SnowFall()
{        
    // Initialize snow map resolution and create a depth texture.
    m_MapRes = 8192;
    glGenTextures(1, &m_DepthTex);
    glBindTexture(GL_TEXTURE_2D, m_DepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_MapRes, m_MapRes, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    
    // Set texture parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    const float borderShadow[] = { 2.0f, 2.0f, 2.0f, 2.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderShadow);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Create and configure the framebuffer.
    GLint currFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currFBO);
    glGenFramebuffers(1, &m_SnowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_SnowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTex, 0);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Check framebuffer status.
    GLenum FBOStat = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (FBOStat != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "glCheckFramebufferStatus: %x\n", FBOStat);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Generate vertex arrays and buffers for snowfall geometry.
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_PosBuff);
    glGenBuffers(1, &m_ColBuff);
    glGenBuffers(1, &m_IdxBuff);

    glBindVertexArray(m_VAO);

    // Buffer and set vertex attribute pointers for position and color.
    glBindBuffer(GL_ARRAY_BUFFER, m_PosBuff);
    glBufferData(GL_ARRAY_BUFFER, 0, m_VertPos.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, m_ColBuff);
    glBufferData(GL_ARRAY_BUFFER, 0, m_VertColors.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    glBindVertexArray(0);

    // Load shaders for snow surface and falling snow.
    std::vector<atlas::gl::ShaderUnit> su_Snow
    {
        atlas::gl::ShaderUnit(generated::Shader::getShaderDirectory() + "/SnowSurface.vert", GL_VERTEX_SHADER),
        atlas::gl::ShaderUnit(generated::Shader::getShaderDirectory() + "/SnowSurface.frag", GL_FRAGMENT_SHADER)
    };

    std::vector<atlas::gl::ShaderUnit> su
    {
        atlas::gl::ShaderUnit(generated::Shader::getShaderDirectory() + "/SnowFalling.vert", GL_VERTEX_SHADER),
        atlas::gl::ShaderUnit(generated::Shader::getShaderDirectory() + "/SnowFalling.frag", GL_FRAGMENT_SHADER)
    };

    mShaders.push_back(atlas::gl::Shader(su_Snow));
    mShaders.push_back(atlas::gl::Shader(su));

    // Compile and link shaders.
    mShaders[0].compileShaders();
    mShaders[0].linkShaders();
    mShaders[1].compileShaders();
    mShaders[1].linkShaders();

    // Initialize the snowflake size distribution.
    m_SnowSizeDistr = std::normal_distribution<float>(0.0025f, 0.009f);
}

SnowFall::~SnowFall()
{
}

void SnowFall::addSnow(std::unique_ptr<Snow> snowflake)
{
    m_Snowflakes.push_back(std::move(snowflake));
}

void SnowFall::updateGeometry(atlas::core::Time<> const &t)
{
        // Clear previous vertex data.
        m_VertPos.clear();
        m_VertColors.clear();
        mVertexIndices.clear();

        int i = 0;
        // Calculate hexagon vertices once.
        std::vector<glm::vec3> hexagonVertices;
        const float hexagonRadius = 0.03f; 
        for (int j = 0; j < 6; ++j)
        {
            float angle = static_cast<float>(j) * 60.0f * glm::pi<float>() / 180.0f;
            float x = hexagonRadius * cos(angle);
            float y = hexagonRadius * sin(angle);
            hexagonVertices.push_back(glm::vec3(x, y, 0.0f));
        }

        // Inside the loop where you add snowflakes:
        for (auto &snowFlake : m_Snowflakes)
        {
            snowFlake->updateGeometry(t);

            // Calculate snowflake size using hexagonal distribution.
            float snowflakeSize = m_SnowSizeDistr(m_Gen);

            glm::vec3 snowflakePos = snowFlake->getPos();

            for (const auto &vertex : hexagonVertices)
            {
                glm::vec3 transformedVertex = snowflakePos + glm::vec3(glm::vec4(vertex, 0.0) * snowFlake->getRotation());
                m_VertPos.push_back(transformedVertex);
                m_VertColors.push_back(glm::vec3(1.0f, 1.0f, 1.0f)); // Color for the hexagonal snowflake
            }

            // Define indices to create triangles for the hexagon.
            const std::vector<GLuint> hexagonIndices = { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5 };
            GLuint baseIndex = 6 * i;
            for (const auto &index : hexagonIndices)
            {
                mVertexIndices.push_back(baseIndex + index);
            }

            ++i;
        }


    glBindVertexArray(m_VAO);

    // Buffer vertex data.
    glBindBuffer(GL_ARRAY_BUFFER, m_PosBuff);
    glBufferData(GL_ARRAY_BUFFER, 3 * m_VertPos.size() * sizeof(GLfloat), m_VertPos.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_ColBuff);
    glBufferData(GL_ARRAY_BUFFER, 3 * m_VertColors.size() * sizeof(GLfloat), m_VertColors.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IdxBuff);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mVertexIndices.size() * sizeof(GLint), mVertexIndices.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(0);

    // Remove snow that is below the threshold
    m_Snowflakes.erase(std::remove_if(
    m_Snowflakes.begin(), m_Snowflakes.end(),
    [](const std::unique_ptr<Snow> &snowflake) {
        return snowflake->getPos().y < 0;
    }),
    m_Snowflakes.end());

}


void SnowFall::renderGeometry(atlas::math::Matrix4 const &projection, atlas::math::Matrix4 const &view)
{
    // Render snow surface.
    {
    // Store the current viewport.
    GLint m_viewport[4];
    glGetIntegerv(GL_VIEWPORT, m_viewport);

    // Enable the snow surface shader.
    mShaders[0].enableShaders();
    
    // Bind the snow framebuffer and clear the depth buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, m_SnowFBO);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_MapRes, m_MapRes);
    glEnable(GL_DEPTH_TEST);

    // Enable polygon offset for shadow mapping.
    glEnable(GL_POLYGON_OFFSET_FILL);

    // Set up the view and projection matrices for the skybox.
    glm::mat4 viewSky = glm::lookAt(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 projSky = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 100.0f);            
    glm::mat4 m_ViewProj = projSky * viewSky * mModel;

    // Set shader uniforms.
    const GLint MODEL_VIEW_PROJECTION_UNIFORM_LOCATION = glGetUniformLocation(mShaders[0].getShaderProgram(), "ModelViewProjection");
    glUniformMatrix4fv(MODEL_VIEW_PROJECTION_UNIFORM_LOCATION, 1, GL_FALSE, &m_ViewProj[0][0]);

    const GLint MODEL_UNIFORM_LOCATION = glGetUniformLocation(mShaders[0].getShaderProgram(), "Model");
    glUniformMatrix4fv(MODEL_UNIFORM_LOCATION, 1, GL_FALSE, &mModel[0][0]);

    // Bind vertex array and draw the snow surface.
    glBindVertexArray(m_VAO);  
    glDrawElements(GL_TRIANGLES, mVertexIndices.size(), GL_UNSIGNED_INT, (void *) 0);
    glBindVertexArray(0);

    // Disable the snow surface shader.
    mShaders[0].disableShaders();
    
    // Disable polygon offset.
    glPolygonOffset(0.0f, 0.0f);        
    glDisable(GL_POLYGON_OFFSET_FILL);

    // Restore the original framebuffer and viewport.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);     
    glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
    }

    // Render falling snow.
    {
    // Enable the falling snow shader.
    mShaders[1].enableShaders();
    
    // Set up the model-view-projection matrix for falling snow.
    glm::mat4 m_ViewProj = projection * view * mModel;
    const GLint MODEL_VIEW_PROJECTION_UNIFORM_LOCATION = glGetUniformLocation(mShaders[1].getShaderProgram(), "ModelViewProjection");
    glUniformMatrix4fv(MODEL_VIEW_PROJECTION_UNIFORM_LOCATION, 1, GL_FALSE, &m_ViewProj[0][0]);      

    // Bind vertex array and draw falling snow.
    glBindVertexArray(m_VAO);        
    glDrawElements(GL_TRIANGLES, mVertexIndices.size(), GL_UNSIGNED_INT, (void *) 0);
    glBindVertexArray(0); 

    // Disable the falling snow shader.
    mShaders[1].disableShaders();
    }
}

GLuint SnowFall::getSnowDepthTexture() const
{
    return m_DepthTex;
}