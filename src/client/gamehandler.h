#pragma once
#include "pch.h"

class GameHandler
{
public:
    static void setup(void);
    static void cleanup(void);

private:
    static void onGameStart(void);
    static void onGameStop(void);
    static void onGameTick(void);
};
