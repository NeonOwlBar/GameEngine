#define SDL_MAIN_HANDLED
#include "GameEngine.h"
#include <iostream>

using namespace GE;

// Preprocessor directive to only have console appear when in debug config
// 
// To undo this, only use int main() (remove int WinMain(), along with preprocessor directives), and in project properties do the following:
//      - in Release config
//      - Linker > System > Subsystem
//      - set Subsystem to Console
// 
//      - This is set to Windows inside Release config to remove console.

// Program starts
#ifdef _DEBUG
int main(int argc, char *argv[])
#else
int WinMain(int argc, char* argv[])
#endif // DEBUG
{
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
