#ifndef SnowScene_hpp
#define SnowScene_hpp

#include "Snow.hpp"
#include "SnowfallGenerator.hpp"
#include "SnowFall.hpp"
#include "SnowAccum.hpp"
#include <atlas/utils/Scene.hpp>

class SnowScene : public atlas::utils::Scene
{
	public:
		SnowScene();
		~SnowScene();
	
		void renderScene() override;
		void updateScene(double time) override;

		void mousePressEvent(int button, int action, int modifiers, double xPos, double yPos) override;		
		void mouseMoveEvent(double xPos, double yPos) override;
        void mouseScrollEvent(double xOffset, double yOffset) override;
		void keyPressEvent(int key, int scancode, int action, int mods) override;
		void screenResizeEvent(int width, int height) override;		
		void onSceneEnter() override;
		void onSceneExit() override;

		glm::vec3 getCameraPosition() const;
		glm::vec3 getLightPosition() const;
		
		void addSnow(std::unique_ptr<Snow> snowflake);
		SnowFall const& getSnowFall() const;
		SnowAccum & getSnowAccum();
		glm::vec3 getForceWind();	
		
	private:
		glm::mat4 mProjection;
		float mWidth, mHeight;

		bool m_snowPause;

		SnowFall m_SnowFall;
		SnowAccum m_SnowAccum;

		float mTheta, mRow;

		glm::vec3 m_forceDir;

		glm::vec3 m_LightCoords;

};

#endif
