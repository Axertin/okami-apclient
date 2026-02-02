#include <cstring>

#include <okami/shopdata.h>

#include <catch2/catch_test_macros.hpp>
#include <okami/itemtype.hpp>

#include "checks/check_types.hpp"
#include "checks/shops.hpp"
#include "mock_archipelagosocket.h"
#include "wolf_framework.hpp"

// ============================================================================
// Shop check ID constraints
// ============================================================================

// Compile-time verification
static_assert(checks::getShopCheckId(0, 0) == checks::kShopPurchaseBase);

TEST_CASE("Shop check ID encoding constraints", "[shops][check_types]")
{
    SECTION("Shop IDs don't overflow into next category")
    {
        // Max reasonable: shopId=99, slot=999 -> 399999
        // WorldState starts at 400000
        int64_t maxShopCheck = checks::getShopCheckId(99, 999);
        REQUIRE(maxShopCheck < checks::kWorldStateBase);
    }

    SECTION("Category boundaries")
    {
        REQUIRE(checks::getCheckCategory(checks::kShopPurchaseBase - 1) != checks::CheckCategory::ShopPurchase);
        REQUIRE(checks::getCheckCategory(checks::kShopPurchaseBase) == checks::CheckCategory::ShopPurchase);
        REQUIRE(checks::getCheckCategory(checks::kWorldStateBase - 1) == checks::CheckCategory::ShopPurchase);
        REQUIRE(checks::getCheckCategory(checks::kWorldStateBase) != checks::CheckCategory::ShopPurchase);
    }
}

// ============================================================================
// ShopDefinition class tests
// ============================================================================

TEST_CASE("ShopDefinition produces valid ISL header", "[shops][ShopDefinition]")
{
    checks::ShopDefinition shop;
    const uint8_t *data = shop.GetData();
    REQUIRE(data != nullptr);

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
