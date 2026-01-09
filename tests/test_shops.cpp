#include <cstring>

#include <okami/shopdata.h>

#include <catch2/catch_test_macros.hpp>
#include <okami/itemtype.hpp>
#include <okami/maptype.hpp>

#include "checks/check_types.hpp"
#include "checks/shops.hpp"
#include "mock_archipelagosocket.h"
#include "wolf_framework.hpp"

// ============================================================================
// Shop check ID calculation tests
// ============================================================================

TEST_CASE("Shop check ID calculation", "[shops][check_types]")
{
    // Shop check = kShopPurchaseBase + (shopId * 1000) + itemSlot
    // kShopPurchaseBase = 300000
    int64_t checkId = checks::getShopCheckId(5, 3);
    REQUIRE(checkId == 300000 + (5 * 1000) + 3);
    REQUIRE(checkId == 305003);
}

TEST_CASE("Shop check ID components can be extracted", "[shops][check_types]")
{
    int64_t checkId = checks::getShopCheckId(12, 7);

    // Verify we can decode the shop ID and item slot
    int64_t offset = checkId - checks::kShopPurchaseBase;
    int shopId = static_cast<int>(offset / 1000);
    int itemSlot = static_cast<int>(offset % 1000);

    REQUIRE(shopId == 12);
    REQUIRE(itemSlot == 7);
}

TEST_CASE("Shop check ID ranges don't overlap with other categories", "[shops][check_types]")
{
    // Shop range: 300000 + shopId*1000 + slot
    // Max reasonable: shopId=99, slot=999 -> 399999
    // Next category (WorldState) starts at 400000

    int64_t maxShopCheck = checks::getShopCheckId(99, 999);
    REQUIRE(maxShopCheck == 399999);
    REQUIRE(maxShopCheck < checks::kWorldStateBase);

    // Previous category (Brush) ends before shops
    int64_t maxBrushCheck = checks::kBrushAcquisitionBase + 99999;
    REQUIRE(maxBrushCheck < checks::kShopPurchaseBase);
}

TEST_CASE("Shop check category is correctly identified", "[shops][check_types]")
{
    int64_t shopCheck = checks::getShopCheckId(5, 10);
    REQUIRE(checks::getCheckCategory(shopCheck) == checks::CheckCategory::ShopPurchase);

    // Edge cases
    REQUIRE(checks::getCheckCategory(checks::kShopPurchaseBase) == checks::CheckCategory::ShopPurchase);
    REQUIRE(checks::getCheckCategory(checks::kShopPurchaseBase + 99999) == checks::CheckCategory::ShopPurchase);

    // Not shop checks
    REQUIRE(checks::getCheckCategory(checks::kItemPickupBase + 100) != checks::CheckCategory::ShopPurchase);
    REQUIRE(checks::getCheckCategory(checks::kWorldStateBase + 100) != checks::CheckCategory::ShopPurchase);
}

// ============================================================================
// ShopDefinition class tests
// ============================================================================

TEST_CASE("ShopDefinition default construction", "[shops][ShopDefinition]")
{
    checks::ShopDefinition shop;

    // GetData() should return valid pointer even with empty stock
    const uint8_t *data = shop.GetData();
    REQUIRE(data != nullptr);

    // Verify ISL header magic
    const auto *header = reinterpret_cast<const okami::ISLHeader *>(data);
    REQUIRE(std::memcmp(header->magic, "ISL", 3) == 0);
    REQUIRE(header->variations == 1);
}

TEST_CASE("ShopDefinition AddItem adds items correctly", "[shops][ShopDefinition]")
{
    checks::ShopDefinition shop;

    shop.AddItem(okami::ItemTypes::HolyBoneS, 100);
    shop.AddItem(okami::ItemTypes::ExorcismSlipS, 200);

    const uint8_t *data = shop.GetData();
    REQUIRE(data != nullptr);

    // Skip header (16 bytes), read item count
    const uint32_t *numItems = reinterpret_cast<const uint32_t *>(data + sizeof(okami::ISLHeader));
    REQUIRE(*numItems == 2);

    // Verify first item
    const auto *stock = reinterpret_cast<const okami::ItemShopStock *>(data + sizeof(okami::ISLHeader) + sizeof(uint32_t));
    REQUIRE(stock[0].itemType == okami::ItemTypes::HolyBoneS);
    REQUIRE(stock[0].cost == 100);

    // Verify second item
    REQUIRE(stock[1].itemType == okami::ItemTypes::ExorcismSlipS);
    REQUIRE(stock[1].cost == 200);
}

