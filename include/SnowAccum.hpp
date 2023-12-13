#ifndef SnowAccum_hpp
#define SnowAccum_hpp

#include <atlas/utils/Geometry.hpp>
#include <vector>

class SnowAccum : public atlas::utils::Geometry
{
    public:

        SnowAccum();
        ~SnowAccum();

        //Atlas GUI
        void renderGeometry(atlas::math::Matrix4 const &projection, atlas::math::Matrix4 const &view) override;  
        void updateGeometry(atlas::core::Time<> const &t) override;   
        void drawGui() override;         

        void refreshNearestVert(glm::vec3 const &query);        
                        
    private:
        
        GLuint m_VAO;
        GLuint m_AlphaBuffPos, mNormBuff, mTexCoordBuff, m_IdxBuff;  

        GLuint m_NormTexID;             
        
        int mNumVertices;

        std::vector<glm::vec4> m_alphaPos;
        std::vector<glm::vec3> mNormals; 
        std::vector<glm::vec2> m_TexCoords;
        std::vector<GLuint> mIndices;
        
        bool m_snowAccum;
};

#endif