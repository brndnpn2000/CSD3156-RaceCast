#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "AssetManager.h"
#include "BatchRenderer.h"
#include "InputManager.h"

struct Vector2D
{
    float x , y;

    Vector2D() : x(0.f), y(0.f) {}
    Vector2D(float _x, float _y) : x(_x), y(_y) {}

    // --- Addition ---
    Vector2D operator+(const Vector2D& other) const {
        return { x + other.x, y + other.y };
    }
    Vector2D& operator+=(const Vector2D& other) {
        x += other.x; y += other.y;
        return *this;
    }

    // --- Subtraction ---
    Vector2D operator-(const Vector2D& other) const {
        return { x - other.x, y - other.y };
    }
    Vector2D& operator-=(const Vector2D& other) {
        x -= other.x; y -= other.y;
        return *this;
    }

    // --- Scalar Multiplication (Scaling Velocity/Friction) ---
    Vector2D operator*(float scalar) const {
        return { x * scalar, y * scalar };
    }
    Vector2D& operator*=(float scalar) {
        x *= scalar; y *= scalar;
        return *this;
    }

    // --- Scalar Division ---
    Vector2D operator/(float scalar) const {
        return { x / scalar, y / scalar };
    }

    // --- Negation (Reversing direction) ---
    Vector2D operator-() const {
        return { -x, -y };
    }
};

inline Vector2D operator*(float scalar, const Vector2D& v) {
    return {v.x * scalar, v.y * scalar};
}

class UI_QUAD
{
public:
    UI_QUAD() {}
    UI_QUAD(float x, float y, float w, float h, const std::string& texture, TextureCoordinate tc = TextureCoordinate());

    bool Touched();

    bool Hold();

    void DrawUI();

    Vector2D& GetPosition() { return m_position; }
    Vector2D& GetSize() { return m_size; }

private:
    std::string texture_name;
    Vector2D  m_position;
    Vector2D  m_size;
    TextureCoordinate m_tc;
};

#endif
