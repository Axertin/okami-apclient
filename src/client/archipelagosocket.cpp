
#include "archipelagosocket.h"
#include "loginwindow.h"
#include "gamehooks.h"
#include "logger.h"
#include "receive.h"

#pragma warning(push, 0)
#include <apuuid.hpp>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4996)
const std::string uuid_file = std::string(std::getenv("APPDATA")).append("\\uuid");
#pragma warning(pop)

const std::string CertStore = "cacert.pem";

const std::string GameName = "Okami HD";
const int ItemHandlingVector = 0b111;

static std::string ConnectionStatus = "";
std::string ArchipelagoSocket::uuid = "";
static bool IsHTTPS = false;
static bool IsWSS = false;
static bool IsWS = false;
bool ArchipelagoSocket::Connected = false;
bool ArchipelagoSocket::APSyncQueued = false;

APClient *ArchipelagoSocket::Client = nullptr;

ArchipelagoSocket &ArchipelagoSocket::instance()
{
    static ArchipelagoSocket socket;
    return socket;
}

void ArchipelagoSocket::clientConnect(LoginWindow *LoginData)
{
    std::string uri = LoginData->Server;
    uuid = ap_get_uuid(uuid_file,
                       (uri.empty()) ? APClient::DEFAULT_URI : IsWS ? uri.substr(5)
                                                           : IsWSS  ? uri.substr(6)
                                                                    : uri);

    if (Client != nullptr)
        Client->reset();

    Client = new APClient(uuid, GameName, uri, CertStore);
    APSyncQueued = false;

    Client->set_slot_connected_handler([LoginData](const nlohmann::json &Data)
                                       {
                                           logInfo("[apsocket] APClient Connected");
                                           ConnectionStatus = "Connected";
                                           // bool DeathLink = false;
                                           std::list<std::string> Tags = {};
                                           // if(DeathLink)
                                           //     Tags.push_back("DeathLink");
                                           Client->ConnectUpdate(false, ItemHandlingVector, true, Tags);
                                           Client->StatusUpdate(APClient::ClientStatus::PLAYING);

                                           ; // TODO: Do other things on connection
                                       });
    Client->set_slot_disconnected_handler([LoginData]() {});

    Client->set_slot_refused_handler([LoginData](const std::list<std::string> &Errors)
                                     {
                                    for (const auto& error : Errors) {
                                        logError("[apsocket] AP Slot Refused, Error: %s", error);
                                    } });

    Client->set_room_info_handler([LoginData]()
                                  {
                                std::list<std::string> Tags;
                                Client->ConnectSlot(LoginData->Slot, LoginData->Password, ItemHandlingVector, Tags); });

    Client->set_items_received_handler([](const std::list<APClient::NetworkItem> &Items)
                                       {
                                           for (const auto &Item : Items)
                                           {
                                            
                                                logDebug("[apsocket] Received Item, ID: 0x%X", Item.item);
                                                if (Item.index <= 0 /*APSaveData.MaxReceivedIndex*/)
                                                   return;

                                               receiveAPItem(Item.item);
                                           } });

    Client->set_location_info_handler([](const std::list<APClient::NetworkItem> &Items)
                                      {
                                          ; // TODO: Do Something to handle locations
                                      });
}

void ArchipelagoSocket::sendLocation(int64_t LocationID)
{
    logInfo("[apsocket] Sending Location 0x%X", LocationID);
    std::list<int64_t>
        Check;
    Check.push_back(LocationID);
    Client->LocationChecks(Check);
}

void ArchipelagoSocket::gameFinished()
{
    Client->StatusUpdate(APClient::ClientStatus::GOAL);
}

void ArchipelagoSocket::poll()
{
    if (Client)
    {
        Client->poll();
        ; // TODO: Do something with polled data
    }
}

std::string ArchipelagoSocket::getItemName(int64_t ID, int Player)
{
    if (Client)
    {
        return Client->get_item_name(ID, Client->get_player_game(Player));
    }
    else
    {
        return "";
    }
}

std::string ArchipelagoSocket::getItemDesc(int player)
{
    if (Client)
    {
        return "Item for " + Client->get_player_alias(player) + " playing " + Client->get_player_game(player);
    }
    return "";
}

std::string ArchipelagoSocket::getAddrInfo()
{
    if (Client)
    {
        return Client->get_slot() + "_" + Client->get_seed();
    }
    return "";
}

bool ArchipelagoSocket::scoutLocations(std::list<int64_t> Locations, int CreateAsHint)
{
    if (Client)
    {
        return Client->LocationScouts(Locations, CreateAsHint);
    }
    return false;
}

bool ArchipelagoSocket::isConnected() const
{
    return Connected;
}
std::string ArchipelagoSocket::getUUID() const
{
    return uuid;
}
