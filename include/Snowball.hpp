#ifndef Snowball_hpp
#define Snowball_hpp

#include <atlas/utils/Geometry.hpp>

class Snowball : public atlas::utils::Geometry
{
    public:

        Snowball();

        void renderGeometry(atlas::math::Matrix4 const &proj, atlas::math::Matrix4 const &view) override;    
                        
    private:
        
        GLuint m_VAO;
        GLuint m_PosBuff, m_NormBuff, m_ColBuff, m_TexCoordBuff;   
  
        static GLfloat vertexPos[][3], vertexNorm[][3], vertexColors[][3], texCoords[][2];   
        void loadAndCompileShaders();
        void bufferAndSetTextureCoords(GLuint buffer, GLuint index, GLint size, const GLfloat data[][2]);
        void bufferAndSetAttribute(GLuint buffer, GLuint index, GLint size, const GLfloat data[][3]);
        // void generatePerlinNoise();
};

#endif