#include <catch2/catch_section_info.hpp>
#include <catch2/catch_test_macros.hpp>

#include "aplocationmonitor.h"
#include "mock_socket.hpp"

TEST_CASE("APLocationMonitor basic functionality", "[aplocation]")
{
    APLocationMonitor &monitor = APLocationMonitor::instance();
    MockSocket mock;

    // Reset state for clean testing
    monitor.reset();
    mock.reset();

    SECTION("Singleton instance works")
    {
        APLocationMonitor &monitor2 = APLocationMonitor::instance();
        REQUIRE(&monitor == &monitor2);
    }

    SECTION("Initial state is correct")
    {
        REQUIRE_FALSE(monitor.isSendingEnabled());
        REQUIRE(monitor.getLocationsSentCount() == 0);
    }

    SECTION("Configuration methods work")
    {
        monitor.setSocket(&mock);
        monitor.enableSending(true);

        REQUIRE(monitor.isSendingEnabled());

        monitor.enableSending(false);
        REQUIRE_FALSE(monitor.isSendingEnabled());
    }
}

TEST_CASE("APLocationMonitor location ID calculations", "[aplocation]")
{
    SECTION("Item location IDs")
    {
        REQUIRE(APLocationMonitor::getItemLocationId(1) == 100001);
        REQUIRE(APLocationMonitor::getItemLocationId(255) == 100255);
        REQUIRE(APLocationMonitor::getItemLocationId(0x79) == 100121); // Karmic Transformer 3
    }

    SECTION("Brush location IDs")
    {
        REQUIRE(APLocationMonitor::getBrushLocationId(0) == 200000);
        REQUIRE(APLocationMonitor::getBrushLocationId(3) == 200003);
        REQUIRE(APLocationMonitor::getBrushLocationId(12) == 200012);
    }

    SECTION("Shop location IDs")
    {
        REQUIRE(APLocationMonitor::getShopLocationId(1, 0) == 301000);
        REQUIRE(APLocationMonitor::getShopLocationId(5, 3) == 305003);
        REQUIRE(APLocationMonitor::getShopLocationId(0, 999) == 300999);
    }

    SECTION("World state bit location IDs")
    {
        REQUIRE(APLocationMonitor::getWorldStateBitLocationId(0, 0) == 400000);
        REQUIRE(APLocationMonitor::getWorldStateBitLocationId(1, 13) == 410013);
        REQUIRE(APLocationMonitor::getWorldStateBitLocationId(82, 255) == 1220255);
    }

    SECTION("Collected object location IDs")
    {
        REQUIRE(APLocationMonitor::getCollectedObjectLocationId(0, 0) == 500000);
        REQUIRE(APLocationMonitor::getCollectedObjectLocationId(5, 42) == 550042);
        REQUIRE(APLocationMonitor::getCollectedObjectLocationId(83, 95) == 1330095);
    }

    SECTION("Area restoration location IDs")
    {
        REQUIRE(APLocationMonitor::getAreasRestoredLocationId(0, 0) == 600000);
        REQUIRE(APLocationMonitor::getAreasRestoredLocationId(10, 15) == 700015);
        REQUIRE(APLocationMonitor::getAreasRestoredLocationId(50, 80) == 1100080);
    }

    SECTION("Global flag location IDs")
    {
        REQUIRE(APLocationMonitor::getGlobalFlagLocationId(0) == 700000);
        REQUIRE(APLocationMonitor::getGlobalFlagLocationId(41) == 700041); // Save menu flag
        REQUIRE(APLocationMonitor::getGlobalFlagLocationId(85) == 700085);
    }
}

TEST_CASE("APLocationMonitor event-based location sending", "[aplocation]")
{
    APLocationMonitor &monitor = APLocationMonitor::instance();
    MockSocket mock;

    // Setup
    monitor.reset();
    mock.reset();
    monitor.setSocket(&mock);
    monitor.enableSending(true);
    mock.connected = true;

    SECTION("Item pickup sends correct location")
    {
        monitor.onItemPickup(0x79, 1); // Karmic Transformer 3

        REQUIRE(mock.sentLocations.size() == 1);
        REQUIRE(mock.sentLocations[0] == 100121); // 100000 + 0x79
        REQUIRE(monitor.getLocationsSentCount() == 1);
    }

    SECTION("Brush acquisition sends correct location")
    {
        monitor.onBrushAcquired(3);

        REQUIRE(mock.sentLocations.size() == 1);
        REQUIRE(mock.sentLocations[0] == 200003); // 200000 + 3
        REQUIRE(monitor.getLocationsSentCount() == 1);
    }

    SECTION("Shop purchase sends correct location")
    {
        monitor.onShopPurchase(5, 2, 0x42);

        REQUIRE(mock.sentLocations.size() == 1);
        REQUIRE(mock.sentLocations[0] == 305002); // 300000 + (5*1000) + 2
        REQUIRE(monitor.getLocationsSentCount() == 1);
    }

    SECTION("Multiple different locations are all sent")
    {
        monitor.onItemPickup(1, 1);
        monitor.onBrushAcquired(0);
        monitor.onShopPurchase(1, 0, 99);

        REQUIRE(mock.sentLocations.size() == 3);
        REQUIRE(monitor.getLocationsSentCount() == 3);

        // Check all locations are present (order doesn't matter)
        std::vector<int64_t> expected = {100001, 200000, 301000};
        for (auto expected_loc : expected)
        {
            bool found = std::find(mock.sentLocations.begin(), mock.sentLocations.end(), expected_loc) != mock.sentLocations.end();
            REQUIRE(found);
        }
    }
}

