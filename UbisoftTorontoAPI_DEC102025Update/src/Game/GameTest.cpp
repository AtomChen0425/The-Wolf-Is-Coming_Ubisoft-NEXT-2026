///////////////////////////////////////////////////////////////////////////////
// Filename: GameTest.cpp
// Provides a demo of how to use the API
///////////////////////////////////////////////////////////////////////////////

#if BUILD_PLATFORM_WINDOWS
//------------------------------------------------------------------------
#include <windows.h> 
#endif

#include <iostream>
#include <cassert>
//------------------------------------------------------------------------
#include <math.h> 
//------------------------------------------------------------------------
#include "../ContestAPI/app.h"
#include "../System/ECSSystem.h"
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Engine System instance
//------------------------------------------------------------------------
EngineSystem engineSystem;
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
	std::srand((unsigned)std::time(nullptr));
	engineSystem.ResetGame();
}

void Update(const float deltaTimeMs)
{
    // Update game system
	engineSystem.Update(deltaTimeMs);
}

void Render()
{
	engineSystem.Render();
}

void Shutdown()
{
	engineSystem.Shutdown();
}