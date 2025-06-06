#pragma once
#include "pch.h"

struct ConnectionInfo
{
    std::string URI;
    std::string Password;
    std::string Slot;
};

void clientInit(void);
void clientConnect(void);
void sendLocation(int64_t LocationID);
void gameFinished(void);
void pollSocket(void);

std::string getItemName(int64_t ID, int Player);
std::string getItemDesc(int player);
std::string getAddrInfo();
bool scoutLocations(std::list<int64_t> Locations, int CreateAsHint);