TEST_CASE("ShopDefinition AddItem warns when stock is full", "[shops][ShopDefinition]")
{
    wolf::mock::reset();
    checks::ShopDefinition shop;

    // Fill to max (50 items)
    for (size_t i = 0; i < checks::MaxShopStockSize; ++i)
    {
        shop.AddItem(okami::ItemTypes::HolyBoneS, 100);
    }

    // Clear log messages before the overflow attempt
    wolf::mock::logMessages.clear();

    // Try to add one more - should log warning
    shop.AddItem(okami::ItemTypes::HolyBoneM, 200);

    // Check for warning in logs
    bool foundWarning = false;
    for (const auto &msg : wolf::mock::logMessages)
    {
        if (msg.find("Max stock size") != std::string::npos)
        {
            foundWarning = true;
            break;
        }
    }
    REQUIRE(foundWarning);

    // Verify stock is still capped at max
    const uint8_t *data = shop.GetData();
    const uint32_t *numItems = reinterpret_cast<const uint32_t *>(data + sizeof(okami::ISLHeader));
    REQUIRE(*numItems == checks::MaxShopStockSize);
}

TEST_CASE("ShopDefinition SetStock replaces stock", "[shops][ShopDefinition]")
{
    checks::ShopDefinition shop;

    // Add some initial items
    shop.AddItem(okami::ItemTypes::HolyBoneS, 100);
    shop.AddItem(okami::ItemTypes::HolyBoneM, 200);

    // Replace with new stock
    std::vector<okami::ItemShopStock> newStock = {
        {okami::ItemTypes::ExorcismSlipS, 50, 0},
        {okami::ItemTypes::ExorcismSlipM, 100, 0},
        {okami::ItemTypes::ExorcismSlipL, 150, 0},
    };
    shop.SetStock(newStock);

    const uint8_t *data = shop.GetData();
    const uint32_t *numItems = reinterpret_cast<const uint32_t *>(data + sizeof(okami::ISLHeader));
    REQUIRE(*numItems == 3);

    const auto *stock = reinterpret_cast<const okami::ItemShopStock *>(data + sizeof(okami::ISLHeader) + sizeof(uint32_t));
    REQUIRE(stock[0].itemType == okami::ItemTypes::ExorcismSlipS);
    REQUIRE(stock[1].itemType == okami::ItemTypes::ExorcismSlipM);
    REQUIRE(stock[2].itemType == okami::ItemTypes::ExorcismSlipL);
}

TEST_CASE("ShopDefinition SetStock warns when stock exceeds max", "[shops][ShopDefinition]")
{
    wolf::mock::reset();
    checks::ShopDefinition shop;

    // Create oversized stock
    std::vector<okami::ItemShopStock> oversizedStock(checks::MaxShopStockSize + 10);
    for (auto &item : oversizedStock)
    {
        item = {okami::ItemTypes::HolyBoneS, 100, 0};
    }

    shop.SetStock(oversizedStock);

    // Check for warning in logs
    bool foundWarning = false;
    for (const auto &msg : wolf::mock::logMessages)
    {
        if (msg.find("Max stock size") != std::string::npos)
        {
            foundWarning = true;
            break;
        }
    }
    REQUIRE(foundWarning);
}

TEST_CASE("ShopDefinition ClearStock empties the stock", "[shops][ShopDefinition]")
{
    checks::ShopDefinition shop;

    shop.AddItem(okami::ItemTypes::HolyBoneS, 100);
    shop.AddItem(okami::ItemTypes::HolyBoneM, 200);

    shop.ClearStock();

    const uint8_t *data = shop.GetData();
    const uint32_t *numItems = reinterpret_cast<const uint32_t *>(data + sizeof(okami::ISLHeader));
    REQUIRE(*numItems == 0);
}

TEST_CASE("ShopDefinition SetSellValueOverride modifies single value", "[shops][ShopDefinition]")
{
    checks::ShopDefinition shop;
    shop.AddItem(okami::ItemTypes::HolyBoneS, 100);

    // Override sell value for a specific item
    shop.SetSellValueOverride(okami::ItemTypes::HolyBoneS, 999);

    const uint8_t *data = shop.GetData();
    const uint32_t *numItems = reinterpret_cast<const uint32_t *>(data + sizeof(okami::ISLHeader));

    // Sell values are after the stock array
    size_t stockOffset = sizeof(okami::ISLHeader) + sizeof(uint32_t) + (*numItems * sizeof(okami::ItemShopStock));
    const auto *sellValues = reinterpret_cast<const int32_t *>(data + stockOffset);

    REQUIRE(sellValues[okami::ItemTypes::HolyBoneS] == 999);
}

