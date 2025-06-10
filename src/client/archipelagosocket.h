#pragma once
#include "pch.h"

class LoginWindow; // Forward Declare LoginWindow

class ArchipelagoSocket
{
public:
    static void clientConnect(LoginWindow *LoginData);
    static void sendLocation(int64_t LocationID);
    static void gameFinished(void);
    static void poll(void);

    static std::string getItemName(int64_t ID, int Player);
    static std::string getItemDesc(int Player);
    static std::string getAddrInfo();
    static bool scoutLocations(std::list<int64_t> Locations, int CreateAsHint);

    static std::string uuid;
    static bool Connected;
    static std::string seed;

private:
    static bool APSyncQueued;
    static APClient *Client;
};
