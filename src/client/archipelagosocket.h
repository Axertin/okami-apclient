#pragma once
#include <apclient.hpp>

#include "isocket.h"

class LoginWindow; // Forward Declare LoginWindow

class ArchipelagoSocket : public ISocket
{
  public:
    static ArchipelagoSocket &instance();

    void clientConnect(LoginWindow *LoginData) override;
    void sendLocation(int64_t LocationID) override;
    void gameFinished(void) override;
    void poll(void) override;

    std::string getItemName(int64_t ID, int Player) override;
    std::string getItemDesc(int Player) override;
    std::string getAddrInfo() override;
    bool scoutLocations(std::list<int64_t> Locations, int CreateAsHint) override;

    bool isConnected() const override;
    std::string getUUID() const override;

  private:
    static bool APSyncQueued;
    static APClient *Client;

    static std::string uuid;
    static bool Connected;

    ArchipelagoSocket() = default;
};
