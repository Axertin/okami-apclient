#pragma once
#include <string>

class Window
{
public:
    virtual ~Window() = default;
    virtual void toggleVisibility() = 0;
    virtual void draw(int OuterWidth, int OuterHeight, float UIScale) = 0;
    bool IsVisible = true;
    const std::string name = "";
    Window(const std::string &n) : name(n) {}
};
