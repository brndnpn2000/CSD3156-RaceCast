#ifndef PLAYSTATE_H
#define PLAYSTATE_H
#include "GameState.h"
#include "UIHelper.h"

class PlayState : public GameState
{
public:
    void Init() override;
    void Update(float dt) override;
    void RenderGame() override;
    void RenderUI() override;
    void Exit() override;

private:

};


#endif
