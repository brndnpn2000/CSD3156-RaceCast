#ifndef CHECKPOINTMANAGER_H
#define CHECKPOINTMANAGER_H

#include <vector>
#include <queue>
#include <algorithm>

#include "Environment.h"

struct Checkpoint
{
    std::vector<std::pair<int,int>> grids;
};


class CheckpointManager
{
public:
    static CheckpointManager& GetInstance() {
        static CheckpointManager instance;
        return instance;
    }

    void EndGame()
    {
        for (auto &checkpoint: checkpoint_list)
        {
            checkpoint.second = 100;
        }
    }

    int GetLapCount()
    {
        int lowest = 100000;
        for (auto &checkpoint: checkpoint_list) {
            if (checkpoint.second < lowest)
                lowest = checkpoint.second;
        }
        return lowest;
    }


    bool isEnded()
    {
        return laps >= max_laps;
    }

    void AddLapToCP(int x, int y)
    {
        // 1. Find which specific checkpoint group was hit
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
                laps = checkpoint_list[0].second; // Sync global lap count
            }
        }
        else {
            if (checkpoint_list[hitIdx].second == checkpoint_list[hitIdx - 1].second - 1) {
                checkpoint_list[hitIdx].second++;
            }
        }
    }

    void CheckLapCompletion() {
        int minLap = checkpoint_list[0].second;
        for (auto& cp : checkpoint_list) {
            if (cp.second < minLap) minLap = cp.second;
        }

        // If the lowest value across all CPs has increased, the lap is done.
        if (minLap > laps) {
            laps = minLap;
        }
    }
    template<unsigned int N, unsigned int M>
    void Init(Environment<N,M> enviro)
    {
        checkpoint_list.clear();
        laps = 0; // Reset progress

        // Directions for checking neighbors (Up, Down, Left, Right)
        int dx[] = { 0, 0, -1, 1 };
        int dy[] = { -1, 1, 0, 0 };

        for (int x = 0 ; x < N; x++) {
            for (int y = 0 ; y < M; y++)
            {
                // 1. Identify if current cell is a checkpoint (ID 11)
                if (!enviro.isACheckpoint(x, y))
                    continue;

                // 2. Skip if this cell was already grouped by a previous flood fill
                if (isCheckpoint(x, y))
                    continue;

                // 3. New unique checkpoint found; start grouping neighbors
                Checkpoint new_checkpoint;
                std::queue<std::pair<int, int>> spread_queue;

                spread_queue.push({ x, y });
                new_checkpoint.grids.push_back({ x, y });

                while (!spread_queue.empty())
                {
                    std::pair<int, int> curr = spread_queue.front();
                    spread_queue.pop();

                    for (int i = 0; i < 4; i++) {
                        int nx = curr.first + dx[i];
                        int ny = curr.second + dy[i];

                        // Boundary check and neighbor type check
                        if (nx >= 0 && nx < N && ny >= 0 && ny < M) {
                            if (enviro.isACheckpoint(nx, ny)) {
                                // Only add if not already in this specific group
                                bool alreadyInGroup = false;
                                for (auto& g : new_checkpoint.grids) {
                                    if (g.first == nx && g.second == ny) {
                                        alreadyInGroup = true;
                                        break;
                                    }
                                }

                                if (!alreadyInGroup) {
                                    spread_queue.push({ nx, ny });
                                    new_checkpoint.grids.push_back({ nx, ny });
                                }
                            }
                        }
                    }
                }
                // 4. Register the grouped checkpoint
                checkpoint_list.push_back({ new_checkpoint, 0 });
            }
        }
    }


private:

    bool isCheckpoint(int x, int y)
    {
        for (auto& checkpoints : checkpoint_list)
        {
            for (auto& checkpoint : checkpoints.first.grids)
            {
                if (checkpoint.first == x && checkpoint.second == y)
                    return true;
            }

        }
        return false;
    }




    int laps = 0;
    int max_laps = 1;
    std::vector<std::pair<Checkpoint, int>> checkpoint_list;
};

#endif
