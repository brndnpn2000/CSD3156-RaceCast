#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Environment.h"
#include "UIHelper.h"
#include "Globals.h"

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

    PlayerObject() {}

    void onUpdate(float dt, bool accelerate, bool reverse)
    {
        float rawTilt = g_DeviceTilt;

        if (std::abs(rawTilt) < 0.5f) {
            rawTilt = 0.0f;
        }

        float sensitivity = MAX_STEER / 7.0f;
        float targetSteering = rawTilt * sensitivity;

        float lerpSpeed = 10.0f;
        steering += (targetSteering - steering) * lerpSpeed * dt;

        // Clamp steering
        if (steering > MAX_STEER) steering = MAX_STEER;
        if (steering < -MAX_STEER) steering = -MAX_STEER;


        float accel = 5.0f;
        if (accelerate) {
            speed += accel * dt;
        } else if (reverse) {
            speed -= accel * dt;
        }

        speed *= pow(0.95f, dt * 60.0f);


        float turnFactor = 1.5f;
        rotation += steering * speed * turnFactor * dt;

        const float PI = 3.1415926535f;
        float rad = rotation * (PI / 180.0f);

        velocity.x = cosf(rad) * speed;
        velocity.y = sinf(rad) * speed;
    }

    void SpeedRecalculate()
    {
        speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    }

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

                // --- 1. Calculate Texture U-Coordinate ---
                float wallHitX; // Where exactly on the wall the ray hit (0.0 to 1.0)
                if (side == 0) wallHitX = position.y + perpWallDist * rayDir.y;
                else           wallHitX = position.x + perpWallDist * rayDir.x;
                wallHitX -= floor(wallHitX); // Get only the fractional part

                // --- 2. Create Texture Coordinates ---
                TextureCoordinate tc;
                // Vertical strips use the full height (V: 0 to 1)
                // but only a thin slice of the width (U: wallHitX)
                tc.uv[0][0] = wallHitX; tc.uv[0][1] = 1.0f; // Top-Left of strip
                tc.uv[1][0] = wallHitX; tc.uv[1][1] = 0.0f; // Bottom-Left
                tc.uv[2][0] = wallHitX; tc.uv[2][1] = 0.0f; // Bottom-Right
                tc.uv[3][0] = wallHitX; tc.uv[3][1] = 1.0f; // Top-Right

                // --- 3. Shading Logic ---
                // Base color (White)
                float r = 1.0f, g = 1.0f, b = 1.0f;

                // Make Y-axis walls (side 1) darker for 3D depth
                if (side == 1) {
                    r *= 0.7f; g *= 0.7f; b *= 0.7f;
                }

                // Optional: Distance shading (darker further away)
                float fog = 1.0f / (1.0f + correctedDist * 0.2f);
                r *= fog; g *= fog; b *= fog;

                // --- 4. Render ---
                float xPos = ((float)i / (float)(numRays - 1)) * 2.0f - 1.0f;
                float quadWidth = 2.05f / (float)numRays;
                GLuint tex = AssetManager::GetInstance().GetTexture("fender.png");

                // Assuming DrawQuad supports color tinting
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
                render_pos_x,
                render_pos_y,
                render_size_x,
                render_size_y,
                playerTex
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

            // 1. Move the player
            position.x += velocity.x * sub_dt;
            position.y += velocity.y * sub_dt;

            // 2. Continuous Correction (Run twice for corner cases)
            for (int resolve_pass = 0; resolve_pass < 2; resolve_pass++) {
                for (int i = 0; i < 4; i++) {
                    float rx = localCorners[i].x * c - localCorners[i].y * s;
                    float ry = localCorners[i].x * s + localCorners[i].y * c;

                    float worldX = position.x + rx;
                    float worldY = position.y + ry;

                    int gx = (int)std::floor(worldX);
                    int gy = (int)std::floor(worldY);

                    if (gx < 0 || gx >= N || gy < 0 || gy >= M || env.isWall(gx, gy)) {
                        // Find the overlap on both axes relative to the tile we hit
                        float tileCenterX = (float)gx + 0.5f;
                        float tileCenterY = (float)gy + 0.5f;

                        float dx = worldX - tileCenterX;
                        float dy = worldY - tileCenterY;

                        // Resolve on the axis of LEAST penetration (shortest path out)
                        if (std::abs(dx) > std::abs(dy)) {
                            // Push horizontally
                            float pushX = (dx > 0) ? ((float)gx + 1.001f) - worldX : ((float)gx - 0.001f) - worldX;
                            position.x += pushX;
                            velocity.x *= -0.1f; // Dampen speed
                        } else {
                            // Push vertically
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