TEST_CASE("APLocationMonitor duplicate prevention", "[aplocation]")
{
    APLocationMonitor &monitor = APLocationMonitor::instance();
    MockSocket mock;

    // Setup
    monitor.reset();
    mock.reset();
    monitor.setSocket(&mock);
    monitor.enableSending(true);
    mock.connected = true;

    SECTION("Same location sent multiple times only sends once")
    {
        // Send same item pickup multiple times
        monitor.onItemPickup(0x79, 1);
        monitor.onItemPickup(0x79, 1);
        monitor.onItemPickup(0x79, 1);

        REQUIRE(mock.sentLocations.size() == 1);
        REQUIRE(monitor.getLocationsSentCount() == 1);
    }

    SECTION("Different quantities of same item only send once")
    {
        monitor.onItemPickup(42, 1);
        monitor.onItemPickup(42, 5);
        monitor.onItemPickup(42, 99);

        REQUIRE(mock.sentLocations.size() == 1);
        REQUIRE(monitor.getLocationsSentCount() == 1);
        REQUIRE(mock.sentLocations[0] == 100042);
    }
}

TEST_CASE("APLocationMonitor sending conditions", "[aplocation]")
{
    APLocationMonitor &monitor = APLocationMonitor::instance();
    MockSocket mock;

    SECTION("No locations sent when sending disabled")
    {
        monitor.reset();
        mock.reset();
        monitor.setSocket(&mock);
        monitor.enableSending(false); // Disabled
        mock.connected = true;

        monitor.onItemPickup(1, 1);
        monitor.onBrushAcquired(0);

        REQUIRE(mock.sentLocations.size() == 0);
        REQUIRE(monitor.getLocationsSentCount() == 0);
    }

    SECTION("No locations sent when socket not connected")
    {
        monitor.reset();
        mock.reset();
        monitor.setSocket(&mock);
        monitor.enableSending(true);
        mock.connected = false; // Not connected

        monitor.onItemPickup(1, 1);
        monitor.onBrushAcquired(0);

        REQUIRE(mock.sentLocations.size() == 0);
        REQUIRE(monitor.getLocationsSentCount() == 0);
    }

    SECTION("No locations sent when no socket set")
    {
        monitor.reset();
        mock.reset();
        monitor.setSocket(nullptr); // No socket
        monitor.enableSending(true);

        monitor.onItemPickup(1, 1);
        monitor.onBrushAcquired(0);

        REQUIRE(mock.sentLocations.size() == 0);
        REQUIRE(monitor.getLocationsSentCount() == 0);
    }

    SECTION("Locations sent when all conditions met")
    {
        monitor.reset();
        mock.reset();
        monitor.setSocket(&mock);
        monitor.enableSending(true);
        mock.connected = true;

        monitor.onItemPickup(1, 1);

        REQUIRE(mock.sentLocations.size() == 1);
        REQUIRE(monitor.getLocationsSentCount() == 1);
    }
}

TEST_CASE("APLocationMonitor reset functionality", "[aplocation]")
{
    APLocationMonitor &monitor = APLocationMonitor::instance();
    MockSocket mock;

    // Setup and send some locations
    monitor.reset();
    mock.reset();
    monitor.setSocket(&mock);
    monitor.enableSending(true);
    mock.connected = true;

    monitor.onItemPickup(1, 1);
    monitor.onBrushAcquired(0);
    monitor.onShopPurchase(1, 0, 99);

    REQUIRE(monitor.getLocationsSentCount() == 3);

    SECTION("Reset clears sent locations cache")
    {
        monitor.reset();

        REQUIRE(monitor.getLocationsSentCount() == 0);

        // Same locations should be sendable again
        monitor.setSocket(&mock);
        monitor.enableSending(true);
        mock.connected = true;
        mock.sentLocations.clear(); // Clear mock's tracking too

        monitor.onItemPickup(1, 1); // Same item as before

        REQUIRE(mock.sentLocations.size() == 1);
        REQUIRE(monitor.getLocationsSentCount() == 1);
    }
}

TEST_CASE("APLocationMonitor edge cases", "[aplocation]")
{
    APLocationMonitor &monitor = APLocationMonitor::instance();
    MockSocket mock;

    SECTION("Large location IDs are handled correctly")
    {
        monitor.reset();
        mock.reset();
        monitor.setSocket(&mock);
        monitor.enableSending(true);
        mock.connected = true;

        // Test with maximum map ID and bit index
        int64_t expectedId = APLocationMonitor::getWorldStateBitLocationId(82, 9999);
        REQUIRE(expectedId == 1229999); // 400000 + (82 * 10000) + 9999
    }

    SECTION("Zero values are handled correctly")
    {
        monitor.reset();
        mock.reset();
        monitor.setSocket(&mock);
        monitor.enableSending(true);
        mock.connected = true;

        monitor.onItemPickup(0, 0);
        monitor.onBrushAcquired(0);
        monitor.onShopPurchase(0, 0, 0);

        REQUIRE(mock.sentLocations.size() == 3);
        REQUIRE(mock.sentLocations[0] == 100000);
        REQUIRE(mock.sentLocations[1] == 200000);
        REQUIRE(mock.sentLocations[2] == 300000);
    }
}
