#pragma once
#include <list>
#include <string>

class LoginWindow;

class ISocket
{
  public:
    virtual void clientConnect(LoginWindow *login) = 0;
    virtual void sendLocation(int64_t locationID) = 0;
    virtual void gameFinished() = 0;
    virtual void poll() = 0;

    virtual std::string getItemName(int64_t id, int player) = 0;
    virtual std::string getItemDesc(int player) = 0;
    virtual std::string getAddrInfo() = 0;
    virtual bool scoutLocations(std::list<int64_t> locations,
                                int createAsHint) = 0;

    virtual bool isConnected() const = 0;
    virtual std::string getUUID() const = 0;

    virtual ~ISocket() = default;
};
