#include "MenuState.h"

void MenuState::Init()
{
    AssetManager::GetInstance().LoadTexture("image/mainmenu_bg.png");
    AssetManager::GetInstance().LoadTexture("image/RaceCast.png");
    AssetManager::GetInstance().LoadTexture("image/play_button.png");

    // Init of UI
    background = UI_QUAD(0,0,2,2,"mainmenu_bg.png");
    racecast_logo = UI_QUAD(0.f,1.4f,1,0.5,"RaceCast.png");
    play_button = UI_QUAD(0.f,-1.5f,0.8f,0.3f,"play_button.png");
}

void MenuState::Update(float dt)
{
    Blink(dt);

    if (PlayExitAnimation)
    {
        ExitAnimation(dt);
    }
    else if (PlayEntryAnimation)
    {
        EntryAnimation(dt);
    }
    else if (background.Touched())
    {
        PlayExitAnimation = true;
    }
}


void MenuState::RenderGame()
{
    // Clear with your chosen color

}

void MenuState::RenderUI()
{
    background.DrawUI();
    racecast_logo.DrawUI();
    if (blink) play_button.DrawUI();
}

void MenuState::Exit()
{
    AssetManager::GetInstance().CleanAll();
}

void MenuState::EntryAnimation(float dt)
{
    racecast_logo.GetPosition().y -= 2.f * dt;
    play_button.GetPosition().y += 2.f * dt;

    if (racecast_logo.GetPosition().y < 0.5f)
    {
        racecast_logo.GetPosition().y = 0.5f;
    }

    if (play_button.GetPosition().y > -0.6f)
    {
        play_button.GetPosition().y = -0.6f;
    }

    if (racecast_logo.GetPosition().y == 0.5f && play_button.GetPosition().y == -0.6f)
    {
        PlayEntryAnimation = false;
    }
}

void MenuState::ExitAnimation(float dt)
{

    racecast_logo.GetPosition().y += 2.f * dt;
    play_button.GetPosition().y -= 2.f * dt;
    animation_elapse += dt;
    if (animation_elapse > 0.5f)
    {
        GSM.ChangeState(new SelectionState);
        animation_elapse = 0.f;
        PlayExitAnimation = false;
    }
}

void MenuState::Blink(float dt)
{
    blink_elapsed += dt;
    if (blink_elapsed > 0.5f)
    {
        blink = true;
        if (blink_elapsed > 1.f)
        {
            blink = false;
            blink_elapsed = 0.f;
        }
    }
}

