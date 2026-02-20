//
// Created by brand on 13/2/2026.
//

#ifndef RACECAST_ENVIRONMENT_H
#define RACECAST_ENVIRONMENT_H

#include "Globals.h"
#include "AssetManager.h"
#include "BatchRenderer.h"

template <unsigned int N, unsigned int M>
class Environment
{
public:

    Environment();


    Environment(std::initializer_list<unsigned int> list);

    // Adding the assignment operator allows: env = { ... } after construction
    Environment& operator=(std::initializer_list<unsigned int> list);

    void DebugAssetInit();

    void RenderDebug();

    void RenderMinimap(float x, float y);



    bool isWall(unsigned int x, unsigned int y);

    bool isACheckpoint(unsigned int x, unsigned int y);


private:
    unsigned int map[N][M];
    unsigned int width = N;
    unsigned int height = M;

    void assign_from_list(std::initializer_list<unsigned int> list);

};

#include "Environment.tpp"

#endif //RACECAST_ENVIRONMENT_H
