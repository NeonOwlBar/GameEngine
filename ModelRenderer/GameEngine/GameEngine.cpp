#include "GameEngine.h"
#include <iostream>

namespace GE {

	GameEngine::GameEngine() {
		x = 50;
		y = 50;
		w = 640;
		h = 480;
	}

	GameEngine::~GameEngine()
	{
	}

	bool GameEngine::init(bool vSyncStatus, float fogVal) {
		// Initialise SDL video subsystem
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cerr << "Unable to initialise SDL!  SDL error: " << SDL_GetError() << std::endl;

			return false;
		}

		geRunning = true;
		mouseControlCam = true;
		showCtrlMenu = false;
		showLightSettings = false;
		greyscaleOn = false;

		// Hides cursor when window is active
		SDL_ShowCursor(SDL_DISABLE);

		// Set the OpenGL version for the program
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		// Set the type of profile which is core meaning modern OpenGL
		// means no legacy features for backwards compatibility
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

		// Create the window and frame features
		// In this case window has a fixed size and prepares window for OpenGL to render into
		window = SDL_CreateWindow("Game Engine Architecture", x, y, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

		// Check window was created
		if (window == nullptr) {
			// Display an error if window wasn't created
			// Could log the errors to a file or create more robust error handling
			// e.g. give specific errors a number
			std::cerr << "Unable to create window! SDL error: " << SDL_GetError() << std::endl;

			// Return failure so caller can work out what to do
			return false;
		}

		// Creates the OpenGL context and links it to the window object
		// Context represents OpenGL for program such as objects and drawing
		glContext = SDL_GL_CreateContext(window);

		// Check context object was created
		if (glContext == nullptr) {
			// Display error and return failure
			std::cerr << "SDL could not create GL context! SDL error: " << SDL_GetError() << std::endl;
			return false;
		}

		// Initialise GLEW.  GLEW solves a problem with OpenGL on windows
		// as default API supported by Windows is 1.1 (from 1997)
		// GLEW binds latest extensions that can be used
		GLenum status = glewInit();

		// Check GLEW initialised ok
		if (status != GLEW_OK) {
			std::cerr << "Error initialising GLEW! Error: " << glewGetErrorString(status) << std::endl;

			return false;
		}

		// V Sync
		if (SDL_GL_SetSwapInterval(vSyncStatus) != 0) {
			std::cerr << "Warning: unable to set VSync! Error: " << SDL_GetError() << std::endl;

			return false;
		}

		dist = glm::vec3(0.0f, 0.0f, -100.0f);

		// Create the camera object
		cam = new Camera(glm::vec3(20.0f, 20.0f, 100.0f),	// Position
			glm::vec3(0.0f, 0.0f, 20.0f) + dist,	// Look at
			glm::vec3(0.0f, 1.0f, 0.0f),	// Up direction
			70.0f, float(w) / float(h), 0.1f, 1000.0f);	// fov, aspect ratio based on window dimensions, near and far clip planes


		skybox = new SkyboxRenderer("assets/skybox/skyrenderfront.bmp", "assets/skybox/skyrenderback.bmp",
									"assets/skybox/skyrenderright.bmp", "assets/skybox/skyrenderleft.bmp",
									"assets/skybox/skyrendertop.bmp", "assets/skybox/skyrenderbottom.bmp");


	// Frame Buffer
		fb = new FrameBuffer(w, h);
		fb->init();

		/*mQuad = new Model();

		bool resultQuad = mQuad->loadFromFile("assets/quad.obj");

		mrQuad = new ModelRenderer(fogVal);
		mrQuad->init();

		mrQuad->setPos(10.0f, 10.0f, -10.0f);
		mrQuad->setScale(10.f, 10.f, 10.f);
		mrQuad->setRotation(90.0f, 0.0f, 0.0f);

		mrQuad->setTexture(fb->getTexture());*/


	// Post Processing
		ppGrey = new PostProcessGreyscale(w, h);
		ppGrey->init();


	// Spaceship Model
		// Create the Model object
		mShip = new Model();
		models.push_back(mShip);

		// Load vertices from monkey.obj file
		bool result = mShip->loadFromFile("assets/space-shuttle/source/space-shuttle.obj");

		// Check it was loaded successfully
		if (!result)
		{
			std::cerr << "Failed to load model: " << mShip->getFileName() << std::endl;
		}

		texShip = new Texture("assets/space-shuttle/textures/ShuttleDiffuseMap_.jpg");
		
		// Create a ModelRenderer passing the Model object just loaded
		mrShip = new ModelRenderer(fogVal);
		modelRenderers.push_back(mrShip);
		// Initialise the ModelRenderer, // reduce the scale
		// move it away from camera
		mrShip->init();
		mrShip->setPos(150.0f, 100.f, 0.0f);
		mrShip->setRotation(0, 5.0f, 20.0f);
		mrShip->setScale(0.5f, 0.5f, 0.5f);
		mrShip->setTexture(texShip);


	// Ghost Model
		// Create the Model object
		mGhost = new Model();
		models.push_back(mGhost);

		// Load vertices from monkey.obj file
		bool resultGhost = mGhost->loadFromFile("assets/destiny-spectre/source/Spectre.obj");

		// Check it was loaded successfully
		if (!resultGhost)
		{
			std::cerr << "Failed to load model: " << mGhost->getFileName() << std::endl;
		}

		texGhost = new Texture("assets/destiny-spectre/source/Spectre_diff.jpg");

		// Create a ModelRenderer passing the Model object just loaded
		mrGhost = new ModelRenderer(fogVal);
		modelRenderers.push_back(mrGhost);

		// Initialise the ModelRenderer, // reduce the scale
		// move it away from camera
		mrGhost->init();
		mrGhost->setPos(-70.0, 30.0f, -50.0f);
		mrGhost->setRotation(0.0f, 180.0f, 0.0f);
		mrGhost->setScale(0.05f, 0.05f, 0.05f);
		mrGhost->setTexture(texGhost);

		irGhost = new InstancedRenderer();
		irGhost->init();

		// Create a vector to storer the locations, rotations and scales of the isntance
		// std::vector<InstancePosRotScale> instancesGhost;

		// Going to create a fleet of ships so need to create a vector of positions
		// Scale and rotation will be the same. Z position based on the loop count
		// variable where first model will be at z0.0f, enxt -10.0f, -20.0f and so on
		const int num_ghosts = 5;
		float xVal = 0;	// offset for random x value
		float zVal = 0;	// offset for random z value
		for (int count = 0; count < num_ghosts; count++) {
			if ((count % 2) == 0) {
				zVal = 15.0f;
			}
			else {
				zVal = 50.0f;
			}
			instancesGhost.push_back(InstanceData{ 15 + count * 23.0f, float(rand() % 20 + 30), 10.0f + zVal,	//	posX, Y, Z 
													0.0f, float(rand() % 180 + 90), 0.0f,						// rotX, Y, Z
													0.05f, 0.05f, 0.05f,										// scaleX, Y, Z
													float(rand() % 5 + 1),										// speed
													0.0f														// time not needed for each instance,
				});																								// can be GameEngine member variable

		}
		irGhost->setInstanceData(instancesGhost);


	// Tree
		mTree = new Model();
		models.push_back(mTree);

		bool resultTree = mTree->loadFromFile("assets/bioluminescent-tree/source/Tree/BioTree.fbx");

		if (!resultTree)
		{
			std::cerr << "Failed to load model: " << mTree->getFileName() << std::endl;
		}

		texTree = new Texture("assets/bioluminescent-tree/source/Tree/Tree_lambert1_BaseColor.png");
		mrTree = new ModelRenderer(fogVal);
		modelRenderers.push_back(mrTree);

		mrTree->init();
		mrTree->setPos(150.f, 3.f, 76.f);
		mrTree->setRotation(0.0f, 0.0f, 0.0f);
		mrTree->setScale(2.f, 4.f, 2.f);
		mrTree->setTexture(texTree);


	// Plant
		mPlant = new Model();
		models.push_back(mPlant);

		bool resultPlant = mPlant->loadFromFile("assets/mutant-tree-1/source/plant/Obstaculo_001.fbx");

		if (!resultPlant)
		{
			std::cerr << "Failed to load model: " << mPlant->getFileName() << std::endl;
		}

		texPlant = new Texture("assets/mutant-tree-1/source/plant/Texture/plant_lambert3SG_AlbedoAlpha.png");
		mrPlant = new ModelRenderer(fogVal);
		modelRenderers.push_back(mrPlant);

		mrPlant->init();
		mrPlant->setPos(72.f, 3.5f, 140.f);
		mrPlant->setRotation(0.0f, 0.0f, 0.0f);
		mrPlant->setScale(5.f, 5.f, 5.f);
		mrPlant->setTexture(texPlant);

		irPlant = new InstancedRenderer();
		irPlant->init();

		const int num_plants = 10;
		xVal = 0;	// offset for random x value 
		float scale = 0;	// random scale for plants
		for (int count = 0; count < num_plants; count++) {
			scale = rand() % 2 + 1;
			if ((count % 2) == 0) {
				xVal = 230.0f;
			}
			else {
				xVal = 190.0f;
			}
			instancesPlant.push_back(InstanceData{ xVal + (rand() % 40), 5.0f, 10.0f + (count * (rand() % 5 + 20)),		//	posX, Y, Z 
													0.0f, 0.0f, 0.0f,													// rotX, Y, Z
													scale * 5.f, scale * 5.f, scale * 5.f,								// scaleX, Y, Z
													0.0f,																// no speed value required
													0.0f																// no time value required
				});	
		}
		irPlant->setInstanceData(instancesPlant);

	// Plant2
		mPlant2 = new Model();
		models.push_back(mPlant2);

		bool resultPlant2 = mPlant2->loadFromFile("assets/mutant-tree-2/source/plant/Obstaculo_002.fbx");

		if (!resultPlant2)
		{
			std::cerr << "Failed to load model: " << mPlant->getFileName() << std::endl;
		}

		texPlant2 = new Texture("assets/mutant-tree-2/source/plant/Texture/plant_lambert3SG_AlbedoAlpha.png");
		mrPlant2 = new ModelRenderer(fogVal);
		modelRenderers.push_back(mrPlant2);

		mrPlant2->init();
		mrPlant2->setPos(186.f, 1.3f, 144.f);
		mrPlant2->setRotation(0.0f, 0.0f, 0.0f);
		mrPlant2->setScale(5.f, 5.f, 5.f);
		mrPlant2->setTexture(texPlant2);


	// Buildings1
		mBuild1 = new Model();
		models.push_back(mBuild1);

		bool resultBuild1 = mBuild1->loadFromFile("assets/low-poly-sci-fi-buildings/source/OfficeBuilding/OfficeBuilding.obj");
		if (!resultBuild1)
			std::cerr << "Failed to load model: " << mBuild1->getFileName() << std::endl;

		texBuild1 = new Texture("assets/low-poly-sci-fi-buildings/source/OfficeBuilding/OfficeBuilding.png");
		mrBuild1 = new ModelRenderer(fogVal);
		modelRenderers.push_back(mrBuild1);

		mrBuild1->init();
		mrBuild1->setPos(90.f, 0.f, 180);
		mrBuild1->setRotation(0.0f, 0.0f, 0.0f);
		mrBuild1->setScale(18.f, 18.f, 18.f);
		mrBuild1->setTexture(texBuild1);

	// Helmet
		mHelmet = new Model();
		models.push_back(mHelmet);

		bool resultHelmet = mHelmet->loadFromFile("assets/sci-fi-helmet/source/helmet_low/helmet_low.obj");

		if (!resultHelmet)
			std::cerr << "Could not load model: " << mHelmet->getFileName() << std::endl;

		texHelmet = new Texture("assets/sci-fi-helmet/source/helmet_low/helmet_Base_Color.jpg");
		mrHelmet = new ModelRenderer(fogVal);
		modelRenderers.push_back(mrHelmet);

		mrHelmet->init();
		mrHelmet->setPos(50.f, 6.f, 85.f);
		mrHelmet->setRotation(0.0f, 0.0f, 0.0f);
		mrHelmet->setScale(2.f, 2.f, 2.f);
		mrHelmet->setTexture(texHelmet);

		irSparks = new InstancedRenderer();
		irSparks->init();

		mSpark = new Model();
		bool resultQuad = mSpark->loadFromFile("assets/cube.obj");
		texSpark = new Texture("assets/spark.png");
		mrSpark = new ModelRenderer(fogVal);
		mrSpark->init();
		mrSpark->setPos(mrHelmet->getPosX(), mrHelmet->getPosY() + 4, mrHelmet->getPosZ());
		mrSpark->setScale(1.f, 1.f, 1.f);
		mrSpark->setRotation(0.0f, 0.0f, 0.0f);
		mrSpark->setTexture(texSpark);

		const int num_sparks = 15;
		for (int count = 0; count < num_sparks; count++) {
			
			instancesSparks.push_back(InstanceData{ mrSpark->getPosX(), mrSpark->getPosY(), mrSpark->getPosZ(),		// posX, Y, Z 
													float(rand() % 180), float(rand() % 360), float(rand() % 160 - 80),		// rotX, Y, Z
													scale * 0.08f, scale * 0.1f, scale * 0.08f,								// scaleX, Y, Z
													float(rand() % 500 + 100)/100,											// speed value will act as lifespan limit
													0.0f																	// time to track lifespan
				});
		}
		irSparks->setInstanceData(instancesSparks);


	// Terrain
		terrain = new Terrain("assets/terrain/moonHeightmap.png");
		terrainTex = new Texture("assets/terrain/moonTexture.jpg");

		mrTerr = new ModelRenderer(fogVal);
		modelRenderers.push_back(mrTerr);
		terrBounds = new SDL_Rect{ 0, 0, 300, 300 };
		mrTerr->init();
		mrTerr->setPos(terrBounds->x, 0, terrBounds->y);
		mrTerr->setScale(terrBounds->w / 100, 1.0f, terrBounds->h / 100);
		//mrTerr->setTexture(terrainTex);
		mrTerr->setTexture(fb->getTexture());
		

	// Billboards
		// Load a texture for the billboard
		billTex = new Texture("assets/billboards/tree.png");
		
		// By default, billboard will be at 0, 0, 0
		// in front of the camera
		bill = new Billboard(billTex);
		bill2 = new Billboard(billTex);

		// Scale billboard as only 1.0x1.0
		bill->setScaleX(15.0f);
		bill->setScaleY(30.0f);

		bill2->setScaleX(15.0f);
		bill2->setScaleY(30.0f);

		// Set its position to in front of model
		bill->setX(mrPlant->getPosX());// * 0.05);
		bill->setY(mrPlant->getPosY());// * 0.05);
		bill->setZ(mrPlant->getPosZ() + 2);// * 0.05);

		bill2->setX(mrPlant->getPosX() + 5);// * 0.05);
		bill2->setY(mrPlant->getPosY());// * 0.05);
		bill2->setZ(mrPlant->getPosZ());// * 0.05);

		// Create the billboard renderer object
		billRend = new BillboardRenderer(fogVal);

		// Init the renderer
		billRend->init();


	// GUI
		// Create the GUIRenderer object
		gr = new GUIRenderer();

		gr->init(w, h);

		// Create a text object for the FPS
		// Third parameter is initial text to be replaced in update()
		FPSMsg = new GUIText(w * 0.7, 0, w * 0.25, h * 0.1, "FPS: 0", "assets/font/open-sans/OpenSans-Regular.ttf");

		controlsToggleMsg = new GUIText(w * 0.05, h * 0.9, w * 0.3, h * 0.1, "Press C for controls", "assets/font/open-sans/OpenSans-Regular.ttf");

		Texture* texControls = new Texture("assets/gui/controls.png");
		if (texControls == NULL)
			std::cout << "Could not load controls image." << std::endl;
		controlsMenuMsg = new GUIImage(0.0f, 0.0f, w, h, texControls);
		

		// Load a texture for the "minimap" and create a GUIImage object for texture
		Texture* miniMapTex = new Texture("assets/gui/minimapTexture.jpg");
		// Likewise, load a texture for the player and create a GUIImage for it
		miniMapImg = new GUIImage(0.0f, 0.0f, w * 0.2f, h * 0.2667, miniMapTex);

		Texture* miniMapPlayerTex = new Texture("assets/gui/playerMinimap.png");
		miniMapPlayerImg = new GUIImage(0.0f - (miniMapPlayerTex->getWidth() / 2), 0.0f - (miniMapPlayerTex->getHeight() / 2), w * 0.025f, h * 0.033f, miniMapPlayerTex);

		Texture* miniMapBuildingTex = new Texture("assets/gui/buildingTopView.png");
		// positions are worked out in 3D space relative to the terrain, 
		// then multipled by the size of 2D minimap to be placed accurately.
		miniMapBuildingImg = new GUIImage(miniMapImg->getWidth() * (mrBuild1->getPosX() / terrBounds->w), 
			miniMapImg->getHeight() * (mrBuild1->getPosZ() / terrBounds->h), 
			miniMapImg->getWidth() * (4 * mrBuild1->getScaleX() / terrBounds->w), 
			miniMapImg->getHeight() * (5 * mrBuild1->getScaleZ() / terrBounds->h),
			miniMapBuildingTex);


	// Light Settings
		Texture* texLightColour = new Texture("assets/gui/guiLightColours.png");
		lightColourMsg = new GUIImage(w * 0.65, h * 0.15, w * 0.2, h * 0.1, texLightColour);
		lightColourValuesMsg = new GUIText(w * 0.65, h * 0.25, w * 0.3, h * 0.1, "Colour: 255, 255, 255", "assets/font/open-sans/OpenSans-Regular.ttf");

		Texture* texRedUp = new Texture("assets/gui/guiIncreaseRed.png");
		lightRedUp = new GUIImage(w * 0.75, h * 0.40, w * 0.05, h * 0.05, texRedUp);
		Texture* texRedDown = new Texture("assets/gui/guiDecreaseRed.png");
		lightRedDown = new GUIImage(w * 0.75, h * 0.48, w * 0.05, h * 0.05, texRedDown);

		Texture* texGreenUp = new Texture("assets/gui/guiIncreaseGreen.png");
		lightGreenUp = new GUIImage(w * 0.83, h * 0.40, w * 0.05, h * 0.05, texGreenUp);
		Texture* texGreenDown = new Texture("assets/gui/guiDecreaseGreen.png");
		lightGreenDown = new GUIImage(w * 0.83, h * 0.48, w * 0.05, h * 0.05, texGreenDown);

		Texture* texBlueUp = new Texture("assets/gui/guiIncreaseBlue.png");
		lightBlueUp = new GUIImage(w * 0.91, h * 0.40, w * 0.05, h * 0.05, texBlueUp);
		Texture* texBlueDown = new Texture("assets/gui/guiDecreaseBlue.png");
		lightBlueDown = new GUIImage(w * 0.91, h * 0.48, w * 0.05, h * 0.05, texBlueDown);


	// Mouse Settings
		texMouseIncrease = new Texture("assets/gui/guiIncreaseSign.png");
		texMouseDecrease = new Texture("assets/gui/guiDecreaseSign.png");
		mouseSensMsg = new GUIText(w * 0.5, h * 0.78, w * 0.25, h * 0.08, 
			"Mouse Settings", "assets/font/open-sans/OpenSans-Regular.ttf");
		mouseSensDown = new GUIImage(w * 0.8, h * 0.8, w * 0.05, h * 0.05, texMouseDecrease);
		mouseSensUp = new GUIImage(w * 0.9, h * 0.8, w * 0.05, h * 0.05, texMouseIncrease);

		// Separate textures to allow textures to be deleted successfully
		// Even though the textures use the same image, deleting a GUIImage* object 
		// deletes the associated texture. Then another GUIImage with the same texture can no longer 
		// be properly deleted, because the texture has already been deleted.
		texCamIncrease = new Texture("assets/gui/guiIncreaseSign.png");
		texCamDecrease = new Texture("assets/gui/guiDecreaseSign.png");
		camSpeedMsg = new GUIText(w * 0.5, h * 0.88, w * 0.25, h * 0.08, 
			"Camera Settings", "assets/font/open-sans/OpenSans-Regular.ttf");
		camSpeedDown = new GUIImage(w * 0.8, h * 0.9, w * 0.05, h * 0.05, texCamDecrease);
		camSpeedUp = new GUIImage(w * 0.9, h * 0.9, w * 0.05, h * 0.05, texCamIncrease);
		

	// General
		last_time = current_time = SDL_GetTicks();

		frame_count = 0;

		// Got this far then must have been successful setting up SDL and OpenGL
		return true;
	}

