//
// Created by AQIL on 2/20/2026.
//

#ifndef RACECAST_HIGHSCORE_H
#define RACECAST_HIGHSCORE_H

#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>

#include "GameStateManager.h"
#include "GameState.h"
#include "AssetManager.h"
#include "BatchRenderer.h"
#include "Globals.h"
#include "GameObject.h"
#include "CheckpointManager.h"
#include "MenuState.h"

class HighScore {
public:
    float getHighScore() const { return m_Score; }

    void setHighScore(float value) { m_Score = value; }

    static std::vector<float>& getMapScoreList(int mapNumber);
    static std::string SerializeScoreList(const std::vector<float>& v);
    static std::vector<float> DeserializeScoreList(const std::string& s);

    static std::array<std::vector<float>, 3> mapScores;
private:
    float m_Score = 0;
    static std::vector<float> invalid;
};

#endif //RACECAST_HIGHSCORE_H


