#pragma once
#include "pch.h"
#include "window.h"
#include "frametimer.h"

class DevTools : public Window
{
public:
    DevTools() : Window("Developer Tools") {}
    void toggleVisibility() override;
    void draw(int OuterWidth, int OuterHeight, float UIScale) override;
    void updateFrameTime();

private:
    FrameTimer Framer;
};
