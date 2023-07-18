#pragma once

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <time.h>
#include <cmath>
#include <sstream>
#include <vector>
#include "ModelRenderer.h"
#include "Camera.h"
#include "Model.h"
#include "SkyboxRenderer.h"
#include "Terrain.h"
#include "Billboard.h"
#include "BillboardRenderer.h"
#include "FontRenderer.h"
#include "GUIRenderer.h"
#include "InstancedRenderer.h"
#include "Framebuffer.h"
#include "PostProcessGrey.h"

namespace GE {

	// Basic Game Engine class
	class GameEngine {
	public:
		GameEngine();			// Constructor

		virtual ~GameEngine();	// Destructor

		bool init(bool vSync, float _fogVal);	// Object initialisation
		bool keep_running();	// Indicates if user has closed window and, hence, game
		void processInput();	// Process key changes
		void update();			// Update game logic
		void draw();			// Render frame
		void shutdown();		// Release objects created during the game

		// Extra methods
		void setwindowtitle(const char*);

		// checks camera is within map boundaries
		void checkPos();

		SDL_Rect* getTerrBounds()
		{
			return terrBounds;
		}

		float getCamSpeed()
		{
			return camMoveSpeed;
		}
		float getMouseSense()
		{
			return mouseSens;
		}

		void setCamSpeed(float x)
		{
			camMoveSpeed += x;
		}
		void setMouseSense(float x) 
		{
			mouseSens += x;
		}

		int x, y{};
		int w, h{};

	private:
		// Private data
		// SDL window object to represent the frame
		SDL_Window* window{};

		// SDL Gl context to represent OpenGL in the program
		SDL_GLContext glContext{};

		// Camera
		Camera* cam{};

		bool geRunning{};
		bool mouseControlCam{};
		bool showCtrlMenu{};
		bool showLightSettings{};
		bool greyscaleOn{};
		
		// Mouse/camera 
		float camMoveSpeed = 2.0f;
		float mouseSens = 0.4f;
		bool showMouseSens{};

		// Add code for distance/direction to look from camera
		glm::vec3 dist{};

		float modelDist{};

		// stores all model renderers
		std::vector<ModelRenderer*> modelRenderers{};
		std::vector<Model*> models{};
		std::vector<Texture*> textures{};
		std::vector<InstancedRenderer*>instancedRenderers{};

		// For ship
		Model* mShip{};
		Texture* texShip{};
		ModelRenderer* mrShip{};

		// For Ghost
		Model* mGhost{};
		Texture* texGhost{};
		ModelRenderer* mrGhost{};
		InstancedRenderer* irGhost{};
		std::vector<InstanceData> instancesGhost{};
		// for sine wave movement
		float time = 0.0f;

		// Tree
		Model* mTree{};
		Texture* texTree{};
		ModelRenderer* mrTree{};

		// Plant
		Model* mPlant{};
		Texture* texPlant{};
		ModelRenderer* mrPlant{};
		InstancedRenderer* irPlant{};
		std::vector<InstanceData> instancesPlant{};

		// Plant2
		Model* mPlant2{};
		Texture* texPlant2{};
		ModelRenderer* mrPlant2{};

		// Buildings1
		Model* mBuild1{};
		Texture* texBuild1{};
		ModelRenderer* mrBuild1{};

		// Helmet
		Model* mHelmet{};
		Texture* texHelmet{};
		ModelRenderer* mrHelmet{};
		Model* mSpark{};
		ModelRenderer* mrSpark{};
		InstancedRenderer* irSparks{};
		std::vector<InstanceData> instancesSparks{};
		Texture* texSpark{};

		// Terrain
		Terrain* terrain{};
		Texture* terrainTex{};
		ModelRenderer* mrTerr{};
		SDL_Rect* terrBounds{};

		// Billboard objects
		Texture* billTex{};
		Billboard* bill{};
		Billboard* bill2{};
		BillboardRenderer* billRend{};

		// Skybox
		SkyboxRenderer* skybox{};

		// Framebuffer
		FrameBuffer* fb{};

		/*Model* mQuad{};
		ModelRenderer* mrQuad{};*/

		// Post processing
		PostProcessGreyscale* ppGrey{};

		// GUI
		// GUI renderer object
		GUIRenderer* gr{};

		// FPSMsg text object
		GUIText* FPSMsg{};

		// Controls option text object
		GUIText* controlsToggleMsg{};
		GUIImage* controlsMenuMsg{};

		// Mouse sensitivity
		Texture* texMouseIncrease{};
		Texture* texMouseDecrease{};
		GUIText* mouseSensMsg{};
		GUIImage* mouseSensUp{};
		GUIImage* mouseSensDown{};

		Texture* texCamIncrease{};
		Texture* texCamDecrease{};
		GUIText* camSpeedMsg{};
		GUIImage* camSpeedUp{};
		GUIImage* camSpeedDown{};

		// Minimap objects
		GUIImage* miniMapImg{};
		GUIImage* miniMapPlayerImg{};
		GUIImage* miniMapBuildingImg{};

		// HUD image
		GUIImage* cockpitImg{};

		// Light colour bar
		GUIImage* lightColourMsg{};
		GUIText* lightColourValuesMsg{};
		GUIImage* lightRedUp{};
		GUIImage* lightRedDown{};
		GUIImage* lightGreenUp{};
		GUIImage* lightGreenDown{};
		GUIImage* lightBlueUp{};
		GUIImage* lightBlueDown{};

		// FPS info
		Uint32 last_time{};
		Uint32 current_time{};
		Uint32 frame_count{};
	};

	// Helper function
	void display_info_message(const char*);

}




