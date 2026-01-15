#include "UISystem.h"
#include "../../ContestAPI/app.h"
#include "../../ContestAPI/AppSettings.h"

// Helper to get screen position based on alignment
void UIElement::GetScreenPosition(float& outX, float& outY) const {
    outX = x;
    outY = y;
    
    // Apply alignment offset
    switch (alignment) {
        case UIAlignment::TopLeft:
            // No offset needed, x and y are already absolute
            break;
        case UIAlignment::TopCenter:
            outX = (APP_VIRTUAL_WIDTH / 2.0f) + x;
            break;
        case UIAlignment::TopRight:
            outX = APP_VIRTUAL_WIDTH - x;
            break;
        case UIAlignment::MiddleLeft:
            outY = (APP_VIRTUAL_HEIGHT / 2.0f) + y;
            break;
        case UIAlignment::MiddleCenter:
            outX = (APP_VIRTUAL_WIDTH / 2.0f) + x;
            outY = (APP_VIRTUAL_HEIGHT / 2.0f) + y;
            break;
        case UIAlignment::MiddleRight:
            outX = APP_VIRTUAL_WIDTH - x;
            outY = (APP_VIRTUAL_HEIGHT / 2.0f) + y;
            break;
        case UIAlignment::BottomLeft:
            outY = APP_VIRTUAL_HEIGHT - y;
            break;
        case UIAlignment::BottomCenter:
            outX = (APP_VIRTUAL_WIDTH / 2.0f) + x;
            outY = APP_VIRTUAL_HEIGHT - y;
            break;
        case UIAlignment::BottomRight:
            outX = APP_VIRTUAL_WIDTH - x;
            outY = APP_VIRTUAL_HEIGHT - y;
            break;
    }
}

// UIText implementation
void UIText::Render() const {
    if (!visible) return;
    
    float screenX, screenY;
    GetScreenPosition(screenX, screenY);
    
    void* renderFont = font ? font : GLUT_BITMAP_HELVETICA_18;
    App::Print(screenX, screenY, text.c_str(), r, g, b, renderFont);
}

// UIManager implementation
UIText* UIManager::AddText(const std::string& text, float x, float y,
                           float r, float g, float b,
                           UIAlignment alignment) {
    auto uiText = std::make_unique<UIText>(text, x, y, r, g, b, alignment);
    UIText* ptr = uiText.get();
    elements.push_back(std::move(uiText));
    return ptr;
}

void UIManager::Clear() {
    elements.clear();
}

void UIManager::Render() const {
    for (const auto& element : elements) {
        if (element->IsVisible()) {
            element->Render();
        }
    }
}
