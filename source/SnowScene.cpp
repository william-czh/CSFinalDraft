#include "SnowScene.hpp"
#include "Snow.hpp"
#include "SnowfallGenerator.hpp"
#include "Surface.hpp"
#include "SnowAccum.hpp"
#include "Shader.hpp"
#include "Snowball.hpp"

#include <atlas/core/GLFW.hpp>
#include <atlas/utils/GUI.hpp>
#include <glm/gtc/type_ptr.hpp>

SnowScene::SnowScene() :
    m_snowPause(true),
    mRow(5.0),
    mTheta(0.0),
    m_LightCoords(-25.0f, 15.0f, -25.0f)
{
    // Create SnowfallGenerator and set its bounding box.
    std::unique_ptr<SnowfallGenerator> snowfallGen = std::make_unique<SnowfallGenerator>();
    snowfallGen->setBBox(glm::vec3(-10.5f, 12.0f, -10.5f), glm::vec3(10.5f, 12.0f, 10.5f));

    // Create Surface.
    std::unique_ptr<Surface> platform = std::make_unique<Surface>();

    // Create Snowballs.
    // std::unique_ptr<Snowball> snowball = std::make_unique<Snowball>();

    // Add geometries to the vector.
    mGeometries.push_back(std::move(snowfallGen));
    mGeometries.push_back(std::move(platform));
    // mGeometries.push_back(std::move(snowball));
}

SnowScene::~SnowScene()
{
}

//Atlas Util Functions
void SnowScene::mousePressEvent(int button, int action, int modifiers, double xPos, double yPos)
{
    atlas::utils::Gui::getInstance().mousePressed(button, action, modifiers);
}

void SnowScene::mouseMoveEvent(double xPos, double yPos)
{
    atlas::utils::Gui::getInstance().mouseMoved(xPos, yPos);
}

void SnowScene::mouseScrollEvent(double xOffset, double yOffset)
{
    atlas::utils::Gui::getInstance().mouseScroll(xOffset, yOffset);
}

void SnowScene::keyPressEvent(int key, int scancode, int action, int mods)
{
    // Check for both WASD and arrow key input.
    switch (key)
    {
        case GLFW_KEY_RIGHT:
        case GLFW_KEY_D:
            mTheta -= 2.0 * (M_PI / 180.0f);
            break;
        case GLFW_KEY_LEFT:
        case GLFW_KEY_A:
            mTheta += 2.0 * (M_PI / 180.0f);
            break;
        case GLFW_KEY_UP:
        case GLFW_KEY_W:
            mRow += 0.1;
            break;
        case GLFW_KEY_DOWN:
        case GLFW_KEY_S:
            mRow -= 0.1;
            break;
    }
    atlas::utils::Gui::getInstance().keyPress(key, scancode, action, mods);
}


void SnowScene::screenResizeEvent(int width, int height)
{
    glViewport(0, 0, width, height);

    float aspectRatio = (float)width / height;
    mProjection = glm::perspective(glm::radians(70.0f), aspectRatio, 0.01f, 100.0f);

    atlas::utils::Gui::getInstance().screenResize(width, height);

    mWidth = (float)width;
    mHeight = (float)height;
}

void SnowScene::renderScene()
{
    atlas::utils::Gui::getInstance().newFrame();

    // Set up camera parameters.
    float aspectRatio = 1.0f;
    glm::vec3 eye(20.0f * cos(mTheta), mRow, 20.0f * sin(mTheta));
    glm::vec3 look(0.0, 0.0, 0.0);
    glm::vec3 up(0.0, 1.0, 0.0);
    glm::mat4 view = glm::lookAt(eye, look, up);

    // Clear the screen.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render ImGui window for simulation parameters
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Simulation Parameters");
    ImGui::Text("Snow Particles: %d", Snow::getSnowAmount());    
	ImGui::Checkbox("Snow Paused", &m_snowPause);
    ImGui::SliderFloat3("Wind Direction", value_ptr(m_forceDir), -50.0f, 50.0f);
    ImGui::SliderFloat3("Light Coordinates", value_ptr(m_LightCoords), -25.0f, 25.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::End();

    // Render SnowFall geometry.
    m_SnowFall.renderGeometry(mProjection, view);

    // Render other geometries.
    for (auto &geometry : mGeometries)
    {
        geometry->drawGui();
        geometry->renderGeometry(mProjection, view);
    }

    // Render SnowAccum geometry.
    m_SnowAccum.renderGeometry(mProjection, view);
    m_SnowAccum.drawGui();

    // Render ImGui.
    ImGui::Render();
}

void SnowScene::updateScene(double time)
{
    double delta = time - mTime.currentTime;

    mTime.deltaTime = delta;
    mTime.totalTime += mTime.deltaTime;
    mTime.currentTime += delta;

    atlas::utils::Gui::getInstance().update(mTime);

    if (!m_snowPause)
    {
        for (auto &geometry : mGeometries)
        {
            geometry->updateGeometry(mTime);
        }
        m_SnowFall.updateGeometry(mTime);
        m_SnowAccum.updateGeometry(mTime);
    }
}

void SnowScene::addSnow(std::unique_ptr<Snow> snow)
{
    m_SnowFall.addSnow(std::move(snow));
}

SnowFall const &SnowScene::getSnowFall() const
{
    return m_SnowFall;
}

SnowAccum &SnowScene::getSnowAccum()
{
    return m_SnowAccum;
}

glm::vec3 SnowScene::getForceWind()
{
    return m_forceDir;
}

glm::vec3 SnowScene::getCameraPosition() const
{
    return glm::vec3(20.0f * cos(mTheta), mRow, 20.0f * sin(mTheta));
}

glm::vec3 SnowScene::getLightPosition() const
{
    return m_LightCoords;
}

void SnowScene::onSceneEnter()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void SnowScene::onSceneExit()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}
