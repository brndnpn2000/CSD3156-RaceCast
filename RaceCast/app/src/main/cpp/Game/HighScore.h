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
#include "FontAsset.h"

class HighScore {
public:
    static std::vector<float>& getMapScoreList(int mapNumber);
    static std::string SerializeScoreList(const std::vector<float>& v);
    static std::vector<float> DeserializeScoreList(const std::string& s);
    static void saveScoreInMap(int mapNum, float score);
    static void displayHighScores(int& mapNumber, float& final_time);

    static FontAsset* highScoreFont;
    static std::array<std::vector<float>, 3> mapScores;
private:
    static std::vector<float> invalid;
};

#endif //RACECAST_HIGHSCORE_H


