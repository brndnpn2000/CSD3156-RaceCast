//
// Created by brand on 23/2/2026.
//

#ifndef RACECAST_MAP3_H
#define RACECAST_MAP3_H


#include "GameStateManager.h"
#include "GameState.h"
#include "AssetManager.h"
#include "BatchRenderer.h"
#include "Globals.h"
#include "GameObject.h"
#include "CheckpointManager.h"
#include "MenuState.h"
#include "FontAsset.h"   // ADD

class Map3 : public GameState
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

    //MAP NUMBER
    int mapNumber = 1;

    //SCORE
    bool score_saved = false;

    // TIMERS
    float start_light_timer;
    UI_QUAD start_light;
    float final_time;
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

    int current_lap_int = 1;
    int total_lap_int = 3;


    // OBJECTS;
    UI_QUAD background;
    Environment<10,10> environment;
    PlayerObject player;

    bool countdown_started = false;
};


#endif //RACECAST_MAP3_H
