//
// Created by AQIL on 2/20/2026.
//

#include "HighScore.h"

std::array<std::vector<float>, 3> HighScore::mapScores = {
    std::vector<float>{},
    std::vector<float>{},
    std::vector<float>{}
};
std::vector<float> HighScore::invalid = {0.0f};

std::vector<float>& HighScore::getMapScoreList(int mapNumber)
{
    LOGI("getMapScoreList(mapNumber=%d)", mapNumber);

    if (mapNumber < 1 || mapNumber > 3) {
        LOGI(" -> returning invalid");
        return invalid;
    }

    LOGI(" -> returning mapScores[%d], size=%zu", mapNumber, mapScores[(size_t)mapNumber].size());
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

