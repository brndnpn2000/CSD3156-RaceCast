#include "SelectionState.h"

void SelectionState::Init()
{
    AssetManager::GetInstance().LoadTexture("image/mainmenu_bg.png");
    AssetManager::GetInstance().LoadTexture("image/play_button.png");
    AssetManager::GetInstance().LoadTexture("image/selected_frame.png");
    AssetManager::GetInstance().LoadTexture("image/play.png");
    AssetManager::GetInstance().LoadTexture("image/back.png");

    AUDIO.LoadAudio("audio/bgm.mp3");
    AUDIO.LoadAudio("audio/ui_sfx.mp3");

    AUDIO.StopAudio("audio/bgm.mp3");

    background = UI_QUAD(0,0,2,2,"mainmenu_bg.png");
    text = UI_QUAD(0,1.5,1,0.5,"play_button.png");

    float aspect_ratio = Globals::screen_size.first / Globals::screen_size.second;

    float box_size = 0.4f;

    map1 = UI_QUAD(-0.5,0,box_size,box_size * aspect_ratio,"");
    map2 = UI_QUAD(0,0,box_size,box_size * aspect_ratio,"");
    map3 = UI_QUAD(0.5,0,box_size,box_size * aspect_ratio,"");

    float frame_size = box_size + 0.05f;
    select_frame = UI_QUAD(0,0,frame_size, frame_size * aspect_ratio,"selected_frame.png");

    play_button = UI_QUAD(0.f, -0.75, 0.35, 0.25, "play.png");
    back_button = UI_QUAD(-0.82f, 0.8, 0.25, 0.18 , "back.png");
}

void SelectionState::Update(float dt)
{
    if (PlayB_Animation)
    {
        BeginningAnimation(dt);
        return;
    }

    if (map1.Touched())
    {
        selected_map = 0;
        select_frame.GetPosition() = map1.GetPosition();
    }

    else if (map2.Touched())
    {
        selected_map = 1;
        select_frame.GetPosition() = map2.GetPosition();
    }
    else if (map3.Touched())
    {
        selected_map = 2;
        select_frame.GetPosition() = map3.GetPosition();
    }

    if (back_button.Touched())
    {
        GameStateManager::GetInstance().ChangeState(new MenuState);
    }

    if (play_button.Touched())
    {
        switch (selected_map)
        {
            case 0 :
                {
                    GameStateManager::GetInstance().ChangeState(new Map1);
                }
                break;
            case 1 :
                {
                    //GameStateManager::GetInstance().ChangeState(new Map2);
                }
                break;
            case 2 :
                {
                    //GameStateManager::GetInstance().ChangeState(new Map3);
                }
                break;
        }

    }


}

void SelectionState::RenderGame()
{

}

void SelectionState::RenderUI()
{
    background.DrawUI();
    text.DrawUI();

    map1.DrawUI();
    map2.DrawUI();
    map3.DrawUI();

    select_frame.DrawUI();

    play_button.DrawUI();
    back_button.DrawUI();

}

void SelectionState::Exit()
{
    AssetManager::GetInstance().CleanAll();
}

void SelectionState::BeginningAnimation(float dt)
{
    text.GetPosition().y -= 2 * dt;
    if (text.GetPosition().y < 0.7f)
    {
        PlayB_Animation = false;
    }
}

