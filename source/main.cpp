//Uses Atlas GUI Toolkit: https://github.com/marovira/atlas
#include <atlas/utils/Application.hpp>
#include <atlas/utils/WindowSettings.hpp>
#include <atlas/gl/ErrorCheck.hpp>
#include "SnowScene.hpp"

int main()
{
    // Set OpenGL error severity to catch high and medium severity errors.
    atlas::gl::setGLErrorSeverity(ATLAS_GL_ERROR_SEVERITY_HIGH | ATLAS_GL_ERROR_SEVERITY_MEDIUM);

    // Configure window settings.
    atlas::utils::WindowSettings settings;
    settings.title = "Big Bucks Animations - Snow Simulation";
    settings.contextVersion = atlas::utils::ContextVersion(4, 1);
    settings.isForwardCompat = true;
    settings.windowSize = std::make_tuple(1024, 1024);

    // Get the application instance.
    auto &application = atlas::utils::Application::getInstance();

    // Create the application window with specified settings.
    application.createWindow(settings);

    // Create a unique pointer to the SnowScene.
    atlas::utils::ScenePointer snowScene = std::make_unique<SnowScene>();

    // Add the SnowScene to the application.
    application.addScene(std::move(snowScene));

    // Run the application loop.
    application.runApplication();

    return 0;
}
