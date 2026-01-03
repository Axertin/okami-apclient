#include <catch2/catch_test_macros.hpp>
#include "aplocationmonitor.h"
#include "mocks/mock_archipelagosocket.h"

/**
 * Test suite for APLocationMonitor
 *
 * This suite tests the APLocationMonitor's location ID calculation functions,
 * duplicate tracking, state management, and bit change filtering.
 */

// Pure functions - Location ID calculations
TEST_CASE("APLocationMonitor location ID calculations", "[aplocationmonitor]")
{
    SECTION("getItemLocationId calculates correct ID")
    {
        REQUIRE(APLocationMonitor::getItemLocationId(0) == 100000);
        REQUIRE(APLocationMonitor::getItemLocationId(1) == 100001);
        REQUIRE(APLocationMonitor::getItemLocationId(42) == 100042);
        REQUIRE(APLocationMonitor::getItemLocationId(999) == 100999);
    }

    SECTION("getBrushLocationId calculates correct ID")
    {
        REQUIRE(APLocationMonitor::getBrushLocationId(0) == 200000);
        REQUIRE(APLocationMonitor::getBrushLocationId(1) == 200001);
        REQUIRE(APLocationMonitor::getBrushLocationId(13) == 200013);
        REQUIRE(APLocationMonitor::getBrushLocationId(50) == 200050);
    }

    SECTION("getShopLocationId calculates correct ID")
    {
        REQUIRE(APLocationMonitor::getShopLocationId(0, 0) == 300000);
        REQUIRE(APLocationMonitor::getShopLocationId(0, 1) == 300001);
        REQUIRE(APLocationMonitor::getShopLocationId(1, 0) == 301000);
        REQUIRE(APLocationMonitor::getShopLocationId(1, 5) == 301005);
        REQUIRE(APLocationMonitor::getShopLocationId(5, 10) == 305010);
        REQUIRE(APLocationMonitor::getShopLocationId(42, 99) == 342099);
    }

    SECTION("getWorldStateBitLocationId calculates correct ID")
    {
        REQUIRE(APLocationMonitor::getWorldStateBitLocationId(0, 0) == 400000);
        REQUIRE(APLocationMonitor::getWorldStateBitLocationId(0, 1) == 400001);
        REQUIRE(APLocationMonitor::getWorldStateBitLocationId(1, 0) == 410000);
        REQUIRE(APLocationMonitor::getWorldStateBitLocationId(1, 5) == 410005);
        REQUIRE(APLocationMonitor::getWorldStateBitLocationId(5, 100) == 450100);
        REQUIRE(APLocationMonitor::getWorldStateBitLocationId(42, 255) == 820255);
    }

    SECTION("getCollectedObjectLocationId calculates correct ID")
    {
        REQUIRE(APLocationMonitor::getCollectedObjectLocationId(0, 0) == 500000);
        REQUIRE(APLocationMonitor::getCollectedObjectLocationId(0, 1) == 500001);
        REQUIRE(APLocationMonitor::getCollectedObjectLocationId(1, 0) == 510000);
        REQUIRE(APLocationMonitor::getCollectedObjectLocationId(1, 5) == 510005);
        REQUIRE(APLocationMonitor::getCollectedObjectLocationId(5, 50) == 550050);
        REQUIRE(APLocationMonitor::getCollectedObjectLocationId(42, 95) == 920095);
    }

    SECTION("getAreasRestoredLocationId calculates correct ID")
    {
        REQUIRE(APLocationMonitor::getAreasRestoredLocationId(0, 0) == 600000);
        REQUIRE(APLocationMonitor::getAreasRestoredLocationId(0, 1) == 600001);
        REQUIRE(APLocationMonitor::getAreasRestoredLocationId(1, 0) == 610000);
        REQUIRE(APLocationMonitor::getAreasRestoredLocationId(1, 5) == 610005);
        REQUIRE(APLocationMonitor::getAreasRestoredLocationId(5, 50) == 650050);
        REQUIRE(APLocationMonitor::getAreasRestoredLocationId(42, 95) == 1020095);
    }

    SECTION("getGlobalFlagLocationId calculates correct ID")
    {
        REQUIRE(APLocationMonitor::getGlobalFlagLocationId(0) == 700000);
        REQUIRE(APLocationMonitor::getGlobalFlagLocationId(1) == 700001);
        REQUIRE(APLocationMonitor::getGlobalFlagLocationId(42) == 700042);
        REQUIRE(APLocationMonitor::getGlobalFlagLocationId(85) == 700085);
    }

    SECTION("getGameProgressLocationId calculates correct ID")
    {
        REQUIRE(APLocationMonitor::getGameProgressLocationId(0) == 800000);
        REQUIRE(APLocationMonitor::getGameProgressLocationId(1) == 800001);
        REQUIRE(APLocationMonitor::getGameProgressLocationId(42) == 800042);
        REQUIRE(APLocationMonitor::getGameProgressLocationId(95) == 800095);
    }
}

