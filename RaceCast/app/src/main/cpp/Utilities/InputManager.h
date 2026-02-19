#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <map>

struct TouchPoint
{
    float x = 0.f;
    float y = 0.f;
    bool isPressed = false;
    bool isTriggered = false;
};

class InputManager
{
public:
    static InputManager& GetInstance();

    void HandleTouch(int action, int pointerID, float x , float y);
    void Update();

    const TouchPoint& GetTouch(int pointerId) { return m_Touches[pointerId]; }
    const std::map<int, TouchPoint>& GetAllTouches() { return m_Touches; }

private:
    std::map<int, TouchPoint> m_Touches;
    std::map<int, bool> m_WasPressedMap;
};


#endif
