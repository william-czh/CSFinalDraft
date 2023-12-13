#ifndef SnowfallGenerator_hpp
#define SnowfallGenerator_hpp

#include <atlas/utils/Geometry.hpp>
#include <random>

class SnowfallGenerator : public atlas::utils::Geometry
{
    public:

        SnowfallGenerator();

        void updateGeometry(atlas::core::Time<> const &t) override;
        void drawGui();
                
        void setBBox(glm::vec3 const &a, glm::vec3 const &b);
        
    private:

        glm::vec3 m_BBoxA, m_BBoxB;

        std::default_random_engine m_Gen;
        std::uniform_real_distribution<float> m_UniDistrX, m_UniDistrY, m_UniDistrZ;
        std::uniform_real_distribution<float> m_UniDistrVec, m_UniDistrAngle;        

        int m_SnowingRate;
        
        float m_accumSnow;
              
};

#endif