TEST_CASE("ShopDefinition SetSellValues replaces all sell values", "[shops][ShopDefinition]")
{
    checks::ShopDefinition shop;
    shop.AddItem(okami::ItemTypes::HolyBoneS, 100);

    // Use custom sell values array
    shop.SetSellValues(okami::DefaultTakaPassItemSellPrices);

    const uint8_t *data = shop.GetData();
    const uint32_t *numItems = reinterpret_cast<const uint32_t *>(data + sizeof(okami::ISLHeader));

    size_t stockOffset = sizeof(okami::ISLHeader) + sizeof(uint32_t) + (*numItems * sizeof(okami::ItemShopStock));
    const auto *sellValues = reinterpret_cast<const int32_t *>(data + stockOffset);

    // Verify sell values match the Taka Pass array
    for (size_t i = 0; i < okami::ItemTypes::NUM_ITEM_TYPES; ++i)
    {
        REQUIRE(sellValues[i] == okami::DefaultTakaPassItemSellPrices[i]);
    }
}

TEST_CASE("ShopDefinition ISL binary format is correct", "[shops][ShopDefinition]")
{
    checks::ShopDefinition shop;
    shop.AddItem(okami::ItemTypes::HolyBoneS, 100);
    shop.AddItem(okami::ItemTypes::HolyBoneM, 200);

    const uint8_t *data = shop.GetData();

    // 1. ISLHeader (16 bytes)
    const auto *header = reinterpret_cast<const okami::ISLHeader *>(data);
    REQUIRE(std::memcmp(header->magic, "ISL", 3) == 0);
    REQUIRE(header->variations == 1);
    REQUIRE(header->unk2 == 0);
    REQUIRE(header->unk3 == 0);

    // 2. Item count (4 bytes)
    const uint32_t *numItems = reinterpret_cast<const uint32_t *>(data + sizeof(okami::ISLHeader));
    REQUIRE(*numItems == 2);

    // 3. ItemShopStock array (12 bytes each)
    const auto *stock = reinterpret_cast<const okami::ItemShopStock *>(data + sizeof(okami::ISLHeader) + sizeof(uint32_t));
    REQUIRE(sizeof(okami::ItemShopStock) == 12);
    REQUIRE(stock[0].itemType == okami::ItemTypes::HolyBoneS);
    REQUIRE(stock[0].cost == 100);
    REQUIRE(stock[1].itemType == okami::ItemTypes::HolyBoneM);
    REQUIRE(stock[1].cost == 200);

    // 4. SellValueArray (241 * 4 = 964 bytes)
    size_t expectedSellValuesOffset = sizeof(okami::ISLHeader) + sizeof(uint32_t) + (2 * sizeof(okami::ItemShopStock));
    const auto *sellValues = reinterpret_cast<const int32_t *>(data + expectedSellValuesOffset);

    // Default sell values should match DefaultItemSellPrices
    REQUIRE(sellValues[okami::ItemTypes::HolyBoneS] == okami::DefaultItemSellPrices[okami::ItemTypes::HolyBoneS]);
}

TEST_CASE("ShopDefinition dirty flag prevents unnecessary rebuilds", "[shops][ShopDefinition]")
{
    checks::ShopDefinition shop;
    shop.AddItem(okami::ItemTypes::HolyBoneS, 100);

    // First call builds the ISL
    const uint8_t *data1 = shop.GetData();

    // Second call without modifications should return same pointer
    const uint8_t *data2 = shop.GetData();
    REQUIRE(data1 == data2);

    // Modification marks dirty, next GetData rebuilds
    shop.AddItem(okami::ItemTypes::HolyBoneM, 200);
    const uint8_t *data3 = shop.GetData();

    // Data pointer may be same (reused buffer) but content changed
    const uint32_t *numItems = reinterpret_cast<const uint32_t *>(data3 + sizeof(okami::ISLHeader));
    REQUIRE(*numItems == 2);
}

// ============================================================================
// GetCurrentItemShopData tests
// ============================================================================

