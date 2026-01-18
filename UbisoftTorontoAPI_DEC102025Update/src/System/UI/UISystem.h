#pragma once
#include <string>
#include <vector>
#include <memory>

// UI Alignment options
enum class UIAlignment {
    TopLeft,
    TopCenter,
    TopRight,
    MiddleLeft,
    MiddleCenter,
    MiddleRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

// Base UI Element class
class UIElement {
public:
    UIElement(float x, float y, UIAlignment alignment = UIAlignment::TopLeft)
        : x(x), y(y), alignment(alignment), visible(true) {
    }

    virtual ~UIElement() = default;
    virtual void Render() const = 0;

    void SetPosition(float newX, float newY) { x = newX; y = newY; }
    void SetAlignment(UIAlignment newAlignment) { alignment = newAlignment; }
    void SetVisible(bool isVisible) { visible = isVisible; }
    bool IsVisible() const { return visible; }

protected:
    float x, y;
    UIAlignment alignment;
    bool visible;

    // Helper to get screen position based on alignment
    void GetScreenPosition(float& outX, float& outY) const;
};

// UI Text Element
class UIText : public UIElement {
public:
    UIText(const std::string& text, float x, float y,
        float r = 1.0f, float g = 1.0f, float b = 1.0f,
        UIAlignment alignment = UIAlignment::TopLeft)
        : UIElement(x, y, alignment), text(text), r(r), g(g), b(b), font(nullptr) {
    }

    void SetText(const std::string& newText) { text = newText; }
    void SetColor(float red, float green, float blue) { r = red; g = green; b = blue; }
    void SetFont(void* newFont) { font = newFont; }

    void Render() const override;

private:
    std::string text;
    float r, g, b;
    void* font;
};

// UI Manager - manages all UI elements
class UIManager {
public:
    UIManager() = default;
    ~UIManager() = default;

    // Add UI elements
    UIText* AddText(const std::string& text, float x, float y,
        float r = 1.0f, float g = 1.0f, float b = 1.0f,
        UIAlignment alignment = UIAlignment::TopLeft);

    // Clear all UI elements
    void Clear();

    // Render all visible UI elements
    void Render() const;

    // Get all elements for direct manipulation
    std::vector<std::unique_ptr<UIElement>>& GetElements() { return elements; }

private:
    std::vector<std::unique_ptr<UIElement>> elements;
};
