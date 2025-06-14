#pragma once
#include "pch.h"
#include "archipelagosocket.h"
#include <unordered_map>

void checkBrushes(ISocket &socket = ArchipelagoSocket::instance());
bool checkItems(int ItemID, ISocket &socket = ArchipelagoSocket::instance());
void checkDojoMoves();
