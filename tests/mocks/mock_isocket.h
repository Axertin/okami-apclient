#pragma once

#include <cstdint>
#include <unordered_set>
#include <string>
#include <list>
#include "isocket.h"

namespace mock
{

/**
 * Mock implementation of ISocket for testing
 *
 * Tracks all locations sent and provides query methods for test assertions.
 * All non-essential methods are no-ops for testing purposes.
 */
class MockISocket : public ISocket
{
  public:
    MockISocket() = default;
    ~MockISocket() override = default;

    // ISocket interface implementation - connection management
    void connect(const std::string &server, const std::string &slot, const std::string &password) override
    {
        // No-op for testing
    }

    void disconnect() override
    {
        // No-op for testing
    }

    bool isConnected() const override
    {
        return connected_;
    }

    // ISocket interface implementation - game integration
    void sendLocation(int64_t locationId) override
    {
        sentLocations_.insert(locationId);
    }

    void gameFinished() override
    {
        // No-op for testing
    }

    void poll() override
    {
        // No-op for testing
    }

    void processMainThreadTasks() override
    {
        // No-op for testing
    }

    // ISocket interface implementation - information queries
    std::string getItemName(int64_t id, int player) const override
    {
        return "";  // No-op for testing
    }

    std::string getItemDesc(int player) const override
    {
        return "";  // No-op for testing
    }

    std::string getConnectionInfo() const override
    {
        return "";  // No-op for testing
    }

    std::string getUUID() const override
    {
        return "";  // No-op for testing
    }

    std::string getStatus() const override
    {
        return "";  // No-op for testing
    }

    bool scoutLocations(const std::list<int64_t> &locations, int createAsHint) override
    {
        return true;  // No-op for testing
    }

    // Test helper methods
    void setConnected(bool connected)
    {
        connected_ = connected;
    }

    bool wasLocationSent(int64_t locationId) const
    {
        return sentLocations_.count(locationId) > 0;
    }

    size_t getSentLocationCount() const
    {
        return sentLocations_.size();
    }

    void clearSentLocations()
    {
        sentLocations_.clear();
    }

  private:
    bool connected_ = true;  // Default to connected for convenience
    std::unordered_set<int64_t> sentLocations_;
};

} // namespace mock
