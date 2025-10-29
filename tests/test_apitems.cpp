#include <catch2/catch_test_macros.hpp>

#include "apitems_generated.hpp"

TEST_CASE("APItems registry lookup", "[apitems]")
{
    SECTION("Can find valid brush item")
    {
        const auto *item = apitems::findItem(0x100); // Sunrise
        REQUIRE(item != nullptr);
        CHECK(item->type == apitems::ItemHandlerType::Brush);
        CHECK(std::string(item->name) == "Sunrise");
        CHECK(item->brush.brush_index == 0);
    }

    SECTION("Can find progressive brush item")
    {
        const auto *item = apitems::findItem(0x102); // Power Slash
        REQUIRE(item != nullptr);
        CHECK(item->type == apitems::ItemHandlerType::ProgressiveBrush);
        CHECK(std::string(item->name) == "Power Slash");
        CHECK(item->progressive_brush.brush_index == 2);
        CHECK(item->progressive_brush.upgrade_count == 2);
    }

    SECTION("Can find inventory item")
    {
        const auto *item = apitems::findItem(0x42); // Canine Tracker
        REQUIRE(item != nullptr);
        CHECK(item->type == apitems::ItemHandlerType::Inventory);
        CHECK(std::string(item->name) == "Canine Tracker");
        CHECK(item->inventory.item_id == 0x42);
    }

    SECTION("Can find progressive weapon")
    {
        const auto *item = apitems::findItem(0x300); // Progressive Mirror
        REQUIRE(item != nullptr);
        CHECK(item->type == apitems::ItemHandlerType::Progressive);
        CHECK(std::string(item->name) == "Progressive Mirror");
        CHECK(item->progressive.stage_count == 2);
        CHECK(item->progressive.stages[0] == 0x13);
        CHECK(item->progressive.stages[1] == 0x14);
    }

    SECTION("Returns nullptr for invalid item ID")
    {
        const auto *item = apitems::findItem(0xFFFF);
        CHECK(item == nullptr);
    }
}

TEST_CASE("APItems registry contains all expected items", "[apitems]")
{
    SECTION("Has brush techniques")
    {
        CHECK(apitems::findItem(0x100) != nullptr); // Sunrise
        CHECK(apitems::findItem(0x101) != nullptr); // Rejuvenation
        CHECK(apitems::findItem(0x102) != nullptr); // Power Slash
        CHECK(apitems::findItem(0x103) != nullptr); // Cherry Bomb
    }

    SECTION("Has progressive weapons")
    {
        CHECK(apitems::findItem(0x300) != nullptr); // Progressive Mirror
        CHECK(apitems::findItem(0x301) != nullptr); // Progressive Rosary
        CHECK(apitems::findItem(0x302) != nullptr); // Progressive Sword
    }

    SECTION("Registry is not empty")
    {
        CHECK(apitems::item_registry.size() > 0);
    }
}
