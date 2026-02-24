#ifndef CHECKPOINTMANAGER_H
#define CHECKPOINTMANAGER_H

#include <vector>
#include <queue>
#include <algorithm>
#include "Environment.h"

struct Checkpoint {
    std::vector<std::pair<int, int>> grids;
};

class CheckpointManager {
public:
    static CheckpointManager& GetInstance();

    void EndGame();
    int GetLapCount();
    bool isEnded();
    void AddLapToCP(int x, int y);
    void CheckLapCompletion();

    template<unsigned int N, unsigned int M>
    void Init(Environment<N, M> enviro) {
        checkpoint_list.clear();
        laps = 0;

        int dx[] = { 0, 0, -1, 1 };
        int dy[] = { -1, 1, 0, 0 };

        for (int x = 0; x < N; x++) {
            for (int y = 0; y < M; y++) {
                if (!enviro.isACheckpoint(x, y))
                    continue;

                if (isCheckpoint(x, y))
                    continue;

                Checkpoint new_checkpoint;
                std::queue<std::pair<int, int>> spread_queue;

                spread_queue.push({ x, y });
                new_checkpoint.grids.push_back({ x, y });

                while (!spread_queue.empty()) {
                    std::pair<int, int> curr = spread_queue.front();
                    spread_queue.pop();

                    for (int i = 0; i < 4; i++) {
                        int nx = curr.first + dx[i];
                        int ny = curr.second + dy[i];

                        if (nx >= 0 && nx < N && ny >= 0 && ny < M) {
                            if (enviro.isACheckpoint(nx, ny)) {
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
                checkpoint_list.push_back({ new_checkpoint, 0 });
            }
        }
    }

private:
    CheckpointManager() = default; // Private constructor for Singleton
    bool isCheckpoint(int x, int y);

    int laps = 0;
    int max_laps = 3;
    std::vector<std::pair<Checkpoint, int>> checkpoint_list;
};

#endif