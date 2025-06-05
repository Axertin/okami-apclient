#pragma once

class GameHandler
{
public:
    static void setup(void);
    static void cleanup(void);

private:
    static void onGameTick(void);
};
