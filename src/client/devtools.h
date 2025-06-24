#pragma once
#include "frametimer.h"
#include "window.h"

class DevTools : public Window
{
  public:
    DevTools() : Window("Developer Tools")
    {
    }
    void toggleVisibility() override;
    void draw(int OuterWidth, int OuterHeight, float UIScale) override;

  private:
    FrameTimer Framer;
};