// Stateful operations - Duplicate tracking
TEST_CASE("APLocationMonitor duplicate tracking", "[aplocationmonitor]")
{
    auto& monitor = APLocationMonitor::instance();
    mock::MockArchipelagoSocket mockSocket;

    // Set up monitor with mock socket and enable sending
    mockSocket.setConnected(true);
    monitor.setSocket(&mockSocket);
    monitor.enableSending(true);

    SECTION("Initially no locations have been sent")
    {
        monitor.reset();
        REQUIRE(monitor.getLocationsSentCount() == 0);
    }

    SECTION("Brush acquisition sends location once")
    {
        monitor.reset();
        mockSocket.clearSentLocations();

        // First brush acquisition should send
        monitor.onBrushAcquired(1);
        REQUIRE(mockSocket.getSentLocationCount() == 1);
        REQUIRE(mockSocket.wasLocationSent(200001));

        // Second identical brush acquisition should not send
        monitor.onBrushAcquired(1);
        REQUIRE(mockSocket.getSentLocationCount() == 1);
    }

    SECTION("Shop purchase sends location once")
    {
        monitor.reset();
        mockSocket.clearSentLocations();

        // First shop purchase should send
        monitor.onShopPurchase(5, 3, 42);
        REQUIRE(mockSocket.getSentLocationCount() == 1);
        REQUIRE(mockSocket.wasLocationSent(305003));

        // Second identical shop purchase should not send
        monitor.onShopPurchase(5, 3, 100);  // Different itemId, same shop+slot
        REQUIRE(mockSocket.getSentLocationCount() == 1);
    }

    SECTION("Multiple different locations are tracked separately")
    {
        monitor.reset();
        mockSocket.clearSentLocations();

        monitor.onBrushAcquired(1);
        monitor.onBrushAcquired(2);
        monitor.onBrushAcquired(3);
        monitor.onShopPurchase(0, 0, 10);

        REQUIRE(mockSocket.getSentLocationCount() == 4);
        REQUIRE(mockSocket.wasLocationSent(200001));
        REQUIRE(mockSocket.wasLocationSent(200002));
        REQUIRE(mockSocket.wasLocationSent(200003));
        REQUIRE(mockSocket.wasLocationSent(300000));
    }

    SECTION("Reset clears sent locations cache")
    {
        monitor.reset();
        mockSocket.clearSentLocations();

        // Send some locations
        monitor.onBrushAcquired(5);
        REQUIRE(mockSocket.getSentLocationCount() == 1);

        // Reset and send again
        monitor.reset();
        mockSocket.clearSentLocations();
        monitor.onBrushAcquired(5);
        REQUIRE(mockSocket.getSentLocationCount() == 1);  // Should send again after reset
    }
}

// State management
TEST_CASE("APLocationMonitor state management", "[aplocationmonitor]")
{
    auto& monitor = APLocationMonitor::instance();
    mock::MockArchipelagoSocket mockSocket;

    mockSocket.setConnected(true);
    monitor.setSocket(&mockSocket);

    SECTION("Sending can be enabled and disabled")
    {
        monitor.enableSending(false);
        REQUIRE_FALSE(monitor.isSendingEnabled());

        monitor.enableSending(true);
        REQUIRE(monitor.isSendingEnabled());
    }

    SECTION("Locations are not sent when sending is disabled")
    {
        monitor.reset();
        mockSocket.clearSentLocations();
        monitor.enableSending(false);

        monitor.onBrushAcquired(10);
        REQUIRE(mockSocket.getSentLocationCount() == 0);
    }

    SECTION("Locations are sent when sending is enabled")
    {
        monitor.reset();
        mockSocket.clearSentLocations();
        monitor.enableSending(true);

        monitor.onBrushAcquired(10);
        REQUIRE(mockSocket.getSentLocationCount() == 1);
    }

    SECTION("Locations are not sent when socket is disconnected")
    {
        monitor.reset();
        mockSocket.clearSentLocations();
        mockSocket.setConnected(false);
        monitor.enableSending(true);

        monitor.onBrushAcquired(10);
        REQUIRE(mockSocket.getSentLocationCount() == 0);
    }

    SECTION("Locations are sent when socket is connected")
    {
        monitor.reset();
        mockSocket.clearSentLocations();
        mockSocket.setConnected(true);
        monitor.enableSending(true);

        monitor.onBrushAcquired(10);
        REQUIRE(mockSocket.getSentLocationCount() == 1);
    }
}
