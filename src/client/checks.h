#pragma once
#include "archipelagosocket.h"
#include <unordered_map>

void checkInit();
void checkBrushes(ISocket &socket = ArchipelagoSocket::instance());
void checkBrushes(int index, ISocket &socket = ArchipelagoSocket::instance());
bool checkItems(int ItemID, ISocket &socket = ArchipelagoSocket::instance());
void enableChecks();
void disableChecks();
void checkDojoMoves();
bool isChecksEnabled();
