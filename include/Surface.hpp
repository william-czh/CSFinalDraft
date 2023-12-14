#ifndef Surface_hpp
#define Surface_hpp

#include <atlas/utils/Geometry.hpp>

class Surface : public atlas::utils::Geometry
{
    public:

        Surface();

        void renderGeometry(atlas::math::Matrix4 const &proj, atlas::math::Matrix4 const &view) override;    

        struct Cube {
            std::vector<glm::vec3> vertexColors;
            std::vector<glm::vec2> texCoords;
            std::vector<glm::vec3> vertexPos;
            std::vector<glm::vec3> vertexNorm;
        };
                        
    private:
        
        GLuint m_VAO;
        GLuint m_PosBuff, m_NormBuff, m_ColBuff, m_TexCoordBuff;

        GLuint m_Snowball_VAO;
        GLuint m_Snowball_PosBuff, m_Snowball_NormBuff, m_Snowball_ColBuff, m_Snowball_TexCoordBuff;

        static GLfloat vertexPos[][3], vertexNorm[][3], vertexColors[][3], texCoords[][2];
        // static GLfloat snowball_vertexPos[][3], snowball_vertexNorm[][3], snowball_vertexColors[][3], snowball_texCoords[][2];

        void loadAndCompileShaders();
        void bufferAndSetTextureCoords(GLuint buffer, GLuint index, GLint size, const GLfloat data[][2], GLint dataSize);
        void bufferAndSetAttribute(GLuint buffer, GLuint index, GLint size, const GLfloat data[][3], GLint dataSize);

        Cube populateCubeData(glm::vec3 center, float length);
        void makeFace(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight, Surface::Cube &cubeData);
};

#endif