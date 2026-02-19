#include "UIHelper.h"

bool UI_QUAD::Touched()
{
    auto& allTouches = InputManager::GetInstance().GetAllTouches();

    float halfWidth  = m_size.x * 0.5f;
    float halfHeight = m_size.y * 0.5f;

    float left   = m_position.x - halfWidth;
    float right  = m_position.x + halfWidth;
    float bottom = m_position.y - halfHeight;
    float top    = m_position.y + halfHeight;

    for (auto const& [id, touch] : allTouches) {
        if (touch.isTriggered) {
            if (touch.x >= left && touch.x <= right &&
                touch.y >= bottom && touch.y <= top)
            {
                return true;
            }
        }
    }

    return false;
}

bool UI_QUAD::Hold()
{
    auto& allTouches = InputManager::GetInstance().GetAllTouches();

    float halfWidth  = m_size.x * 0.5f;
    float halfHeight = m_size.y * 0.5f;

    float left   = m_position.x - halfWidth;
    float right  = m_position.x + halfWidth;
    float bottom = m_position.y - halfHeight;
    float top    = m_position.y + halfHeight;

    for (auto const& [id, touch] : allTouches) {
        if (touch.isPressed) {
            if (touch.x >= left && touch.x <= right &&
                touch.y >= bottom && touch.y <= top)
            {
                return true;
            }
        }
    }

    return false;
}

void UI_QUAD::DrawUI()
{
    GLuint texture = AssetManager::GetInstance().GetTexture(texture_name.c_str());
    DRAW_QUAD(m_position.x, m_position.y, m_size.x, m_size.y, texture, m_tc);
}

UI_QUAD::UI_QUAD(float x, float y, float w, float h, const std::string& texture, TextureCoordinate tc):
    m_position(x,y),
    m_size(w,h),
    texture_name(texture),
    m_tc(tc) {}
