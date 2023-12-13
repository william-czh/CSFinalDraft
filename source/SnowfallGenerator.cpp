#include <iostream> 

#include "SnowfallGenerator.hpp"
#include "Snow.hpp"
#include "SnowScene.hpp"
#include "Shader.hpp"

#include <atlas/utils/Application.hpp>
#include <atlas/utils/GUI.hpp>

SnowfallGenerator::SnowfallGenerator() :
    m_SnowingRate(100),
    m_accumSnow(0.0f),
    m_UniDistrVec(0.0f, 1.0f),
    m_UniDistrAngle(0.0f, (float)(2.0 * M_PI))
{    
}

// Set the bounding box for snow generation.
void SnowfallGenerator::setBBox(glm::vec3 const &a, glm::vec3 const &b)
{
    m_BBoxA = a;
    m_BBoxB = b;

    // Initialize random number distributions for bounding box dimensions.
    m_UniDistrX = std::uniform_real_distribution<float>(std::min(a.x, b.x), std::max(a.x, b.x));
    m_UniDistrY = std::uniform_real_distribution<float>(std::min(a.y, b.y), std::max(a.y, b.y));       
    m_UniDistrZ = std::uniform_real_distribution<float>(std::min(a.z, b.z), std::max(a.z, b.z));         
}

// Update the snow geometry.
void SnowfallGenerator::updateGeometry(atlas::core::Time<> const &t)
{
    // Calculate the number of snowflakes to add based on the snow rate.
    float rate = m_SnowingRate * t.deltaTime;
    int amountNewSnow = static_cast<int>(rate);
    m_accumSnow += rate - amountNewSnow;

    int addedSnow = static_cast<int>(m_accumSnow);
    amountNewSnow += addedSnow;

    m_accumSnow -= addedSnow;

    auto currentScene = dynamic_cast<SnowScene*>(atlas::utils::Application::getInstance().getCurrentScene());
    if (!currentScene) return;

    for (int i = 0; i < amountNewSnow; ++i)
    {
        // To prevent excess snow for performance reasons
        if (Snow::getSnowAmount() >= 9000)
        {
            // Display a message indicating the max snow rate has been exceeded.
            std::cout << "Maximum snow rate exceeded!" << std::endl;
            break;
        }

        std::unique_ptr<Snow> snow = std::make_unique<Snow>();
        snow->setPos(glm::vec3(m_UniDistrX(m_Gen), 
                                         m_UniDistrY(m_Gen), 
                                         m_UniDistrZ(m_Gen)));
        snow->setVeloc(glm::vec3(0.0f, 0.0f, 0.0f));

        float theta = acos(2.0 * m_UniDistrVec(m_Gen) - 1);
        float phi = 2.0 * M_PI * m_UniDistrVec(m_Gen);  
        
        glm::vec3 rotVec = glm::vec3(sin(theta) * cos(phi), 
                                             sin(theta) * sin(phi), 
                                             cos(theta));
    
        float angTheta = m_UniDistrAngle(m_Gen);
    
        snow->setRotation(glm::rotate(glm::mat4(1.0f), angTheta, rotVec));
        
        currentScene->addSnow(std::move(snow));
    }
}

// Draw GUI options for the snow cloud.
void SnowfallGenerator::drawGui()
{
    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
	
    ImGui::Begin("Snowfall Generator Options");
    ImGui::SliderInt("Snow/sec", &m_SnowingRate, 0, 800);
    ImGui::End();
}
