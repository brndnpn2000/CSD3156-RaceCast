#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "BatchRenderer.h"

class GameState
{
    public:
    virtual ~GameState() {}
    virtual void Init() = 0;
    virtual void Update(float dt) = 0;
    virtual void RenderGame() = 0;
    virtual void RenderUI() = 0;
    virtual void Exit() = 0;

};

void convertSecondsToMinutes(const int &in, int &min, int &seconds);

#endif