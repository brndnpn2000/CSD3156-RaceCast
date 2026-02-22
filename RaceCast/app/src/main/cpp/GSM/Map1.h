#ifndef MAP1_H
#define MAP1_H

#include "GameStateManager.h"
#include "GameState.h"
#include "AssetManager.h"
#include "BatchRenderer.h"
#include "Globals.h"
#include "GameObject.h"
#include "CheckpointManager.h"
#include "MenuState.h"

class Map1 : public GameState
{
public:
    void Init() override;
    void Update(float dt) override;
    void RenderGame() override;
    void RenderUI() override;
    void Exit() override;
    void Reset();

private:
    float aspect_ratio = 0.f; // x / y;

    // TIMERS
    float start_light_timer;
    UI_QUAD start_light;
    float race_timer;
    float end_timer;
    int race_minutes;
    int race_seconds;

    // INPUT
    UI_QUAD accelerator;
    UI_QUAD reverse;

    // END SCREEN MENU
    UI_QUAD end_screen_bg;
    UI_QUAD back_to_menu_button;
    UI_QUAD retry_button;

    // LAP COUNTER
    float font_height;
    float font_width;

    UI_QUAD current_lap;
    int current_lap_int = 1;
    UI_QUAD total_lap;
    int total_lap_int = 3;
    UI_QUAD slash;

    float tc_offset = 1.f / 12.f;
    TextureCoordinate total_lap_tc;
    TextureCoordinate current_lap_tc;
    TextureCoordinate slash_tc;

    // OBJECTS;
    UI_QUAD background;
    Environment<10,10> environment;
    PlayerObject player;

    bool countdown_started = false;
};


#endif