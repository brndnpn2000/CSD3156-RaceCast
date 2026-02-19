#include "GameStateManager.h"

void GameStateManager::ChangeState(GameState* next)
{
    next_state = next;
}

void GameStateManager::Update(float dt)
{
    if (next_state)
    {
        if (current_state)
        {
            current_state->Exit();
            delete current_state;
        }

        current_state = next_state;
        current_state->Init();
        next_state = nullptr;
    }

    if (current_state)
    {
        current_state->Update(dt);
    }
}

void GameStateManager::Render()
{
    {
        if (current_state)
        {
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            current_state->RenderGame();
            BatchRenderer::GetInstance().BeginBatch();
            current_state->RenderUI();
            BatchRenderer::GetInstance().Flush();
        }
    }
}

GameStateManager &GameStateManager::GetInstance() {
    static GameStateManager instance;
    return instance;
}
