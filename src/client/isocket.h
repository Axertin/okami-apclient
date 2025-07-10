#pragma once
#include <functional>
#include <list>
#include <string>

class LoginWindow;

class ISocket
{
  public:
    virtual ~ISocket() = default;

    // Connection management
    virtual void connect(const std::string &server, const std::string &slot, const std::string &password) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;

    // Game integration
    virtual void sendLocation(int64_t locationID) = 0;
    virtual void gameFinished() = 0;
    virtual void poll() = 0;
    virtual void processMainThreadTasks() = 0;

    // Information queries
    virtual std::string getItemName(int64_t id, int player) const = 0;
    virtual std::string getItemDesc(int player) const = 0;
    virtual std::string getConnectionInfo() const = 0;
    virtual std::string getUUID() const = 0;
    virtual std::string getStatus() const = 0;
    virtual bool scoutLocations(const std::list<int64_t> &locations, int createAsHint) = 0;
};
