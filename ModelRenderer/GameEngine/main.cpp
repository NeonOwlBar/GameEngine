#define SDL_MAIN_HANDLED
#include "GameEngine.h"
#include <iostream>

using namespace GE;

// Program starts
int main(int argc, char *argv[]) {
	// Create a game engine object
	GameEngine ge;
	
	srand(time(NULL));

	ge.init(true, 0.0045f);
	
	// Main loop
	while (ge.keep_running()) {
		// Update game state
		ge.update();

		// Handle input
		ge.processInput();

		// Draw
		ge.draw();
	}

	// Destroy game engine
	ge.shutdown();

	return 0;
}
// Program ends
