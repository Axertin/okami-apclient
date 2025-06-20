#pragma once

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
    static bool onBrushEdit(void *MaybeInventoryStruct, int BitIndex, int Operation);
};