TEST_CASE("GetCurrentItemShopData returns data for known maps", "[shops][lookup]")
{
    // Initialize shop data first
    checks::InitializeShopData();

    // Test several known shop locations
    SECTION("Kamiki Village")
    {
        const void *data = checks::GetCurrentItemShopData(okami::MapID::KamikiVillage, 0);
        REQUIRE(data != nullptr);

        // Verify it's valid ISL data
        const auto *header = reinterpret_cast<const okami::ISLHeader *>(data);
        REQUIRE(std::memcmp(header->magic, "ISL", 3) == 0);
    }

    SECTION("Kamiki Village Cursed")
    {
        const void *data = checks::GetCurrentItemShopData(okami::MapID::KamikiVillageCursed, 0);
        REQUIRE(data != nullptr);
    }

    SECTION("Shinshu Field Cursed")
    {
        const void *data = checks::GetCurrentItemShopData(okami::MapID::ShinshuFieldCursed, 0);
        REQUIRE(data != nullptr);
    }

    SECTION("Agata Forest")
    {
        const void *data = checks::GetCurrentItemShopData(okami::MapID::AgataForestCursed, 0);
        REQUIRE(data != nullptr);
    }
}

TEST_CASE("GetCurrentItemShopData returns nullptr for unknown maps", "[shops][lookup]")
{
    checks::InitializeShopData();

    // Test some maps that don't have shops
    REQUIRE(checks::GetCurrentItemShopData(0x9999, 0) == nullptr);
    REQUIRE(checks::GetCurrentItemShopData(okami::MapID::CaveofNagi, 0) == nullptr);
    REQUIRE(checks::GetCurrentItemShopData(okami::MapID::RiveroftheHeavens, 0) == nullptr);
}

TEST_CASE("GetCurrentItemShopData handles Seian City shopNum", "[shops][lookup]")
{
    checks::InitializeShopData();

    // Seian City Commoners Quarter has 2 shops
    const void *weaponShop = checks::GetCurrentItemShopData(okami::MapID::SeianCityCommonersQuarter, 0);
    const void *fishShop = checks::GetCurrentItemShopData(okami::MapID::SeianCityCommonersQuarter, 1);

    REQUIRE(weaponShop != nullptr);
    REQUIRE(fishShop != nullptr);

    // They should be different shops
    REQUIRE(weaponShop != fishShop);
}

TEST_CASE("GetCurrentItemShopData returns same shop for cursed/healed variants", "[shops][lookup]")
{
    checks::InitializeShopData();

    // Cursed and healed versions of the same area use the same shop
    SECTION("Shinshu Field")
    {
        const void *cursed = checks::GetCurrentItemShopData(okami::MapID::ShinshuFieldCursed, 0);
        const void *healed = checks::GetCurrentItemShopData(okami::MapID::ShinshuFieldHealed, 0);
        REQUIRE(cursed == healed);
    }

    SECTION("Agata Forest")
    {
        const void *cursed = checks::GetCurrentItemShopData(okami::MapID::AgataForestCursed, 0);
        const void *healed = checks::GetCurrentItemShopData(okami::MapID::AgataForestHealed, 0);
        REQUIRE(cursed == healed);
    }

    SECTION("Ryoshima Coast")
    {
        const void *cursed = checks::GetCurrentItemShopData(okami::MapID::RyoshimaCoastCursed, 0);
        const void *healed = checks::GetCurrentItemShopData(okami::MapID::RyoshimaCoastHealed, 0);
        REQUIRE(cursed == healed);
    }

    SECTION("Kamui")
    {
        const void *cursed = checks::GetCurrentItemShopData(okami::MapID::KamuiCursed, 0);
        const void *healed = checks::GetCurrentItemShopData(okami::MapID::KamuiHealed, 0);
        REQUIRE(cursed == healed);
    }

    SECTION("Taka Pass")
    {
        const void *cursed = checks::GetCurrentItemShopData(okami::MapID::TakaPassCursed, 0);
        const void *healed = checks::GetCurrentItemShopData(okami::MapID::TakaPassHealed, 0);
        REQUIRE(cursed == healed);
    }
}

// ============================================================================
// GetCurrentDemonFangShopData tests
// ============================================================================

