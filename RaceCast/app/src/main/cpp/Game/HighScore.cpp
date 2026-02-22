//
// Created by AQIL on 2/20/2026.
//

#include <algorithm>

#include "HighScore.h"

FontAsset* HighScore::highScoreFont;

std::array<std::vector<float>, 3> HighScore::mapScores = {
    std::vector<float>{},
    std::vector<float>{},
    std::vector<float>{}
};
std::vector<float> HighScore::invalid = {0.0f};

void HighScore::displayHighScores(int& mapNumber, float& final_time)
{
    BatchRenderer::GetInstance().RenderText("HIGHSCORES",-0.35f,0.8f, 0.1f, *HighScore::highScoreFont);
    std::vector<float>& currMapScoreList = HighScore::getMapScoreList(mapNumber);
    float y_start = 0.6f;
    for(size_t i = 0;i<currMapScoreList.size();++i)
    {
        float currScoreFloat = currMapScoreList[i];
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << currScoreFloat;
        std::string scoreStr = oss.str();
        std::string displayScore = std::to_string(i+1) + ". " + scoreStr;
        BatchRenderer::GetInstance().RenderText(displayScore,-0.35f,y_start, 0.1f, *HighScore::highScoreFont);
        y_start -= 0.2f;
    }

    BatchRenderer::GetInstance().RenderText("Your Score:",-0.35f,y_start-0.2f, 0.1f, *HighScore::highScoreFont);
    std::ostringstream oss2;
    oss2 << std::fixed << std::setprecision(1) << final_time;
    std::string finalScoreStr = oss2.str();
    BatchRenderer::GetInstance().RenderText(finalScoreStr,-0.35f,y_start-0.4f, 0.1f, *HighScore::highScoreFont);
}

void HighScore::saveScoreInMap(int mapNum, float score) {
    std::vector<float>& currMapScoreList = getMapScoreList(mapNum);
    currMapScoreList.push_back(score);

    std::sort(currMapScoreList.begin(), currMapScoreList.end());

    if (currMapScoreList.size() > 5) currMapScoreList.resize(5);
    LOGI("Score Saved: %f", score);
}

std::vector<float>& HighScore::getMapScoreList(int mapNumber)
{
    //LOGI("getMapScoreList(mapNumber=%d)", mapNumber);

    if (mapNumber < 1 || mapNumber > 3) {
        //LOGI(" -> returning invalid");
        return invalid;
    }

    //LOGI(" -> returning mapScores[%d], size=%zu", mapNumber - 1, mapScores[(size_t)mapNumber - 1].size());
    return mapScores[(size_t)mapNumber - 1];
}

std::string HighScore::SerializeScoreList(const std::vector<float> &v) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) oss << ",";
        oss << v[i];
    }
    oss << "]";
    return oss.str();
}

std::vector<float> HighScore::DeserializeScoreList(const std::string &s) {
    std::vector<float> out;
    // very small parser: extract numbers between [ ]
    const char* p = s.c_str();
    while (*p) {
        // skip until digit, minus, or dot
        while (*p && !((*p >= '0' && *p <= '9') || *p == '-' || *p == '.')) p++;
        if (!*p) break;
        char* end = nullptr;
        float val = std::strtof(p, &end);
        if (end == p) break;
        out.push_back(val);
        p = end;
    }
    return out;
}

