#include "InputManager.h"

InputManager &InputManager::GetInstance()
{
    static InputManager instance;
    return instance;
}

void InputManager::HandleTouch(int action, int pointerID, float x, float y) {
    auto& touch = m_Touches[pointerID];
    touch.x = x;
    touch.y = y;

    // Correct Android Action Constants:
    // 0 = ACTION_DOWN
    // 1 = ACTION_UP
    // 2 = ACTION_MOVE
    // 5 = ACTION_POINTER_DOWN
    // 6 = ACTION_POINTER_UP

    if (action == 0 || action == 5) {
        touch.isPressed = true;
    }
    else if (action == 2) {
        // Move - just keep isPressed as true
        touch.isPressed = true;
    }
    else if (action == 1 || action == 6) {
        touch.isPressed = false;
    }
}

void InputManager::Update() {
    for (auto& [id, touch] : m_Touches) {
        // 1. Calculate if this is the EXACT frame the touch started
        touch.isTriggered = touch.isPressed && !m_WasPressedMap[id];

        // 2. Update our history map for the next frame
        m_WasPressedMap[id] = touch.isPressed;

        // Note: We do NOT set touch.isPressed = false here.
        // Only the JNI HandleTouch function can set it to false
        // when the finger physically leaves the screen.
    }
}
