#pragma once

#include <chrono>
#include <functional>
#include <list>
#include <string>
#include <vector>

#include <stdint.h>

/**
 * @brief Scouted item information returned from AP server
 */
struct ScoutedItem
{
    int64_t item;     // AP item ID
    int64_t location; // Location ID that was scouted
    int player;       // Destination player slot
    unsigned flags;   // Item classification flags
};

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
    virtual void sendLocations(const std::vector<int64_t> &locationIDs) = 0;
    virtual void gameFinished() = 0;
    virtual void poll() = 0;
    virtual void processMainThreadTasks() = 0;

    // Information queries
    virtual std::string getItemName(int64_t id, int player) const = 0;
    virtual std::string getItemDesc(int player) const = 0;
    virtual std::string getConnectionInfo() const = 0;
    virtual std::string getUUID() const = 0;
    virtual std::string getStatus() const = 0;

    // Location scouting
    virtual bool scoutLocations(const std::list<int64_t> &locations, int createAsHint) = 0;

    /**
     * @brief Scout locations synchronously (blocking)
     *
     * @param locations List of location IDs to scout
     * @param createAsHint Hint creation mode (0=none, 1=create, 2=create_no_send)
     * @param timeout Maximum time to wait for response
     * @return Vector of scouted items, empty if failed or timeout
     */
    virtual std::vector<ScoutedItem> scoutLocationsSync(const std::list<int64_t> &locations, int createAsHint = 0,
                                                        std::chrono::milliseconds timeout = std::chrono::seconds(5)) = 0;

    /**
     * @brief Get the current player's slot number
     * @return Player slot, or -1 if not connected
     */
    virtual int getPlayerSlot() const = 0;
};
