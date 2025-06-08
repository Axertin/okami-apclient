#pragma once
#include "pch.h"

class GameHooks
{
public:
    static void setup(void);
    static void cleanup(void);

private:
    static void onReturnToMenu(void);
    static void onGameStop(void);
    static void onGameTick(void);
    static void onTreasurePickup(void *rcx, int edx, void *r8, void *r9);
};
