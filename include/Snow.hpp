#ifndef Snow_hpp
#define Snow_hpp

#include <atlas/utils/Geometry.hpp>
#include <set>
#include <vector>
#include <random>

class Snow : public atlas::utils::Geometry
{
    public:

        Snow();
        ~Snow();        

        void computeGeometry(atlas::core::Time<> const &t);

        void updateGeometry(atlas::core::Time<> const &t) override;
        
        void applyTransformations();        
        
        void setMass(float mass);
        float getMass() const;

        void setPos(glm::vec3 const &position);
        glm::vec3 getPos() const;

        void setVeloc(glm::vec3 const &velocity);
		glm::vec3 getVeloc() const;

		void setAccel(glm::vec3 const &acceleration);
        glm::vec3 getAccel() const;
        
        void setModel(glm::mat4 const &model);

        glm::mat4 getRotation() const;
        void setRotation(glm::mat4 const &rotation);        
        
        static int getSnowAmount();
        
    private:

        float m_size;
        glm::vec3 mPosition, mVelocity, mAcceleration;
        glm::vec3 computeVelocity(float deltaTime, glm::vec3 const &velocity);
        glm::vec3 computeVelocity(float deltaTime);        
		glm::vec3 computeAcceleration(float deltaTime, glm::vec3 const &velocity);		
        glm::vec3 computeAcceleration(glm::vec3 const &position, glm::vec3 const &velocity);
        
        std::default_random_engine m_Gen;
        std::normal_distribution<float> m_NormDistr;  
        std::uniform_real_distribution<float> m_UniDistr;  
                
        static int amountSnow;
        glm::mat4 mRotMat;
};

#endif