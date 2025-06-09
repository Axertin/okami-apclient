
#include "pch.h"
#include "archipelago.h"

#pragma warning(push, 0)
#include <apuuid.hpp>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4996)
const std::string uuid_file = std::string(std::getenv("APPDATA")).append("\\uuid");
#pragma warning(pop)

const std::string CertStore = "cacert.pem";

const std::string GameName = "Ōkami HD";
const int ItemHandlingVector = 0b111;

static APClient *ap = nullptr;
static std::string uuid = "";
static std::string Seed = "";
static std::string ConnectionStatus = "";
static bool IsHTTPS = false;
static bool IsWSS = false;
static bool IsWS = false;
static bool APConnected = false;
static bool APSyncQeued = false;

void clientInit()
{
}

void clientConnect(ConnectionInfo Connection)
{
    uuid = ap_get_uuid(uuid_file,
                       (Connection.URI.empty()) ? APClient::DEFAULT_URI : IsWS ? Connection.URI.substr(5)
                                                                      : IsWSS  ? Connection.URI.substr(6)
                                                                               : Connection.URI);

    if (ap != nullptr)
        ap->reset();

    ap = new APClient(uuid, GameName, Connection.URI, CertStore);
    APSyncQeued = false;

    ap->set_slot_connected_handler([Connection](const nlohmann::json &Data)
                                   {
                                       ConnectionStatus = "Connected";
                                       // bool DeathLink = false;
                                       std::list<std::string> Tags = {};
                                       // if(DeathLink)
                                       //     Tags.push_back("DeathLink");
                                       ap->ConnectUpdate(false, ItemHandlingVector, true, Tags);
                                       ap->StatusUpdate(APClient::ClientStatus::PLAYING);

                                       ; // TODO: Do other things on connection
                                   });
    ap->set_slot_disconnected_handler([Connection]() {});

    ap->set_slot_refused_handler([Connection](const std::list<std::string> &Errors)
                                 {
                                    for (const auto& error : Errors) {
                                        ; // TODO: Do something when refused a slot
                                    } });

    ap->set_room_info_handler([Connection]()
                              {
                                std::list<std::string> Tags;
                                ap->ConnectSlot(Connection.Slot, Connection.Password, ItemHandlingVector, Tags); });

    ap->set_items_received_handler([](const std::list<APClient::NetworkItem> &Items)
                                   {
                                    for (const auto& Item : Items) {
                                        ; // TODO: Do something with the items we get
                                    } });

    ap->set_location_info_handler([](const std::list<APClient::NetworkItem> &Items)
                                  {
                                      ; // TODO: Do Something to handle locations
                                  });
}

void sendLocation(int64_t LocationID)
{
    std::list<int64_t> Check;
    Check.push_back(LocationID);
    ap->LocationChecks(Check);
}

void gameFinished()
{
    ap->StatusUpdate(APClient::ClientStatus::GOAL);
}

void pollSocket()
{
    if (ap)
    {
        ap->poll();
        ; // TODO: Do something with polled data
    }
}

std::string getItemName(int64_t ID, int Player)
{
    if (ap)
    {
        return ap->get_item_name(ID, ap->get_player_game(Player));
    }
    else
    {
        return "";
    }
}

std::string getItemDesc(int player)
{
    if (ap)
    {
        return "Item for " + ap->get_player_alias(player) + " playing " + ap->get_player_game(player);
    }
    return "";
}

std::string getAddrInfo()
{
    if (ap)
    {
        return ap->get_slot() + "_" + ap->get_seed();
    }
    return "";
}

bool scoutLocations(std::list<int64_t> Locations, int CreateAsHint)
{
    if (ap)
    {
        return ap->LocationScouts(Locations, CreateAsHint);
    }
    return false;
}
