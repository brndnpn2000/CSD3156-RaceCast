#include "GameObject.h"

PlayerObject::PlayerObject() : position{0.0f, 0.0f}, size{1.0f, 1.0f} {}

void PlayerObject::onUpdate(float dt, bool accelerate, bool reverse) {
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

    float accel = 8.0f;
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

void PlayerObject::SpeedRecalculate() {
    speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
}