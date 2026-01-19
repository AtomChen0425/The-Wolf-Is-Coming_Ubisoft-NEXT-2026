#include "UISystem.h"
#include "../../ContestAPI/app.h"
#include "../../ContestAPI/AppSettings.h"

// Helper to get screen position based on alignment
void UIElement::GetScreenPosition(float& outX, float& outY) const {
    outX = x;
    outY = y;

    // Apply alignment offset
    switch (alignment) {
    case UIAlignment::BottomLeft:
        // No offset needed, x and y are already absolute
        break;
    case UIAlignment::BottomCenter:
        outX = (APP_VIRTUAL_WIDTH / 2.0f) + x;
        break;
    case UIAlignment::BottomRight:
        outX = APP_VIRTUAL_WIDTH * 0.95 - x;
        break;
    case UIAlignment::MiddleLeft:
        outY = (APP_VIRTUAL_HEIGHT / 2.0f) + y;
        break;
    case UIAlignment::MiddleCenter:
        outX = (APP_VIRTUAL_WIDTH / 2.0f) + x;
        outY = (APP_VIRTUAL_HEIGHT / 2.0f) + y;
        break;
    case UIAlignment::MiddleRight:
        outX = APP_VIRTUAL_WIDTH * 0.95 - x;
        outY = (APP_VIRTUAL_HEIGHT / 2.0f) + y;
        break;
    case UIAlignment::TopLeft:
        outY = APP_VIRTUAL_HEIGHT * 0.95 - y;
        break;
    case UIAlignment::TopCenter:
        outX = (APP_VIRTUAL_WIDTH / 2.0f) + x;
        outY = APP_VIRTUAL_HEIGHT * 0.95 - y;
        break;
    case UIAlignment::TopRight:
        outX = APP_VIRTUAL_WIDTH * 0.95 - x;
        outY = APP_VIRTUAL_HEIGHT * 0.95 - y;
        break;
    }
}

// UIText implementation
void UIText::Render() const {
    if (!visible) return;

    float screenX, screenY;
    GetScreenPosition(screenX, screenY);

    App::Print(screenX, screenY, text.c_str(), r, g, b);
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
