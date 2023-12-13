#ifndef SnowFall_hpp
#define SnowFall_hpp

#include "Snow.hpp"
#include <atlas/utils/Geometry.hpp>
#include <vector>

class SnowFall : public atlas::utils::Geometry
{
    public:

        SnowFall();
        ~SnowFall();        

        void updateGeometry(atlas::core::Time<> const &t) override;        
        void renderGeometry(atlas::math::Matrix4 const &projection, atlas::math::Matrix4 const &view) override;    

        void addSnow(std::unique_ptr<Snow> snowflake); 
        GLuint getSnowDepthTexture() const;       
                
    private:

        int m_MapRes;
        GLuint m_SnowFBO;
        GLuint m_DepthTex;
        
        GLuint m_VAO;
        GLuint m_PosBuff, m_ColBuff, m_IdxBuff;        
        
        std::vector<glm::vec3> m_VertPos, m_VertColors;
        std::vector<GLint> mVertexIndices;

        std::vector<std::unique_ptr<Snow>> m_Snowflakes;     
        
        std::default_random_engine m_Gen;        
        std::normal_distribution<float> m_SnowSizeDistr;                  
};

#endif