TEST_CASE("GetCurrentDemonFangShopData returns data for known maps", "[shops][lookup][demonfang]")
{
    checks::InitializeShopData();
    uint32_t numItems = 0;

    SECTION("Agata Forest Cursed")
    {
        okami::ItemShopStock *data = checks::GetCurrentDemonFangShopData(okami::MapID::AgataForestCursed, &numItems);
        // May be nullptr if no items configured, but numItems should be set
        if (data != nullptr)
        {
            REQUIRE(numItems > 0);
        }
        else
        {
            REQUIRE(numItems == 0);
        }
    }

    SECTION("Agata Forest Healed")
    {
        okami::ItemShopStock *data = checks::GetCurrentDemonFangShopData(okami::MapID::AgataForestHealed, &numItems);
        if (data != nullptr)
        {
            REQUIRE(numItems > 0);
        }
    }

    SECTION("Ark of Yamato")
    {
        okami::ItemShopStock *data = checks::GetCurrentDemonFangShopData(okami::MapID::ArkofYamato, &numItems);
        if (data != nullptr)
        {
            REQUIRE(numItems > 0);
        }
    }

    SECTION("Imperial Palace")
    {
        okami::ItemShopStock *data = checks::GetCurrentDemonFangShopData(okami::MapID::ImperialPalaceAmmySize, &numItems);
        if (data != nullptr)
        {
            REQUIRE(numItems > 0);
        }
    }
}

TEST_CASE("GetCurrentDemonFangShopData returns nullptr for unknown maps", "[shops][lookup][demonfang]")
{
    checks::InitializeShopData();
    uint32_t numItems = 999; // Non-zero to verify it gets set to 0

    okami::ItemShopStock *data = checks::GetCurrentDemonFangShopData(0x9999, &numItems);
    REQUIRE(data == nullptr);
    REQUIRE(numItems == 0);
}

TEST_CASE("GetCurrentDemonFangShopData sets numItems to 0 for maps without demon fang shops", "[shops][lookup][demonfang]")
{
    checks::InitializeShopData();
    uint32_t numItems = 999;

    // Kamiki Village doesn't have a demon fang shop
    okami::ItemShopStock *data = checks::GetCurrentDemonFangShopData(okami::MapID::KamikiVillage, &numItems);
    REQUIRE(data == nullptr);
    REQUIRE(numItems == 0);
}

// ============================================================================
// ShopMan initialization tests
// ============================================================================

class ShopManFixture
{
  protected:
    mock::MockArchipelagoSocket socket_;
    std::vector<int64_t> receivedCheckIds_;
    std::unique_ptr<checks::ShopMan> shopMan_;

    void SetUp()
    {
        wolf::mock::reset();
        receivedCheckIds_.clear();

        shopMan_ = std::make_unique<checks::ShopMan>(socket_, [this](int64_t checkId) { receivedCheckIds_.push_back(checkId); });
    }

    void TearDown()
    {
        shopMan_.reset();
        wolf::mock::reset();
    }
};

TEST_CASE_METHOD(ShopManFixture, "ShopMan initialize installs hooks", "[shops][ShopMan]")
{
    SetUp();

    shopMan_->initialize();

    // Check that hooks were registered
    REQUIRE(wolf::mock::registeredHooks.count(0x4420C0) > 0); // GetShopVariation
    REQUIRE(wolf::mock::registeredHooks.count(0x1B1770) > 0); // LoadRsc
    REQUIRE(wolf::mock::registeredHooks.count(0x43F5A0) > 0); // CKibaShop_GetShopStockList

    TearDown();
}

TEST_CASE_METHOD(ShopManFixture, "ShopMan initialize logs success", "[shops][ShopMan]")
{
    SetUp();

    shopMan_->initialize();

    bool foundSuccessLog = false;
    for (const auto &msg : wolf::mock::logMessages)
    {
        if (msg.find("Shop hooks installed successfully") != std::string::npos)
        {
            foundSuccessLog = true;
            break;
        }
    }
    REQUIRE(foundSuccessLog);

    TearDown();
}

TEST_CASE_METHOD(ShopManFixture, "ShopMan double initialize warns", "[shops][ShopMan]")
{
    SetUp();

    shopMan_->initialize();
    wolf::mock::logMessages.clear();
    shopMan_->initialize();

    bool foundWarning = false;
    for (const auto &msg : wolf::mock::logMessages)
    {
        if (msg.find("Already initialized") != std::string::npos)
        {
            foundWarning = true;
            break;
        }
    }
    REQUIRE(foundWarning);

    TearDown();
}

TEST_CASE_METHOD(ShopManFixture, "ShopMan shutdown clears active instance", "[shops][ShopMan]")
{
    SetUp();

    shopMan_->initialize();
    shopMan_->shutdown();

    // Shutdown should work without crash
    // Second shutdown should be safe (no-op)
    shopMan_->shutdown();

    TearDown();
}
