#include "Map1.h"

void Map1::Init()
{
    AssetManager::GetInstance().LoadTexture("image/pedals.png");
    AssetManager::GetInstance().LoadTexture("image/numbering.png");
    AssetManager::GetInstance().LoadTexture("image/map1_bg.png");
    AssetManager::GetInstance().LoadTexture("image/night_bg.png");
    AssetManager::GetInstance().LoadTexture("image/fender.png");
    AssetManager::GetInstance().LoadTexture("image/starting_light.png");
    AssetManager::GetInstance().LoadTexture("image/mainmenu_bg.png");


    start_light_timer = 0.f;
    race_timer = 0.f;
    end_timer = 0.f;
    aspect_ratio = (float)Globals::screen_size.first / (float)Globals::screen_size.second;

    environment.DebugAssetInit();
    environment =
            {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // Row 0 (Top Wall)
                    1, 0, 0, 0, 11, 0, 0, 0, 0, 1, // Row 1
                    1, 0, 0, 0, 11, 0, 0, 0, 0, 1, // Row 2
                    1, 0, 0, 1, 1, 1, 1, 0, 0, 1, // Row 3
                    1, 0, 0, 1, 0, 0, 1, 0, 0, 1, // Row 4
                    1, 0, 0, 1, 0, 0, 1, 0, 0, 1, // Row 5
                    1, 0, 0, 1, 1, 1, 1, 0, 0, 1, // Row 6
                    1, 0, 0, 0, 11, 0, 0, 0, 0, 1, // Row 7
                    1, 0, 0, 0, 11, 0, 0, 0, 0, 1, // Row 8
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1  // Row 9 (Bottom Wall)
            };
    CheckpointManager::GetInstance().Init<10,10>(environment);

    player.position = Vector2D(5.5,1.5);
    player.size = Vector2D(0.7,0.45);

    TextureCoordinate acc, rev;
    acc.GetBR()[0] = 0.5;
    acc.GetTR()[0] = 0.5;

    rev.GetBL()[0] = 0.5;
    rev.GetTL()[0] = 0.5;

    accelerator =   UI_QUAD(0.65,-0.5,0.3,0.6,"pedals.png",acc);
    reverse =       UI_QUAD(-0.65,-0.5,0.4,0.5,"pedals.png",rev);

    // lap counter
    font_height = 0.1f;
    font_width = font_height * aspect_ratio;

    total_lap_tc.GetBL()[0] = tc_offset * total_lap_int;
    total_lap_tc.GetTL()[0] = total_lap_tc.GetBL()[0];
    total_lap_tc.GetBR()[0] = tc_offset * (total_lap_int + 1);
    total_lap_tc.GetTR()[0] = total_lap_tc.GetBR()[0];
    total_lap = UI_QUAD(0.655,0.42,font_height,font_width,"numbering.png", total_lap_tc);

    slash_tc.GetBL()[0] = tc_offset * 11.f;
    slash_tc.GetTL()[0] = slash_tc.GetBL()[0];
    slash = UI_QUAD(0.6,0.5,font_height * 1.2f,font_width * 1.2f,"numbering.png", slash_tc);


    // End Screen
    end_screen_bg = UI_QUAD(0.f,0.f,2.f,2.f,"mainmenu_bg.png");
    retry_button = UI_QUAD(0.6,-0.5,font_height * 1.2f,font_width * 1.2f,"numbering.png", slash_tc);
    back_to_menu_button = UI_QUAD(-0.6,-0.5,font_height * 1.2f,font_width * 1.2f,"numbering.png", slash_tc);
}

void Map1::Update(float dt)
{
    // start timer
    start_light_timer += dt;
    if (start_light_timer < 3.f) // game starting
    {
        TextureCoordinate start_tc;
        float offset = 1.f / 4.f;
        start_tc.GetTL()[1] = 1.f - ((float)(int)start_light_timer * offset);
        start_tc.GetTR()[1] = start_tc.GetTL()[1];
        start_tc.GetBL()[1] = 1.f - (((float)(int)start_light_timer + 1.f) * offset);
        start_tc.GetBR()[1] = start_tc.GetBL()[1];
        start_light = UI_QUAD(0.f,0.5f,0.7f,0.55f,"starting_light.png", start_tc);
    }
    else if (CheckpointManager::GetInstance().isEnded() && end_timer > 1.0f) // game over
    {

        if (retry_button.Touched())
            Reset();

        if (back_to_menu_button.Touched())
            GSM.ChangeState(new MenuState);
    }
    else // game running
    {
        if (CheckpointManager::GetInstance().isEnded())
        {
            end_timer += dt;
        }
        else
        {
            race_timer += dt;
            // convert seconds -> minutes + seconds
            convertSecondsToMinutes((int)race_timer, race_minutes, race_seconds);
        }

        // input
        bool forward = accelerator.Hold();
        bool backward = reverse.Hold();

        // physics
        player.onUpdate(dt, forward, backward);

        // collision
        player.CollisionPhysics<10, 10>(environment, dt);
        player.SpeedRecalculate();
        if (environment.isACheckpoint(player.position.x, player.position.y))
        {
            CheckpointManager::GetInstance().AddLapToCP(player.position.x, player.position.y);
        }
    }

    // update rendering objects
        // current lap counter
    current_lap_int = CheckpointManager::GetInstance().GetLapCount();
    current_lap_tc.GetBL()[0] = tc_offset * (float)current_lap_int;
    current_lap_tc.GetTL()[0] = current_lap_tc.GetBL()[0];
    current_lap_tc.GetBR()[0] = tc_offset * (float)(current_lap_int + 1);
    current_lap_tc.GetTR()[0] = current_lap_tc.GetBR()[0];
    current_lap = UI_QUAD(0.545,0.61,font_height,font_width,"numbering.png", current_lap_tc);

        // background
    TextureCoordinate moving_bg_tc;
    float center = player.rotation / 360.f;
    moving_bg_tc.GetBL()[0] = center - 0.125f;
    moving_bg_tc.GetTL()[0] = moving_bg_tc.GetBL()[0];
    moving_bg_tc.GetBR()[0] = center + 0.125f;
    moving_bg_tc.GetTR()[0] = moving_bg_tc.GetBR()[0];
    background = UI_QUAD(0.f,0.5f,2.f,1.f,"night_bg.png",moving_bg_tc);
}

void Map1::RenderGame()
{
    background.DrawUI();
    player.RenderRayCast<10,10>(environment);

}

void Map1::RenderUI()
{
    if (!(CheckpointManager::GetInstance().isEnded() && end_timer > 1.0f)) // game running
    {
        environment.RenderMinimap(player.position.x, player.position.y);

        accelerator.DrawUI();
        reverse.DrawUI();
        current_lap.DrawUI();
        slash.DrawUI();
        total_lap.DrawUI();

        // render "traffic light"
        if (start_light_timer < 4.0f) start_light.DrawUI();
    }
    else // end screen comes out
    {
        end_screen_bg.DrawUI();
        retry_button.DrawUI();
        back_to_menu_button.DrawUI();
    }
}

void Map1::Exit()
{

}

void Map1::Reset()
{
    GSM.ChangeState(new Map1);
}

