#pragma once

#include "logger.h"
#include "window.h"

// Console - Handles UI rendering only
class Console : public Window
{
public:
    Console();               // Default constructor uses global logger
    Console(Logger &logger); // Alternative constructor for custom logger
    ~Console();

    void toggleVisibility() override;
    void draw(int OuterWidth, int OuterHeight, float UIScale) override;

private:
    void drawLogOutput();

    Logger &logger_;
    bool autoScroll_;
    bool showTimestamps_;
    bool levelEnabled_[4];
};
