#ifndef MENU_STATE_H
#define MENU_STATE_H
#include "GameState.h"
#include "GameStateManager.h"
#include "ShaderManager.h"
#include "PlayState.h"
#include "UIHelper.h"
#include "InputManager.h"
#include "SelectionState.h"
#include "AudioManager.h"

class MenuState : public GameState
{
public:
    void Init() override;
    void Update(float dt) override;
    void RenderGame() override;
    void RenderUI() override;
    void Exit() override;

private:
    UI_QUAD background;
    UI_QUAD racecast_logo;
    UI_QUAD play_button;

    bool PlayEntryAnimation = true;
    void EntryAnimation(float dt);

    float animation_elapse = 0.f;

    bool PlayExitAnimation = false;
    void ExitAnimation(float dt);

    void Blink(float dt);
    float blink_elapsed = 0.f;
    bool blink = true;
};

#endif