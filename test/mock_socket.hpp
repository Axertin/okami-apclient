#pragma once
#include "isocket.h"
#include <vector>
#include <string>

class MockSocket : public ISocket
{
public:
    std::vector<int64_t> sentLocations;
    std::vector<std::string> sentMessages;
    bool connected = true;
    std::string uuid = "mock-uuid";

    void clientConnect(LoginWindow *) override {}
    void sendLocation(int64_t id) override { sentLocations.push_back(id); }
    void gameFinished() override {}
    void poll() override {}

    std::string getItemName(int64_t, int) override { return "MockItem"; }
    std::string getItemDesc(int) override { return "Mock description"; }
    std::string getAddrInfo() override { return "127.0.0.1:1234"; }
    bool scoutLocations(std::list<int64_t>, int) override { return true; }

    bool isConnected() const override { return connected; }
    std::string getUUID() const override { return uuid; }
};
