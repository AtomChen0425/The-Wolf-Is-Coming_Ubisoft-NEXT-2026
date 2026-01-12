///////////////////////////////////////////////////////////////////////////////
// Filename: GameTest.cpp
// Provides a demo of how to use the API
///////////////////////////////////////////////////////////////////////////////

#if BUILD_PLATFORM_WINDOWS
//------------------------------------------------------------------------
#include <windows.h> 
#endif

#include <iostream>
//------------------------------------------------------------------------
#include <math.h> 
//------------------------------------------------------------------------
#include "../ContestAPI/app.h"
#include "../System/System.h"
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Example data....
//------------------------------------------------------------------------
CSimpleSprite* testSprite;
GameSystem gameSystem;
enum
{
	ANIM_FORWARDS,
	ANIM_BACKWARDS,
	ANIM_LEFT,
	ANIM_RIGHT,
};
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
	std::srand((unsigned)std::time(nullptr));
	gameSystem.ResetGame();
}

void Update(const float deltaTimeMs)
{
    // Update game system
    gameSystem.Update(deltaTimeMs);
}

void Render()
{
	gameSystem.Render();
}

void Shutdown()
{
	gameSystem.Shutdown();
}