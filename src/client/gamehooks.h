#pragma once

class GameHooks
{
  public:
    static bool initialize();
    static void setup();
    static void cleanup();
    static void giveItem(int ItemID, int NumItems);

  private:
    static void onReturnToMenu();
    static void onGameStop();
    static void onGameTick();
    static void onItemPickup(void *MaybeInventoryStruct, int ItemID, int NumItems);
    static bool onBrushEdit(void *MaybeInventoryStruct, int BitIndex, int Operation);
};
