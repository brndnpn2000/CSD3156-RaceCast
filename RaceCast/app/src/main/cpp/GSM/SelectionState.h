#ifndef SELECTIONSTATE_H
#define SELECTIONSTATE_H

#include "GameStateManager.h"
#include "GameState.h"
#include "MenuState.h"
#include "InputManager.h"
#include "UIHelper.h"
#include "Globals.h"
#include "Map1.h"

class SelectionState : public GameState
{
public:
    void Init() override;
    void Update(float dt) override;
    void RenderGame() override;
    void RenderUI() override;
    void Exit() override;

private:
    UI_QUAD background;
    UI_QUAD text;

    UI_QUAD map1;
    UI_QUAD map2;
    UI_QUAD map3;

    UI_QUAD play_button;
    UI_QUAD back_button;

    UI_QUAD select_frame;

    unsigned int selected_map = 1;

    // Beginning Animation
    void BeginningAnimation(float dt);
    float animation_elapsed = 0.f;
    bool PlayB_Animation = true;



    // Closing Animation

};


#endif
