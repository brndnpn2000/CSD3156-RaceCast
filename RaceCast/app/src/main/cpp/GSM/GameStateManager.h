#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include "GameState.h"
#include <GLES3/gl31.h>
#include "BatchRenderer.h"
class GameStateManager
{
public:
    static GameStateManager& GetInstance();

    void ChangeState(GameState* next);

    void Update(float dt);

    void Render();

private:
    GameState* current_state = nullptr;
    GameState* next_state = nullptr;
};

#define GSM GameStateManager::GetInstance()
#endif