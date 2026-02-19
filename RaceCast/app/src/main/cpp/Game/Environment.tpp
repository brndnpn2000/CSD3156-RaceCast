#include "Environment.h"

template<unsigned int N, unsigned int M>
Environment<N,M>::Environment()
{
    for (int m{0} ; m < M; m++)
        for (int n{0} ; n < N; n++)
            map[n][m] = 0;
}

template<unsigned int N, unsigned int M>
Environment<N,M>::Environment(std::initializer_list<unsigned int> list)
{
    assign_from_list(list);
}

template<unsigned int N, unsigned int M>
Environment<N,M>& Environment<N,M>::operator=(std::initializer_list<unsigned int> list)
{
    assign_from_list(list);
    return *this;
}

template<unsigned int N, unsigned int M>
bool Environment<N,M>::isWall(unsigned int x, unsigned int y)
{
    return (map[y][x] == 1) ? true : false;
}

template<unsigned int N, unsigned int M>
bool Environment<N,M>::isACheckpoint(unsigned int x, unsigned int y)
{
    return (map[y][x] == 11) ? true : false;
}

template<unsigned int N, unsigned int M>
void Environment<N, M>::assign_from_list(std::initializer_list<unsigned int> list)
{
    auto it = list.begin();
    for (unsigned int i = 0; i < N; ++i) {
        for (unsigned int j = 0; j < M; ++j) {
            if (it != list.end()) {
                map[i][j] = *it++;
            } else {
                // Optional: fill remaining with default if list is too short
                map[i][j] = static_cast<unsigned int>(0);
            }
        }
    }
}

template<unsigned int N, unsigned int M>
void Environment<N, M>::DebugAssetInit()
{
    AssetManager::GetInstance().LoadTexture("image/white-grid.png");
    AssetManager::GetInstance().LoadTexture("image/black-grid.png");
    AssetManager::GetInstance().LoadTexture("image/tree.png");
}


template<unsigned int N, unsigned int M>
void Environment<N, M>::RenderDebug()
{
    float screen_ar = Globals::screen_size.first / Globals::screen_size.second;
    float boxSize_x = 0.f; // ndc
    float boxSize_y = 0.f; // ndc

    boxSize_y = 2.f / (float)M;
    boxSize_x = boxSize_y / screen_ar;

    float starting_point_x = -1 + boxSize_x / 2.f;
    float starting_point_y =  1 - boxSize_y / 2.f;

    for (unsigned int n = 0; n < N ; n++)
        for (unsigned int m = 0 ; m < M ; m++)
        {
            float position_x = starting_point_x + m * boxSize_x;
            float position_y = starting_point_y - n * boxSize_y;

            unsigned int current_grid = map[n][m];

            GLuint texture = 0;

            switch (current_grid)
            {
                case 0 :
                {
                    texture = AssetManager::GetInstance().GetTexture("white-grid.png");
                }
                    break;

                case 1 :
                {
                    texture = AssetManager::GetInstance().GetTexture("black-grid.png");
                }
                    break;

                case 2 :
                {
                    texture = AssetManager::GetInstance().GetTexture("tree.png");
                }
                    break;
            }

            BatchRenderer::GetInstance().DrawQuad(position_x,position_y,boxSize_x,boxSize_y, texture);
        }
}

template<unsigned int N, unsigned int M>
void Environment<N, M>::RenderMinimap(float playerX, float playerY)
{
    // 1. Setup Scaling (Minimap is 50% of screen height)
    float minimapScale = 0.5f;
    float screen_ar = Globals::screen_size.first / Globals::screen_size.second;

    // 2. Calculate Tile Sizes in NDC
    float boxSize_y = minimapScale / (float)M;
    float boxSize_x = boxSize_y / screen_ar;

    // 3. Anchoring to Top-Left (-1, 1)
    float padding = 0.05f;
    float starting_point_x = -1.0f + (boxSize_x / 2.0f) + padding;
    float starting_point_y =  1.0f - (boxSize_y / 2.0f) - padding;

    // 4. Render Only Wall Tiles (Black)
    for (unsigned int n = 0; n < N; n++) {
        for (unsigned int m = 0; m < M; m++) {
            if (map[n][m] == 1) { // Only render if it's a wall
                float pos_x = starting_point_x + m * boxSize_x;
                float pos_y = starting_point_y - n * boxSize_y;

                BatchRenderer::GetInstance().DrawQuad(
                        pos_x, pos_y, boxSize_x, boxSize_y,
                        AssetManager::GetInstance().GetTexture("white-grid.png"),
                        TextureCoordinate(), 1.0f, 1.0f, 1.0f, 0.8f
                );
            }
        }
    }

    // 5. Render Player Position (White)
    // Map player's world float coordinates to the minimap's NDC space
    float p_ndc_x = starting_point_x + (playerX * boxSize_x);
    float p_ndc_y = starting_point_y - (playerY * boxSize_y);

    BatchRenderer::GetInstance().DrawQuad(
            p_ndc_x, p_ndc_y,
            boxSize_x * 1.2f, boxSize_y * 1.2f, // Slightly larger for visibility
            AssetManager::GetInstance().GetTexture("black-grid.png"),
            TextureCoordinate(), 1.0f, 1.0f, 1.0f, 1.0f
    );
}