	// A game would call this function to check if the user closed the window
	// resulting in a SQL_QUIT message
	bool GameEngine::keep_running() {
		// Need to do this keep the event queue up to date
		SDL_PumpEvents();

		SDL_Event evt;

		// Check for quit event
		if (SDL_PeepEvents(&evt, 1, SDL_GETEVENT, SDL_QUIT, SDL_QUIT) || geRunning == false) {
			// If user quit program, then return false meaning
			// don't keep running
			return false;
		}

		// Got this far means no quit event so keep running
		return true;
	}

	void GameEngine::processInput()
	{
		// get current mouse x and y
		int mouse_x, mouse_y;
		SDL_GetMouseState(&mouse_x, &mouse_y);
		// Calculate the difference between old mouse and new mouse position
		// - note inversion on y as this flipped
		int diffX = mouse_x - (w / 2);
		int diffY = (h / 2) - mouse_y;

		if (mouseControlCam)
		{
			// Update the yaw and pitch based on the mouse differential
			cam->setYaw(cam->getYaw() + diffX * mouseSens);
			cam->setPitch(cam->getPitch() + diffY * mouseSens);

			// Set the mouse back to the centre
		/*#ifdef _WIN32
			// works well in Windows. _WIN32 includes x64 configuration by default
			SDL_WarpMouseGlobal(x + (w / 2), y + (h / 2));
		#else
			// works better in virtual machine, possibly in fully installed Linux too
			SDL_WarpMouseInWindow(window, w / 2, h / 2);
		#endif*/
			SDL_WarpMouseInWindow(window, w / 2, h / 2);
		}
		

		// Calculate new camera facing direction from the change to yaw and pitch
		// For more info about this, see the Euler Angles section here:
		// https://learnopengl.com/Getting-started/Camera
		glm::vec3 direction;
		direction.x = cos(glm::radians(cam->getYaw())) * cos(glm::radians(cam->getPitch()));
		direction.y = sin(glm::radians(cam->getPitch()));
		direction.z = sin(glm::radians(cam->getYaw())) * cos(glm::radians(cam->getPitch()));
		cam->setTarget(glm::normalize(direction));

		// Array to hold states of key presses we're interested in
		bool keyStates[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		// indices to the above array to store respectiive key presses
		int UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3, QUIT = 4, MSENSUP = 5, 
			MSENSDOWN = 6, CAMSPDUP = 7, CAMSPDDOWN = 8, RUP = 9, RDN = 10, 
			GUP = 11, GDN = 12, BUP = 13, BDN = 14, GREY = 15;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			// If there is a key pressed, check if it is UP, DOWN, LEFT, or RIGHT
			// Mark the array to indicate this key is held down
			if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.scancode)
				{
					case SDL_SCANCODE_W:
						keyStates[UP] = true;
						break;
					case SDL_SCANCODE_S:
						keyStates[DOWN] = true;
						break;
					case SDL_SCANCODE_A:
						keyStates[LEFT] = true;
						break;
					case SDL_SCANCODE_D:
						keyStates[RIGHT] = true;
						break;
					case SDL_SCANCODE_ESCAPE:
					case SDL_SCANCODE_Q:
						keyStates[QUIT] = true;
						break;
					case SDL_SCANCODE_F:
						if (!greyscaleOn) {
							greyscaleOn = true;
						}
						else {
							greyscaleOn = false;
						}
						break;
					case SDL_SCANCODE_M:
						if (mouseControlCam) {
							mouseControlCam = false;
							SDL_ShowCursor(SDL_ENABLE);
						}
						else {
							SDL_ShowCursor(SDL_DISABLE);
						/*#ifdef _WIN32
							SDL_WarpMouseGlobal(x + (w / 2), y + (h / 2));
						#else
							SDL_WarpMouseInWindow(window, w / 2, h / 2);
						#endif*/
							SDL_WarpMouseInWindow(window, w / 2, h / 2);
							mouseControlCam = true;
						}
						break;
					case SDL_SCANCODE_C:
						if (showCtrlMenu) {
							showCtrlMenu = false;
						}
						else {
							showCtrlMenu = true;
						}
						break;
					case SDL_SCANCODE_E:	// brings up mouse settings menu
						if (!showMouseSens) {
							showMouseSens = true;
						}
						else {
							showMouseSens = false;
						}
						break;
					case SDL_SCANCODE_L:
						if (showLightSettings) {
							showLightSettings = false;
						}
						else {
							showLightSettings = true;
						}
						break;
					case SDL_SCANCODE_UP:
						if (showMouseSens) {
						keyStates[MSENSUP] = true;
						}
						break;
					case SDL_SCANCODE_DOWN:
						if (showMouseSens) {
						keyStates[MSENSDOWN] = true;
						}
						break;
					case SDL_SCANCODE_RIGHT:
						if (showMouseSens) {
						keyStates[CAMSPDUP] = true;
						}
						break;
					case SDL_SCANCODE_LEFT:
						if (showMouseSens) {
						keyStates[CAMSPDDOWN] = true;
						}
						break;
				}
			}
			// If there has been a key lifted, check if it is UP, DOWN, LEFT, OR RIGHT
			// Mark the array to indicate this key is no longer pressed
			if (e.type == SDL_KEYUP)
			{
				switch (e.key.keysym.scancode)
				{
				case SDL_SCANCODE_W:
					keyStates[UP] = false;
					break;
				case SDL_SCANCODE_S:
					keyStates[DOWN] = false;
					break;
				case SDL_SCANCODE_A:
					keyStates[LEFT] = false;
					break;
				case SDL_SCANCODE_D:
					keyStates[RIGHT] = false;
					break;
				case SDL_SCANCODE_ESCAPE:
				case SDL_SCANCODE_Q:
					keyStates[QUIT] = false;
					break;
				}
			}
			if (e.type == SDL_MOUSEBUTTONDOWN) 
			{
				// checks mouse isn't controlling camera, and left mouse button clicked
				if (!mouseControlCam && SDL_GetMouseState(&mouse_x, &mouse_y) && SDL_BUTTON_LMASK) {
					// mouse settings buttons
					if (mouse_x >= mouseSensUp->getX() && mouse_x <= (mouseSensUp->getX() + mouseSensUp->getWidth())
						&& mouse_y >= mouseSensUp->getY() && mouse_y <= (mouseSensUp->getY() + mouseSensUp->getHeight()))
					{
						keyStates[MSENSUP] = true;
					}
					if (mouse_x >= mouseSensDown->getX() && mouse_x <= (mouseSensDown->getX() + mouseSensDown->getWidth())
						&& mouse_y >= mouseSensDown->getY() && mouse_y <= (mouseSensDown->getY() + mouseSensDown->getHeight()))
					{
						keyStates[MSENSDOWN] = true;
					}

					if (mouse_x >= camSpeedUp->getX() && mouse_x <= (camSpeedUp->getX() + camSpeedUp->getWidth())
						&& mouse_y >= camSpeedUp->getY() && mouse_y <= (camSpeedUp->getY() + camSpeedUp->getHeight()))
					{
						keyStates[CAMSPDUP] = true;
					}
					if (mouse_x >= camSpeedDown->getX() && mouse_x <= (camSpeedDown->getX() + camSpeedDown->getWidth())
						&& mouse_y >= camSpeedDown->getY() && mouse_y <= (camSpeedDown->getY() + camSpeedDown->getHeight()))
					{
						keyStates[CAMSPDDOWN] = true;
					}

					// Lighting settings buttons
					if (mouse_x >= lightRedUp->getX() && mouse_x <= (lightRedUp->getX() + lightRedUp->getWidth())
						&& mouse_y >= lightRedUp->getY() && mouse_y <= (lightRedUp->getY() + lightRedUp->getHeight()))
					{
						keyStates[RUP] = true;
					}
					if (mouse_x >= lightRedDown->getX() && mouse_x <= (lightRedDown->getX() + lightRedDown->getWidth())
						&& mouse_y >= lightRedDown->getY() && mouse_y <= (lightRedDown->getY() + lightRedDown->getHeight()))
					{
						keyStates[RDN] = true;
					}

					if (mouse_x >= lightGreenUp->getX() && mouse_x <= (lightGreenUp->getX() + lightGreenUp->getWidth())
						&& mouse_y >= lightGreenUp->getY() && mouse_y <= (lightGreenUp->getY() + lightGreenUp->getHeight()))
					{
						keyStates[GUP] = true;
					}
					if (mouse_x >= lightGreenDown->getX() && mouse_x <= (lightGreenDown->getX() + lightGreenDown->getWidth())
						&& mouse_y >= lightGreenDown->getY() && mouse_y <= (lightGreenDown->getY() + lightGreenDown->getHeight()))
					{
						keyStates[GDN] = true;
					}

					if (mouse_x >= lightBlueUp->getX() && mouse_x <= (lightBlueUp->getX() + lightBlueUp->getWidth())
						&& mouse_y >= lightBlueUp->getY() && mouse_y <= (lightBlueUp->getY() + lightBlueUp->getHeight()))
					{
						keyStates[BUP] = true;
					}
					if (mouse_x >= lightBlueDown->getX() && mouse_x <= (lightBlueDown->getX() + lightBlueDown->getWidth())
						&& mouse_y >= lightBlueDown->getY() && mouse_y <= (lightBlueDown->getY() + lightBlueDown->getHeight()))
					{
						keyStates[BDN] = true;
					}
				}
			}
			if (e.type == SDL_MOUSEBUTTONUP)
			{
			}
		}

		// Handle camera movement based upon the relevant key press
		if (keyStates[UP])
		{
			cam->setPos(cam->getPos() + cam->getTarget() * camMoveSpeed);
		}
		if (keyStates[DOWN])
		{
			cam->setPos(cam->getPos() - cam->getTarget() * camMoveSpeed);
		}
		if (keyStates[LEFT])
		{
			// Take away the cross product of the facing and up vector from the camera position - strafe left
			cam->setPos(cam->getPos() - glm::normalize(glm::cross(cam->getTarget(), cam->getUpDir())) * camMoveSpeed);
		}
		if (keyStates[RIGHT])
		{
			// Take away the cross product of the facing and up vector from the camera position - strafe left
			cam->setPos(cam->getPos() + glm::normalize(glm::cross(cam->getTarget(), cam->getUpDir())) * camMoveSpeed);
		}
		if (keyStates[QUIT])
		{
			geRunning = false;
		}

	
		if (keyStates[MSENSUP]) {
			if (showMouseSens && mouseSens <= 2.0f) {
				setMouseSense(0.1f);
			}
			keyStates[MSENSUP] = false;
		}
		if (keyStates[MSENSDOWN]) {
			if (showMouseSens && mouseSens >= 0.2f) {
				setMouseSense(-0.1f);
			}
			keyStates[MSENSDOWN] = false;
		}
		if (keyStates[CAMSPDUP]) {
			if (showMouseSens && camMoveSpeed <= 7.8f) {
				setCamSpeed(0.2f);
			}
			keyStates[CAMSPDUP] = false;
		}
		if (keyStates[CAMSPDDOWN]) {
			if (showMouseSens && camMoveSpeed >= 0.2) {
				setCamSpeed(-0.2f);
			}
			keyStates[CAMSPDDOWN] = false;
		}


		// 5/255 = 0.0196 - can change values by 5 each press
		if (keyStates[RUP])
		{
			if (mrShip->getLightColourR() < 1.0f) {
				for (auto& mr : modelRenderers) {
					mr->setLightColour(mr->getLightColourR() + 0.0196f, mr->getLightColourG(), mr->getLightColourB());
				}
			}
			keyStates[RUP] = false;
		}
		if (keyStates[RDN])
		{
			if (mrShip->getLightColourR() > 0.0196f) {
				for (auto& mr : modelRenderers) {
					mr->setLightColour(mr->getLightColourR() - 0.0196f, mr->getLightColourG(), mr->getLightColourB());
				}
			}
			keyStates[RDN] = false;
		}

		if (keyStates[GUP])
		{
			if (mrShip->getLightColourG() < 1.0f) {
				for (auto& mr : modelRenderers) {
					mr->setLightColour(mr->getLightColourR(), mr->getLightColourG() + 0.0196f, mr->getLightColourB());
				}
			}
			keyStates[GUP] = false;
		}
		if (keyStates[GDN])
		{
			if (mrShip->getLightColourG() > 0.0196f) {
				for (auto& mr : modelRenderers) {
					mr->setLightColour(mr->getLightColourR(), mr->getLightColourG() - 0.0196f, mr->getLightColourB());
				}
			}
			keyStates[GDN] = false;
		}

		if (keyStates[BUP])
		{
			if (mrShip->getLightColourB() < 1.0f) {
				for (auto& mr : modelRenderers) {
					mr->setLightColour(mr->getLightColourR(), mr->getLightColourG(), mr->getLightColourB() + 0.0196f);
				}
			}
			keyStates[BUP] = false;
		}
		if (keyStates[BDN])
		{
			if (mrShip->getLightColourB() > 0.0196f) {
				for (auto& mr : modelRenderers) {
					mr->setLightColour(mr->getLightColourR(), mr->getLightColourG(), mr->getLightColourB() - 0.0196f);
				}
			}
			keyStates[BDN] = false;
		}

		checkPos();
		//cam->updateCamMatrices();
	}

	// Update method which updates the game logic
	void GameEngine::update() 
	{
		// Rotates shuttle as moving around scene
		mrShip->setRotation(0.0f, mrShip->getRotY() - 0.3f, 0.0f);
		
		time += 0.01f;
		// ghost moves up in down in motion of a sine wave
		mrGhost->setPos(mrGhost->getPosX(), mrGhost->getPosY() + 0.1 * sin(2 * time), mrGhost->getPosZ());

		// camera's x and z affect mini-map player's x and y respectively
		miniMapPlayerImg->setX(cam->getPos().x * 0.4);
		miniMapPlayerImg->setY(cam->getPos().z * 0.4);

		current_time = SDL_GetTicks();

		if (current_time - last_time > 1000)
		{
			// create a variable to store a string
			std::ostringstream msgFrames;

			// create a message based on the value in frame_count
			msgFrames << "FPS = " << frame_count;

			// Set the GUI text to the new frame count
			FPSMsg->setText(msgFrames.str());

			// Reset for next frame
			frame_count = 0;

			last_time = current_time;

			//std::cout << cam->getPosX() << ", " << cam->getPosY() << ", " << cam->getPosZ() << std::endl;
		}

		frame_count++;

		// checkPos();
	}

	// Draw method.  Will modify this to render more complex scenes
	// For now clears the background
	void GameEngine::draw() {
	// Beginning of frame buffer
		if (greyscaleOn) {
			fb->beginRender();
		}

		glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Clear frame buffer and depth buffer for new frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render skybox. Needs to be first to avoid disabling depth testing for all objects
		skybox->draw(cam);

		// Render the model, using the model renderer
		mrShip->setTexture(texShip);
		mrShip->draw(cam, mShip);

		mrGhost->setTexture(texGhost);
		mrGhost->draw(cam, mGhost);

		mrTree->setTexture(texTree);
		mrTree->draw(cam, mTree);

		mrPlant2->setTexture(texPlant2);
		mrPlant2->draw(cam, mPlant2);

		mrBuild1->setTexture(texBuild1);
		mrBuild1->draw(cam, mBuild1);

		mrHelmet->setTexture(texHelmet);
		mrHelmet->draw(cam, mHelmet);

		mrTerr->setTexture(terrainTex);
		mrTerr->drawTerrain(cam, terrain);

		for (auto& g : instancesGhost) {
			g.posY = g.posY + 0.1 * sin(g.speed * time);
		}
		// removes current data from vector to prevent memory leak
		irGhost->destroyInstancedData();
		irGhost->setInstanceData(instancesGhost);
		irGhost->drawInstanced(cam, mGhost, texGhost);

		for (auto& s : instancesSparks) {
			s.posY -= 0.1 * s.time * s.time;	// moves sparks down in binomial shape
			s.posX += 0.2 * sin(s.rotX);
			s.posZ += 0.2 * cos(s.rotZ);

			s.time += 0.01;
			if (s.time >= s.speed) {	// if spark's lifespan is over, reset timer and position
				s.posX = mrSpark->getPosX();
				s.posY = mrSpark->getPosY();
				s.posZ = mrSpark->getPosZ();
				s.time = 0.0f;
			}
		}
		irSparks->destroyInstancedData();
		irSparks->setInstanceData(instancesSparks);
		irSparks->drawInstanced(cam, mSpark, texSpark);

		irPlant->drawInstanced(cam, mPlant, texPlant);

		modelDist = glm::distance(cam->getPos(), glm::vec3(mrPlant->getPosX(), mrPlant->getPosY(), mrPlant->getPosZ()));
		if (modelDist < 150.0f)
		{
			mrPlant->setTexture(texPlant);
			mrPlant->draw(cam, mPlant);
		}
		else
		{
			billRend->draw(bill, cam);
			// billRend->draw(bill2, cam);
		}
		
		// Render GUI objects from back to front
		//gr->drawImage(cockpitImg);
		gr->drawImage(miniMapImg);
		gr->drawText(FPSMsg);
		gr->drawImage(miniMapBuildingImg);
		gr->drawImage(miniMapPlayerImg);

		gr->drawText(controlsToggleMsg);
		if (showCtrlMenu) {
			gr->drawImage(controlsMenuMsg);
		}

		if (showMouseSens) {
			// create a variable to store a string
			std::ostringstream msgMouseSens;
			std::ostringstream msgCamSpeed;

			// create a message based on the values
			msgMouseSens << "Mouse Sensitivity: " << mouseSens;
			msgCamSpeed << "Movement Speed: " << camMoveSpeed;

			// Set the GUI text to the new frame count
			mouseSensMsg->setText(msgMouseSens.str());
			gr->drawText(mouseSensMsg);
			gr->drawImage(mouseSensUp);
			gr->drawImage(mouseSensDown);

			camSpeedMsg->setText(msgCamSpeed.str());
			gr->drawText(camSpeedMsg);
			gr->drawImage(camSpeedUp);
			gr->drawImage(camSpeedDown);
		}

		if (showLightSettings) {
			std::ostringstream msgLightVals;

			// create a message based on the values
			msgLightVals << "Colour: " << round((mrShip->getLightColourR() * 255)) << ", " << round((mrShip->getLightColourG() * 255)) << ", " << round((mrShip->getLightColourB() * 255));

			// Set the GUI text to the new frame count
			lightColourValuesMsg->setText(msgLightVals.str());

			gr->drawImage(lightColourMsg);
			gr->drawText(lightColourValuesMsg);
			gr->drawImage(lightRedUp);
			gr->drawImage(lightRedDown);

			gr->drawImage(lightGreenUp);
			gr->drawImage(lightGreenDown);

			gr->drawImage(lightBlueUp);
			gr->drawImage(lightBlueDown);

		}
		
	// End section of frame buffer
		if (greyscaleOn) {
			fb->endRender();

			ppGrey->apply(fb->getTexture());

			glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
			// Enable depth test
			glEnable(GL_DEPTH_TEST);
			// Clear frame buffer and depth buffer for new frame
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//mrQuad->draw(cam, mQuad);
			ppGrey->renderToScreen();
		}

			SDL_GL_SwapWindow(window);
	}


	// Shutdown method closes OpenGL and SDL down
	// Will need to modify when we create additional objects
	void GameEngine::shutdown() 
	{
		SDL_ShowCursor(SDL_ENABLE);	// Shows cursor

		skybox->destroy();

		// Get the triangle object to release it's resources
		mrShip->destroy();
		mrGhost->destroy();
		mrTree->destroy();
		mrPlant->destroy();
		mrPlant2->destroy();
		mrBuild1->destroy();
		mrHelmet->destroy();
		mrTerr->destroy();

		delete skybox;

		delete mrShip;
		delete mShip;
		delete texShip;

		delete mGhost;
		delete mrGhost;
		delete texGhost;
		delete irGhost;

		delete mTree;
		delete mrTree;
		delete texTree;

		delete mPlant;
		delete mrPlant;
		delete texPlant;
		delete irPlant;
		delete mPlant2;
		delete mrPlant2;
		delete texPlant2;

		delete mBuild1;
		delete texBuild1;
		delete mrBuild1;

		delete mHelmet;
		delete texHelmet;
		delete mrHelmet;
		delete irSparks;

		delete terrain;
		delete mrTerr;
		delete terrainTex;

		delete bill;
		delete bill2;
		delete billRend;
		delete billTex;

		delete cam;

		delete FPSMsg;
		delete controlsToggleMsg;
		delete controlsMenuMsg;

		delete mouseSensMsg;
		delete mouseSensUp;
		delete mouseSensDown;

		delete camSpeedMsg;
		delete camSpeedUp;
		delete camSpeedDown;

		delete miniMapImg;
		delete miniMapPlayerImg;
		delete miniMapBuildingImg;
		delete lightColourMsg;

		delete cockpitImg;

		delete gr;


		SDL_DestroyWindow(window);

		window = nullptr;

		SDL_Quit();
	}

	// Simple helper method to set the text in window title bar
	void GameEngine::setwindowtitle(const char* new_title) {
		SDL_SetWindowTitle(window, new_title);
	}

	// Simple helper function to display program information
	// Note it is separate from the GameEngine class as it doesn't depend on it
	void display_info_message(const char* msg) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Basic Game Engine", msg, nullptr);
	}

	void GameEngine::checkPos()
	{
		if (cam->getPosX() <= terrBounds->x)
		{
			cam->setPosX(terrBounds->x);
		}
		else if (cam->getPosX() >= terrBounds->w)
		{
			cam->setPosX(terrBounds->w);
		}

		if (cam->getPosY() <= 5.f)
		{
			cam->setPosY(5.f);
		}

		if (cam->getPosZ() <= terrBounds->y)
		{
			cam->setPosZ(terrBounds->y);
		}
		else if (cam->getPosZ() >= terrBounds->h)
		{
			cam->setPosZ(terrBounds->h);
		}
		// Call to update the camera view matrix
		// Projection is also updated in case effects like zoom are used (e.g. right click to aim, shrink the fov a little)
		cam->updateCamMatrices();
	}
}