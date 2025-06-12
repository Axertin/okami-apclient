#pragma once
#include "pch.h"

class GameHooks
{
public:
    static void setup(void);
    static void cleanup(void);
    static void giveItem(int ItemID, int NumItems);

private:
    static void onReturnToMenu(void);
    static void onGameStop(void);
    static void onGameTick(void);
    static void onItemPickup(void *MaybeInventoryStruct, int ItemID, int NumItems);
};
