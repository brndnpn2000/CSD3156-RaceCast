#include <sstream>
#include <iomanip>

#include "Map1.h"
#include "HighScore.h"

void Map1::Init()
{
    AssetManager::GetInstance().LoadTexture("image/pedals.png");
    AssetManager::GetInstance().LoadTexture("image/end_screen_buttons.png");
    AssetManager::GetInstance().LoadTexture("image/map1_bg.png");
    AssetManager::GetInstance().LoadTexture("image/fender.png");
    AssetManager::GetInstance().LoadTexture("image/starting_light.png");
    AssetManager::GetInstance().LoadTexture("image/mainmenu_bg.png");

    AssetManager::GetInstance().LoadFont("font", "fonts/font.otf"); // ADD

    AUDIO.LoadAudio("audio/countdown.wav");
    AUDIO.LoadAudio("audio/gameMusic.mp3");
    AUDIO.LoadAudio("audio/accelerate.wav");

    AUDIO.PlayLoopingAudio("audio/gameMusic.mp3");
    AUDIO.UpdateAudioVolume("audio/gameMusic.mp3", 0.1f);
    HighScore::highScoreFont = AssetManager::GetInstance().GetFont("font");

    LOGI("MAP1 Start");
    start_light_timer = 0.f;
    final_time = 0.f;
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

    // End Screen
    end_screen_bg = UI_QUAD(0.f,0.f,2.f,2.f,"mainmenu_bg.png");
    TextureCoordinate retry, menu;
    retry.GetBL()[1] = 0.5f;
    retry.GetBR()[1] = retry.GetBL()[1];
    retry_button = UI_QUAD(0.6,-0.5,0.3,0.3,"end_screen_buttons.png", retry);
    menu.GetTL()[1] = 0.5f;
    menu.GetTR()[1] = menu.GetTL()[1];
    back_to_menu_button = UI_QUAD(-0.6,-0.5,0.3,0.3,"end_screen_buttons.png", menu);
}

void Map1::Update(float dt)
{
    // start timer
    start_light_timer += dt;

    if (!countdown_started && start_light_timer >= 0.0f)
    {
        AUDIO.PlayAudio("audio/countdown.wav");
        AUDIO.UpdateAudioVolume("audio/countdown.wav", 0.6f);
        countdown_started = true; // Ensures this block never runs again
    }



    if (start_light_timer < 3.1f) // game starting
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
        {
            //HighScore::saveScoreInMap(mapNumber, race_timer);
            Reset();
        }


        if (back_to_menu_button.Touched())
        {
            //HighScore::saveScoreInMap(mapNumber, race_timer);
            GSM.ChangeState(new MenuState);
        }

    }
    else // game running
    {
        AUDIO.UpdateAudioVolume("audio/gameMusic.mp3", 0.15f);

        if (accelerator.Hold())
        {
            AUDIO.PlayLoopingAudio("audio/accelerate.wav");
            AUDIO.UpdateAudioVolume("audio/accelerate.wav", 2.0f);
        }
        else
        {
            // Stop the sound when the player lets go
            AUDIO.StopAudio("audio/accelerate.wav");
        }


        if (CheckpointManager::GetInstance().isEnded())
        {
            //save score
            if (!score_saved) {
                final_time = race_timer;
                score_saved = true;
                HighScore::saveScoreInMap(mapNumber, final_time);
            }
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

    // background
    TextureCoordinate moving_bg_tc;
    float center = player.rotation / 360.f;
    moving_bg_tc.GetBL()[0] = center - 0.125f;
    moving_bg_tc.GetTL()[0] = moving_bg_tc.GetBL()[0];
    moving_bg_tc.GetBR()[0] = center + 0.125f;
    moving_bg_tc.GetTR()[0] = moving_bg_tc.GetBR()[0];
    background = UI_QUAD(0.f,0.5f,2.f,1.f,"map1_bg.png",moving_bg_tc);
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

        // render "traffic light"
        if (start_light_timer < 4.0f) start_light.DrawUI();
        FontAsset* font = AssetManager::GetInstance().GetFont("font");
        if (font)
        {
            std::string lap_text;
            lap_text += std::to_string((current_lap_int + 1) > 3 ? 3 : (current_lap_int + 1)) + " / " + std::to_string(total_lap_int);
            BatchRenderer::GetInstance().RenderText(lap_text, 0.4f, 0.7f, 0.2f, *font);

            //display race timer
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << race_timer;
            std::string racer_timer_str = oss.str();

            BatchRenderer::GetInstance().RenderText(racer_timer_str, -0.2f, 0.8f, 0.2f, *font);
        }
    }
    else // end screen comes out
    {
        end_screen_bg.DrawUI();
        retry_button.DrawUI();
        back_to_menu_button.DrawUI();

        //test highScore Font
        if(HighScore::highScoreFont)
        {
            HighScore::displayHighScores(mapNumber, final_time);
        }
    }
}

void Map1::Exit()
{
    AUDIO.StopAudio("audio/gameMusic.mp3");
    AUDIO.StopAudio("audio/accelerate.wav");
}

void Map1::Reset()
{
    GSM.ChangeState(new Map1);
}

