#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Environment.h"
#include "UIHelper.h"
#include "Globals.h"
#include <cmath>

/*
    None = 0,
    Wall = 1,
    Tree = 2,
    Checkpoint = 3
*/

extern float g_DeviceTilt;

class PlayerObject {
public:
    Vector2D position;
    Vector2D size;
    float rotation = 0.f;
    float steering = 0.f;
    float speed = 0.f;
    const float MAX_STEER = 30.0f; // Max wheel turn angle
    const float STEER_SPEED = 150.0f;

    Vector2D velocity;

    PlayerObject();

    void onUpdate(float dt, bool accelerate, bool reverse);
    void SpeedRecalculate();

    template <unsigned int N, unsigned int M>
    void RenderRayCast(Environment<N, M>& env) {
        const float PI = 3.1415926535f;
        float fov = 60.0f;
        int numRays = (int)Globals::screen_size.first;

        for (int i = 0; i < numRays; i++) {
            float rayAngle = (rotation - (fov / 2.0f)) + ((float)i / (float)(numRays - 1)) * fov;
            float rad = rayAngle * (PI / 180.0f);
            Vector2D rayDir = { cosf(rad), sinf(rad) };

            int mapX = (int)position.x;
            int mapY = (int)position.y;

            Vector2D deltaDist = { std::abs(1.0f / rayDir.x), std::abs(1.0f / rayDir.y) };
            Vector2D sideDist;
            int stepX, stepY;

            if (rayDir.x < 0) { stepX = -1; sideDist.x = (position.x - mapX) * deltaDist.x; }
            else { stepX = 1; sideDist.x = (mapX + 1.0f - position.x) * deltaDist.x; }

            if (rayDir.y < 0) { stepY = -1; sideDist.y = (position.y - mapY) * deltaDist.y; }
            else { stepY = 1; sideDist.y = (mapY + 1.0f - position.y) * deltaDist.y; }

            bool hit = false;
            int side = 0;
            while (!hit) {
                if (sideDist.x < sideDist.y) {
                    sideDist.x += deltaDist.x;
                    mapX += stepX;
                    side = 0;
                } else {
                    sideDist.y += deltaDist.y;
                    mapY += stepY;
                    side = 1;
                }
                if (mapX < 0 || mapX >= N || mapY < 0 || mapY >= M) break;
                if (env.isWall(mapX, mapY)) hit = true;
            }

            if (hit) {
                float perpWallDist;
                if (side == 0) perpWallDist = (sideDist.x - deltaDist.x);
                else           perpWallDist = (sideDist.y - deltaDist.y);

                float correctedDist = perpWallDist * cosf((rayAngle - rotation) * (PI / 180.0f));
                float lineHeight = 1.0f / (correctedDist + 0.0001f);

                float wallHitX;
                if (side == 0) wallHitX = position.y + perpWallDist * rayDir.y;
                else           wallHitX = position.x + perpWallDist * rayDir.x;
                wallHitX -= floor(wallHitX);

                TextureCoordinate tc;
                tc.uv[0][0] = wallHitX; tc.uv[0][1] = 1.0f;
                tc.uv[1][0] = wallHitX; tc.uv[1][1] = 0.0f;
                tc.uv[2][0] = wallHitX; tc.uv[2][1] = 0.0f;
                tc.uv[3][0] = wallHitX; tc.uv[3][1] = 1.0f;

                float r = 1.0f, g = 1.0f, b = 1.0f;
                if (side == 1) { r *= 0.7f; g *= 0.7f; b *= 0.7f; }

                float fog = 1.0f / (1.0f + correctedDist * 0.2f);
                r *= fog; g *= fog; b *= fog;

                float xPos = ((float)i / (float)(numRays - 1)) * 2.0f - 1.0f;
                float quadWidth = 2.05f / (float)numRays;
                GLuint tex = AssetManager::GetInstance().GetTexture("fender.png");

                BatchRenderer::GetInstance().DrawQuad(
                        xPos, 0.0f, quadWidth, lineHeight, tex, tc, r, g, b, 1.0f
                );
            }
        }
    }

    template<unsigned int N, unsigned int M>
    void RenderDebug(Environment<N, M> env) {
        float screen_ar = Globals::screen_size.first / Globals::screen_size.second;
        float boxSize_y = 2.f / (float) M;
        float boxSize_x = boxSize_y / screen_ar;

        float grid_origin_x = -1.0f;
        float grid_origin_y = 1.0f;

        float render_pos_x = grid_origin_x + (position.x * boxSize_x);
        float render_pos_y = grid_origin_y - (position.y * boxSize_y);

        float render_size_x = size.x * boxSize_x;
        float render_size_y = size.y * boxSize_y;

        GLuint playerTex = AssetManager::GetInstance().GetTexture("player.png");

        BatchRenderer::GetInstance().DrawQuad(
                render_pos_x, render_pos_y, render_size_x, render_size_y, playerTex
        );
    }

    template <unsigned int N, unsigned int M>
    void CollisionPhysics(Environment<N, M>& env, float dt) {
        const int sub_steps = 4;
        float sub_dt = dt / (float)sub_steps;

        for (int s_step = 0; s_step < sub_steps; s_step++) {
            const float PI = 3.1415926535f;
            float rad = rotation * (PI / 180.0f);
            float s = sinf(rad);
            float c = cosf(rad);

            float hW = size.x * 0.5f;
            float hH = size.y * 0.5f;

            Vector2D localCorners[4] = {
                    { -hW,  hH }, {  hW,  hH },
                    { -hW, -hH }, {  hW, -hH }
            };

            position.x += velocity.x * sub_dt;
            position.y += velocity.y * sub_dt;

            for (int resolve_pass = 0; resolve_pass < 2; resolve_pass++) {
                for (int i = 0; i < 4; i++) {
                    float rx = localCorners[i].x * c - localCorners[i].y * s;
                    float ry = localCorners[i].x * s + localCorners[i].y * c;

                    float worldX = position.x + rx;
                    float worldY = position.y + ry;

                    int gx = (int)std::floor(worldX);
                    int gy = (int)std::floor(worldY);

                    if (gx < 0 || gx >= (int)N || gy < 0 || gy >= (int)M || env.isWall(gx, gy)) {
                        float tileCenterX = (float)gx + 0.5f;
                        float tileCenterY = (float)gy + 0.5f;

                        float dx = worldX - tileCenterX;
                        float dy = worldY - tileCenterY;

                        if (std::abs(dx) > std::abs(dy)) {
                            float pushX = (dx > 0) ? ((float)gx + 1.001f) - worldX : ((float)gx - 0.001f) - worldX;
                            position.x += pushX;
                            velocity.x *= -0.1f;
                        } else {
                            float pushY = (dy > 0) ? ((float)gy + 1.001f) - worldY : ((float)gy - 0.001f) - worldY;
                            position.y += pushY;
                            velocity.y *= -0.1f;
                        }
                    }
                }
            }
        }
    }
};

#endif