#include "CheckpointManager.h"

CheckpointManager& CheckpointManager::GetInstance() {
    static CheckpointManager instance;
    return instance;
}

void CheckpointManager::EndGame() {
    for (auto &checkpoint : checkpoint_list) {
        checkpoint.second = 100;
    }
}

int CheckpointManager::GetLapCount() {
    int highest = 0;
    for (auto &checkpoint : checkpoint_list) {
        if (checkpoint.second > highest)
            highest = checkpoint.second;
    }
    return highest;
}

bool CheckpointManager::isEnded() {
    return laps >= max_laps;
}

void CheckpointManager::AddLapToCP(int x, int y) {
    int hitIdx = -1;
    for (int i = 0; i < (int)checkpoint_list.size(); ++i) {
        for (auto& grid : checkpoint_list[i].first.grids) {
            if (grid.first == x && grid.second == y) {
                hitIdx = i;
                break;
            }
        }
        if (hitIdx != -1) break;
    }

    if (hitIdx == -1) return;

    if (hitIdx == 0) {
        int lastIdx = (int)checkpoint_list.size() - 1;
        if (checkpoint_list[0].second == checkpoint_list[lastIdx].second) {
            checkpoint_list[0].second++;
            laps = checkpoint_list[0].second;
        }
    } else {
        if (checkpoint_list[hitIdx].second == checkpoint_list[hitIdx - 1].second - 1) {
            checkpoint_list[hitIdx].second++;
        }
    }
}

void CheckpointManager::CheckLapCompletion() {
    int minLap = checkpoint_list[0].second;
    for (auto& cp : checkpoint_list) {
        if (cp.second < minLap) minLap = cp.second;
    }

    if (minLap > laps) {
        laps = minLap;
    }
}

bool CheckpointManager::isCheckpoint(int x, int y) {
    for (auto& checkpoints : checkpoint_list) {
        for (auto& checkpoint : checkpoints.first.grids) {
            if (checkpoint.first == x && checkpoint.second == y)
                return true;
        }
    }
    return false;
}