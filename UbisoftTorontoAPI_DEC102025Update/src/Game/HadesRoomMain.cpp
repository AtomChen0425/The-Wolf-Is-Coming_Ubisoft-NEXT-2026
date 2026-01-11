#include "Game/HadesRoomGame.h"

static HadesRoomGame gGame;

void Init() { gGame.Init(); }
void Update(const float deltaTime) { gGame.Update(deltaTime); }
void Render() { gGame.Render(); }
void Shutdown() { gGame.Shutdown(); }