// test/mock_socket.hpp - Updated for new ISocket interface
#pragma once
#include <list>
#include <string>
#include <vector>

#include "isocket.h"

class MockSocket : public ISocket
{
  public:
    // Test data tracking
    std::vector<int64_t> sentLocations;
    std::vector<std::string> statusHistory;
    bool connected = true;
    std::string uuid = "mock-uuid-12345";
    std::string connectionInfo = "mock_slot_seed123";

    // Connection management
    void connect(const std::string &server, const std::string &slot, const std::string &password) override
    {
        lastServer = server;
        lastSlot = slot;
        lastPassword = password;
        hasAttemptedConnection = true;

        // Simulate connection logic
        if (server.empty())
        {
            connected = false;
            currentStatus = "Server address cannot be empty";
        }
        else if (slot.empty())
        {
            connected = false;
            currentStatus = "Slot name cannot be empty";
        }
        else
        {
            connected = true;
            currentStatus = "Connected successfully!";
        }

        statusHistory.push_back(currentStatus);
    }

    void disconnect() override
    {
        connected = false;
        currentStatus = "Disconnected";
        statusHistory.push_back(currentStatus);
    }

    bool isConnected() const override
    {
        return connected;
    }

    // Game integration
    void sendLocation(int64_t locationID) override
    {
        sentLocations.push_back(locationID);
    }

    void gameFinished() override
    {
        gameCompleted = true;
    }

    void poll() override
    {
        pollCount++;
    }

    void processMainThreadTasks() override
    {
        taskProcessCount++;
    }

    // Information queries
    std::string getItemName(int64_t id, int player) const override
    {
        return "MockItem_" + std::to_string(id) + "_P" + std::to_string(player);
    }

    std::string getItemDesc(int player) const override
    {
        return "Mock item for Player " + std::to_string(player) + " (Mock Game)";
    }

    std::string getConnectionInfo() const override
    {
        return connectionInfo;
    }

    std::string getUUID() const override
    {
        return uuid;
    }

    std::string getStatus() const override
    {
        return currentStatus;
    }

    // Advanced features
    bool scoutLocations(const std::list<int64_t> &locations, int createAsHint) override
    {
        for (auto loc : locations)
        {
            scoutedLocations.push_back(loc);
        }
        lastCreateAsHint = createAsHint;
        return true;
    }

    // Test helper data
    std::string lastServer, lastSlot, lastPassword;
    std::string currentStatus = "Disconnected";
    std::vector<int64_t> scoutedLocations;
    int lastCreateAsHint = 0;
    bool gameCompleted = false;
    bool hasAttemptedConnection = false;
    int pollCount = 0;
    int taskProcessCount = 0;

    // Reset for clean tests
    void reset()
    {
        sentLocations.clear();
        statusHistory.clear();
        scoutedLocations.clear();
        connected = false;
        gameCompleted = false;
        hasAttemptedConnection = false;
        pollCount = 0;
        taskProcessCount = 0;
        currentStatus = "Disconnected";
        lastServer.clear();
        lastSlot.clear();
        lastPassword.clear();
    }
};
