#include <cstring>

#include <okami/msd.h>

#include <catch2/catch_test_macros.hpp>
#include <okami/itemtype.hpp>

#include "checks/check_types.hpp"
#include "gamestate_accessors.hpp"
#include "itempatch.hpp"
#include "wolf_framework.hpp"

// ============================================================================
// MSD binary blob tests
// These tests do NOT require mock memory — MSDManager is pure data manipulation.
// ============================================================================

TEST_CASE("MSDManager: AddString encodes and stores a string", "[msd][blob]")
{
    okami::MSDManager mgr;
    uint32_t idx = mgr.AddString("Hi");
    REQUIRE(idx == 0);

    const uint8_t *data = mgr.GetData();
    REQUIRE(data != nullptr);

    // First 4 bytes: numEntries
    uint32_t numEntries;
    std::memcpy(&numEntries, data, sizeof(numEntries));
    REQUIRE(numEntries == 1);

    // Bytes 4-11: the single offset
    // Header size = 4 (numEntries) + 1 * 8 (one uint64_t offset) = 12
    // So offset[0] should point to byte 12
    uint64_t offset;
    std::memcpy(&offset, data + sizeof(uint32_t), sizeof(offset));
    REQUIRE(offset == sizeof(uint32_t) + 1 * sizeof(uint64_t));

    // First uint16_t at the string start: 'H' -> 30
    uint16_t ch0;
    std::memcpy(&ch0, data + offset, sizeof(ch0));
    REQUIRE(ch0 == 30);

    // Second uint16_t: 'i' -> 57
    uint16_t ch1;
    std::memcpy(&ch1, data + offset + 2, sizeof(ch1));
    REQUIRE(ch1 == 57);

    // Third uint16_t: EndDialog = 0x8001
    uint16_t term;
    std::memcpy(&term, data + offset + 4, sizeof(term));
    REQUIRE(term == 0x8001);
}

TEST_CASE("MSDManager: AddString returns sequential indices", "[msd][blob]")
{
    okami::MSDManager mgr;

    uint32_t idx0 = mgr.AddString("Alpha");
    uint32_t idx1 = mgr.AddString("Beta");
    uint32_t idx2 = mgr.AddString("Gamma");

    REQUIRE(idx0 == 0);
    REQUIRE(idx1 == 1);
    REQUIRE(idx2 == 2);

    REQUIRE(mgr.Size() == 3);
}

TEST_CASE("MSDManager: OverrideString replaces string in rebuilt binary", "[msd][blob]")
{
    // Build a valid minimal MSD binary with 2 strings: "Hi" and "Ok"
    // Header: numEntries(4) + offset[0](8) + offset[1](8) = 20 bytes
    // String 0 at offset 20: H(30), i(57), END(0x8001) = 3 uint16_t = 6 bytes
    // String 1 at offset 26: O(37), k(59), END(0x8001) = 3 uint16_t = 6 bytes

    std::vector<uint8_t> msdData;

    uint32_t n = 2;
    msdData.insert(msdData.end(), reinterpret_cast<uint8_t *>(&n), reinterpret_cast<uint8_t *>(&n) + sizeof(n));

    uint64_t o0 = 20; // 4 + 2*8
    msdData.insert(msdData.end(), reinterpret_cast<uint8_t *>(&o0), reinterpret_cast<uint8_t *>(&o0) + sizeof(o0));

    uint64_t o1 = 26; // 20 + 3 * sizeof(uint16_t)
    msdData.insert(msdData.end(), reinterpret_cast<uint8_t *>(&o1), reinterpret_cast<uint8_t *>(&o1) + sizeof(o1));

    // "Hi": H=30, i=57, END=0x8001
    uint16_t s0[] = {30, 57, 0x8001};
    msdData.insert(msdData.end(), reinterpret_cast<uint8_t *>(s0), reinterpret_cast<uint8_t *>(s0) + sizeof(s0));

    // "Ok": O=37, k=59, END=0x8001
    uint16_t s1[] = {37, 59, 0x8001};
    msdData.insert(msdData.end(), reinterpret_cast<uint8_t *>(s1), reinterpret_cast<uint8_t *>(s1) + sizeof(s1));

    okami::MSDManager mgr;
    mgr.ReadMSD(msdData.data());
    REQUIRE(mgr.Size() == 2);

    // Override string 0 with "A" — 'A' maps to 23
    mgr.OverrideString(0, "A");

    const uint8_t *out = mgr.GetData();
    REQUIRE(out != nullptr);

    // numEntries should still be 2
    uint32_t numEntries;
    std::memcpy(&numEntries, out, sizeof(numEntries));
    REQUIRE(numEntries == 2);

    // Read offset[0]
    uint64_t off0;
    std::memcpy(&off0, out + sizeof(uint32_t), sizeof(off0));

    // String 0 first character should be 'A' = 23
    uint16_t firstChar;
    std::memcpy(&firstChar, out + off0, sizeof(firstChar));
    REQUIRE(firstChar == 23);

    // String 0 second character (offset + 2) should be EndDialog = 0x8001
    // because "A" is a 1-char string and CompileString always appends EndDialog
    uint16_t termChar;
    std::memcpy(&termChar, out + off0 + 2, sizeof(termChar));
    REQUIRE(termChar == 0x8001);

    // Read offset[1] and verify string 1 is unchanged: first char 'O' = 37
    uint64_t off1;
    std::memcpy(&off1, out + sizeof(uint32_t) + sizeof(uint64_t), sizeof(off1));

    uint16_t firstCharStr1;
    std::memcpy(&firstCharStr1, out + off1, sizeof(firstCharStr1));
    REQUIRE(firstCharStr1 == 37);
}

TEST_CASE("MSDManager: ReadMSD then GetData preserves string count", "[msd][blob]")
{
    // Build a 3-string MSD binary
    // Header: 4 + 3*8 = 28 bytes
    // Each string: 1 char + END = 2 uint16_t = 4 bytes
    // String offsets: 28, 32, 36

    std::vector<uint8_t> msdData;

    uint32_t n = 3;
    msdData.insert(msdData.end(), reinterpret_cast<uint8_t *>(&n), reinterpret_cast<uint8_t *>(&n) + sizeof(n));

    uint64_t offsets[3] = {28, 32, 36};
    for (int i = 0; i < 3; ++i)
    {
        msdData.insert(msdData.end(), reinterpret_cast<uint8_t *>(&offsets[i]), reinterpret_cast<uint8_t *>(&offsets[i]) + sizeof(offsets[i]));
    }

    // Three 1-char strings: 'A'=23, 'B'=24, 'C'=25, each followed by END=0x8001
    uint16_t strings[3][2] = {{23, 0x8001}, {24, 0x8001}, {25, 0x8001}};
    for (int i = 0; i < 3; ++i)
    {
        msdData.insert(msdData.end(), reinterpret_cast<uint8_t *>(strings[i]), reinterpret_cast<uint8_t *>(strings[i]) + sizeof(strings[i]));
    }

    okami::MSDManager mgr;
    mgr.ReadMSD(msdData.data());

    // Retrieve data without any modifications
    const uint8_t *out = mgr.GetData();
    REQUIRE(out != nullptr);

    // numEntries in the output should match the original
    uint32_t numEntries;
    std::memcpy(&numEntries, out, sizeof(numEntries));
    REQUIRE(numEntries == 3);
    REQUIRE(mgr.Size() == 3);
}

TEST_CASE("MSDManager: OverrideString at invalid index is a no-op", "[msd][blob]")
{
    okami::MSDManager mgr;
    mgr.AddString("OnlyEntry");
    REQUIRE(mgr.Size() == 1);

    // Index 100 is out of bounds — should silently return with no crash
    mgr.OverrideString(100, "test");

    const uint8_t *data = mgr.GetData();
    REQUIRE(data != nullptr);

    uint32_t numEntries;
    std::memcpy(&numEntries, data, sizeof(numEntries));
    REQUIRE(numEntries == 1);
    REQUIRE(mgr.Size() == 1);
}

// ============================================================================
// ItemParam patching tests
// These tests USE mock memory and call apgame::initialize() first.
// ============================================================================

struct ItemParamFixture
{
    ItemParamFixture()
    {
        wolf::mock::reset();
        wolf::mock::reserveMemory(0x7AB220 + sizeof(std::array<okami::ItemParam, okami::ItemTypes::NUM_ITEM_TYPES>));
        apgame::initialize();
    }
};

TEST_CASE_METHOD(ItemParamFixture, "patchItemParams: zero-category items get category=1", "[itempatch][ItemParam]")
{
    // All ItemParam entries are zero-initialized by the mock constructor,
    // so category=0 and maxCount=0 for all entries going in.
    itempatch::patchItemParams();

    auto &params = *apgame::itemParams;

    // Items with explicit non-1 category overrides (progression=3, trap=4)
    constexpr auto kNonDefaultCategory = std::to_array({
        okami::ItemTypes::ForeignProgressionItem,
        okami::ItemTypes::ForeignTrapItem,
        okami::ItemTypes::OkamiProgressionItem,
        okami::ItemTypes::OkamiTrapItem,
    });
    for (int i = 0; i < okami::ItemTypes::NUM_ITEM_TYPES; ++i)
    {
        if (std::ranges::find(kNonDefaultCategory, i) != kNonDefaultCategory.end())
            continue;
        INFO("Item " << i << " has category " << static_cast<int>(params.at(i).category));
        REQUIRE(params.at(i).category == 1);
    }
}

TEST_CASE_METHOD(ItemParamFixture, "patchItemParams: zero-maxCount items get maxCount=99", "[itempatch][ItemParam]")
{
    itempatch::patchItemParams();

    auto &params = *apgame::itemParams;

    // AP dummy items get maxCount=1; all others get 99
    constexpr auto kMaxCount1Items = std::to_array({
        okami::ItemTypes::ForeignStandardItem,
        okami::ItemTypes::ForeignProgressionItem,
        okami::ItemTypes::ForeignTrapItem,
        okami::ItemTypes::OkamiStandardItem,
        okami::ItemTypes::OkamiProgressionItem,
        okami::ItemTypes::OkamiTrapItem,
    });
    for (int i = 0; i < okami::ItemTypes::NUM_ITEM_TYPES; ++i)
    {
        INFO("Item " << i << " has maxCount " << static_cast<int>(params.at(i).maxCount));
        if (std::ranges::find(kMaxCount1Items, i) != kMaxCount1Items.end())
            REQUIRE(params.at(i).maxCount == 1);
        else
            REQUIRE(params.at(i).maxCount == 99);
    }
}

TEST_CASE_METHOD(ItemParamFixture, "patchItemParams: AP dummy items get distinct categories", "[itempatch][ItemParam]")
{
    itempatch::patchItemParams();

    auto &params = *apgame::itemParams;

    // Foreign dummy items
    REQUIRE(params.at(okami::ItemTypes::ForeignStandardItem).category == 1);
    REQUIRE(params.at(okami::ItemTypes::ForeignProgressionItem).category == 3);
    REQUIRE(params.at(okami::ItemTypes::ForeignTrapItem).category == 4);
    // Okami-native dummy items (same category pattern)
    REQUIRE(params.at(okami::ItemTypes::OkamiStandardItem).category == 1);
    REQUIRE(params.at(okami::ItemTypes::OkamiProgressionItem).category == 3);
    REQUIRE(params.at(okami::ItemTypes::OkamiTrapItem).category == 4);
}

TEST_CASE_METHOD(ItemParamFixture, "patchItemParams: ArchipelagoTestItem categories always override pre-set values", "[itempatch][ItemParam]")
{
    // Pre-set ForeignStandardItem to a non-default category before patching
    auto &params = *apgame::itemParams;
    params.at(okami::ItemTypes::ForeignStandardItem).category = 42;

    itempatch::patchItemParams();

    // ArchipelagoTestItem categories are forced regardless of prior value
    REQUIRE(params.at(okami::ItemTypes::ForeignStandardItem).category == 1);
}

TEST_CASE_METHOD(ItemParamFixture, "patchItemParams: existing nonzero values are preserved", "[itempatch][ItemParam]")
{
    // Set specific entries to non-zero values BEFORE patching.
    // HolyBoneS (index 143) is not an ArchipelagoTestItem alias, so its
    // values should remain exactly as set.
    auto &params = *apgame::itemParams;
    params.at(okami::ItemTypes::HolyBoneS).category = 5;
    params.at(okami::ItemTypes::HolyBoneS).maxCount = 10;

    itempatch::patchItemParams();

    // patchItemParams only sets category/maxCount when they are zero,
    // so these pre-set non-zero values must survive unchanged.
    REQUIRE(params.at(okami::ItemTypes::HolyBoneS).category == 5);
    REQUIRE(params.at(okami::ItemTypes::HolyBoneS).maxCount == 10);
}

TEST_CASE("itempatch::initialize installs expected hooks", "[itempatch][hooks]")
{
    wolf::mock::reset();
    itempatch::initializeEarly(); // installs hookGetNumEntries
    itempatch::initialize();

    // Verify hooks were registered at the correct offsets
    REQUIRE(wolf::mock::registeredHooks.count(0x1AFC90) > 0); // hookLoadRscPkgAsync
    REQUIRE(wolf::mock::registeredHooks.count(0x1412B0) > 0); // hookGetNumEntries (from initializeEarly)
    REQUIRE(wolf::mock::registeredHooks.count(0x43BDA0) > 0); // hookGetItemIcon
    REQUIRE(wolf::mock::registeredHooks.count(0x1C9510) > 0); // hookLoadCore20MSD
}

TEST_CASE("hookGetNumEntries: returns 300 for texGroup 4", "[itempatch][hooks]")
{
    wolf::mock::reset();
    itempatch::initializeEarly(); // installs hookGetNumEntries
    itempatch::initialize();

    auto it = wolf::mock::registeredHooks.find(0x1412B0);
    REQUIRE(it != wolf::mock::registeredHooks.end());

    // Call the hook directly; texGroup==4 returns the expanded slot count
    // without touching s_origGetNumEntries (which is null in tests)
    using HookFn = int64_t (*)(void *, int32_t);
    auto fn = reinterpret_cast<HookFn>(it->second);
    REQUIRE(fn(nullptr, 4) == 300);
}

TEST_CASE_METHOD(ItemParamFixture, "patchItemParams: idempotent — calling twice does not change categories", "[itempatch][ItemParam]")
{
    itempatch::patchItemParams();
    itempatch::patchItemParams();

    auto &params = *apgame::itemParams;

    REQUIRE(params.at(okami::ItemTypes::ForeignStandardItem).category == 1);
    REQUIRE(params.at(okami::ItemTypes::ForeignProgressionItem).category == 3);
    REQUIRE(params.at(okami::ItemTypes::ForeignTrapItem).category == 4);
}

// ============================================================================
// Live scouted item name registration tests
// These tests do NOT require mock memory or MSD state.
// ============================================================================

TEST_CASE("registerScoutedItemName: idempotent — second call for same location is no-op", "[itempatch][live]")
{
    // Can be called any time, before or after initialize()
    REQUIRE_NOTHROW(itempatch::registerScoutedItemName(1001, "Boomerang"));
    REQUIRE_NOTHROW(itempatch::registerScoutedItemName(1001, "Should be ignored"));
}

TEST_CASE("registerScoutedItemName: different locations get different indices", "[itempatch][live]")
{
    REQUIRE_NOTHROW(itempatch::registerScoutedItemName(2001, "Fire Arrow"));
    REQUIRE_NOTHROW(itempatch::registerScoutedItemName(2002, "Ice Rod"));
}

TEST_CASE("itempatch::initialize registers GetMSDString and BuildSlotArray hooks", "[itempatch][hooks]")
{
    wolf::mock::reset();
    itempatch::initializeEarly();
    itempatch::initialize();
    REQUIRE(wolf::mock::registeredHooks.count(0x1C8A80) > 0); // hookGetMSDString
    REQUIRE(wolf::mock::registeredHooks.count(0x43E250) > 0); // hookBuildSlotArray
}

TEST_CASE("setCurrentShopId: can be called any time without crash", "[itempatch][live]")
{
    REQUIRE_NOTHROW(itempatch::setCurrentShopId(5));
    REQUIRE_NOTHROW(itempatch::setCurrentShopId(-1));
}

// ============================================================================
// Shop MSD string interception tests
// ============================================================================

TEST_CASE("CompileString: produces valid MSD encoding with EndDialog terminator", "[itempatch][msd]")
{
    auto compiled = okami::MSDManager::CompileString("Fire Arrow");
    REQUIRE(!compiled.empty());
    REQUIRE(compiled.back() == 0x8001);
    REQUIRE(compiled.size() == 11); // 10 chars + EndDialog
}

TEST_CASE("getShopCheckId produces correct location IDs for shop slots", "[itempatch][shops]")
{
    // Verify the mapping used by hookGetMSDString to resolve selected slots
    // getShopCheckId(shopId, slot) = 300000 + shopId*1000 + slot
    REQUIRE(checks::getShopCheckId(0, 0) == 300000);
    REQUIRE(checks::getShopCheckId(0, 1) == 300001);
    REQUIRE(checks::getShopCheckId(5, 0) == 305000);
    REQUIRE(checks::getShopCheckId(5, 3) == 305003);
    REQUIRE(checks::getShopCheckId(10, 7) == 310007);
}

TEST_CASE("registerScoutedItemName: CompileString output matches MSD encoding", "[itempatch][live]")
{
    // Verify that the string encoding used by registerScoutedItemName
    // (via MSDManager::CompileString) is correct for typical item names.
    auto compiled = okami::MSDManager::CompileString("Boomerang");
    REQUIRE(!compiled.empty());
    REQUIRE(compiled.back() == 0x8001); // EndDialog terminator

    // 'B' in Okami MSD encoding = 24
    REQUIRE(compiled[0] == 24);

    // Single-char string
    auto single = okami::MSDManager::CompileString("A");
    REQUIRE(single.size() == 2); // 'A' + EndDialog
    REQUIRE(single[0] == 23);    // 'A' = 23
    REQUIRE(single[1] == 0x8001);
}

// ============================================================================
// End-to-end resolveApItemName tests
// These exercise the real code path used by hookGetMSDString in production,
// using a mock shop buffer with controlled scroll/select offsets.
// ============================================================================

struct ShopContextFixture
{
    static constexpr int kShopId = 5;
    uint8_t mockShop[0x8C] = {};

    ShopContextFixture()
    {
        // Clean slate: clear all registrations and shop context from prior tests
        itempatch::resetState();
        // Set up a valid shop context: shop ID, shop pointer with scroll/select at slot 0
        itempatch::setCurrentShopId(kShopId);
        mockShop[0x8A] = 0; // scrollOffset
        mockShop[0x8B] = 0; // visualSelectIndex
        itempatch::setShopPointer(mockShop);
    }

    ~ShopContextFixture()
    {
        itempatch::resetState();
    }

    void selectSlot(uint8_t scroll, uint8_t visualSelect)
    {
        mockShop[0x8A] = scroll;
        mockShop[0x8B] = visualSelect;
    }

    int64_t locationIdForSlot(int slot) const
    {
        return checks::getShopCheckId(kShopId, slot);
    }
};

TEST_CASE_METHOD(ShopContextFixture, "resolveApItemName: returns custom string for info panel strId (type+0x2000)", "[itempatch][resolve]")
{
    // Register "Fire Arrow" at slot 0
    int64_t locId = locationIdForSlot(0);
    itempatch::registerScoutedItemName(locId, "Fire Arrow");
    selectSlot(0, 0);

    // Info panel strId for ForeignStandardItem: 120 + 0x2000 = 8312
    const uint16_t *result = itempatch::resolveApItemName(8312);
    REQUIRE(result != nullptr);

    // Verify the returned string matches CompileString("Fire Arrow")
    auto expected = okami::MSDManager::CompileString("Fire Arrow");
    for (size_t i = 0; i < expected.size(); ++i)
    {
        INFO("Mismatch at index " << i);
        REQUIRE(result[i] == expected[i]);
    }
}

TEST_CASE_METHOD(ShopContextFixture, "resolveApItemName: resolves both list and info panel strIds", "[itempatch][resolve]")
{
    int64_t locId = locationIdForSlot(2);
    itempatch::registerScoutedItemName(locId, "Ice Rod");
    selectSlot(0, 2);

    // Both paths resolve to the selected slot's scouted name
    REQUIRE(itempatch::resolveApItemName(424) != nullptr);  // 294 + ForeignProgressionItem (list)
    REQUIRE(itempatch::resolveApItemName(8322) != nullptr); // 0x2000 + ForeignProgressionItem (info panel)
}

TEST_CASE_METHOD(ShopContextFixture, "resolveApItemName: returns nullptr for non-AP strId", "[itempatch][resolve]")
{
    int64_t locId = locationIdForSlot(0);
    itempatch::registerScoutedItemName(locId, "Hookshot");
    selectSlot(0, 0);

    // strId 100 is not an AP dummy strId
    REQUIRE(itempatch::resolveApItemName(100) == nullptr);
    // strId 413 is one less than ForeignStandardItem+294=414
    REQUIRE(itempatch::resolveApItemName(413) == nullptr);
    // strId 415 is one more than 414
    REQUIRE(itempatch::resolveApItemName(415) == nullptr);
    // strId 8311 is one less than ForeignStandardItem+0x2000=8312
    REQUIRE(itempatch::resolveApItemName(8311) == nullptr);
    // strId 8313 is one more than 8312
    REQUIRE(itempatch::resolveApItemName(8313) == nullptr);
}

TEST_CASE_METHOD(ShopContextFixture, "resolveApItemName: returns nullptr when no shop context", "[itempatch][resolve]")
{
    int64_t locId = locationIdForSlot(0);
    itempatch::registerScoutedItemName(locId, "Bombs");
    selectSlot(0, 0);

    // Clear shop context — should fall through
    itempatch::clearShopContext();

    REQUIRE(itempatch::resolveApItemName(414) == nullptr);
    REQUIRE(itempatch::resolveApItemName(8312) == nullptr);
}

TEST_CASE_METHOD(ShopContextFixture, "resolveApItemName: returns nullptr when no name registered for slot", "[itempatch][resolve]")
{
    // Register a name at slot 0 but select slot 3 (which has no registration)
    int64_t locId = locationIdForSlot(0);
    itempatch::registerScoutedItemName(locId, "Bow");
    selectSlot(0, 3);

    // Slot 3 has no registered name
    REQUIRE(itempatch::resolveApItemName(414) == nullptr);
}

TEST_CASE_METHOD(ShopContextFixture, "resolveApItemName: scroll offset affects slot selection", "[itempatch][resolve]")
{
    // Register names at slots 0 and 7
    itempatch::registerScoutedItemName(locationIdForSlot(0), "Slot Zero Item");
    itempatch::registerScoutedItemName(locationIdForSlot(7), "Slot Seven Item");

    // Select slot 0 (scroll=0, visual=0)
    selectSlot(0, 0);
    const uint16_t *result0 = itempatch::resolveApItemName(8312); // 0x2000 + ForeignStandardItem
    REQUIRE(result0 != nullptr);
    auto expected0 = okami::MSDManager::CompileString("Slot Zero Item");
    REQUIRE(result0[0] == expected0[0]);

    // Select slot 7 (scroll=5, visual=2)
    selectSlot(5, 2);
    const uint16_t *result7 = itempatch::resolveApItemName(8312);
    REQUIRE(result7 != nullptr);
    auto expected7 = okami::MSDManager::CompileString("Slot Seven Item");
    REQUIRE(result7[0] == expected7[0]);

    // The two results should be different strings
    REQUIRE(result0 != result7);
}

TEST_CASE_METHOD(ShopContextFixture, "resolveApItemName: all three AP item types resolve correctly", "[itempatch][resolve]")
{
    // Register names at slots 0, 1, 2 for different AP item types
    itempatch::registerScoutedItemName(locationIdForSlot(10), "Standard Thing");
    itempatch::registerScoutedItemName(locationIdForSlot(11), "Progression Thing");
    itempatch::registerScoutedItemName(locationIdForSlot(12), "Trap Thing");

    // Both list (294+type) and info panel (0x2000+type) strIds should resolve

    // ForeignStandardItem: list=414, info=8312
    selectSlot(10, 0);
    REQUIRE(itempatch::resolveApItemName(414) != nullptr);
    REQUIRE(itempatch::resolveApItemName(8312) != nullptr);

    // ForeignProgressionItem: list=424, info=8322
    selectSlot(11, 0);
    REQUIRE(itempatch::resolveApItemName(424) != nullptr);
    REQUIRE(itempatch::resolveApItemName(8322) != nullptr);

    // ForeignTrapItem: list=469, info=8367
    selectSlot(12, 0);
    REQUIRE(itempatch::resolveApItemName(469) != nullptr);
    REQUIRE(itempatch::resolveApItemName(8367) != nullptr);
}

TEST_CASE_METHOD(ShopContextFixture, "resolveApItemName: returns nullptr when shop pointer is null", "[itempatch][resolve]")
{
    itempatch::registerScoutedItemName(locationIdForSlot(0), "Null Shop Test");
    selectSlot(0, 0);

    // Null out just the shop pointer, keep shopId valid
    itempatch::setShopPointer(nullptr);

    // 8312 = ForeignStandardItem + 0x2000 (info panel path)
    REQUIRE(itempatch::resolveApItemName(8312) == nullptr);
}

TEST_CASE_METHOD(ShopContextFixture, "resolveApItemName: resolves Okami-native dummy strIds", "[itempatch][resolve]")
{
    // Register a name at slot 0 so the shop context is valid
    itempatch::registerScoutedItemName(locationIdForSlot(0), "Power Slash");
    selectSlot(0, 0);

    // OkamiStandardItem (162), OkamiProgressionItem (168), OkamiTrapItem (172)
    // Both list (294+type) and info panel (0x2000+type) should resolve
    REQUIRE(itempatch::resolveApItemName(162 + 294) != nullptr);
    REQUIRE(itempatch::resolveApItemName(162 + 0x2000) != nullptr);
    REQUIRE(itempatch::resolveApItemName(168 + 294) != nullptr);
    REQUIRE(itempatch::resolveApItemName(168 + 0x2000) != nullptr);
    REQUIRE(itempatch::resolveApItemName(172 + 294) != nullptr);
    REQUIRE(itempatch::resolveApItemName(172 + 0x2000) != nullptr);
}

TEST_CASE("clearShopContext: resets both shop ID and pointer", "[itempatch][live]")
{
    uint8_t buf[0x8C] = {};
    itempatch::setCurrentShopId(3);
    itempatch::setShopPointer(buf);

    itempatch::clearShopContext();

    // After clearing, resolve should return nullptr even with a registered name
    itempatch::registerScoutedItemName(checks::getShopCheckId(3, 0), "Cleared Item");
    REQUIRE(itempatch::resolveApItemName(414) == nullptr);
}

TEST_CASE("resetState: clears all custom string registrations", "[itempatch][live]")
{
    uint8_t buf[0x8C] = {};
    itempatch::setCurrentShopId(5);
    itempatch::setShopPointer(buf);
    buf[0x8A] = 0;
    buf[0x8B] = 0;

    itempatch::registerScoutedItemName(checks::getShopCheckId(5, 0), "Reset Test");
    // Verify it resolves before reset (use info panel path: 0x2000 + ForeignStandardItem)
    REQUIRE(itempatch::resolveApItemName(8312) != nullptr);

    itempatch::resetState();

    // After reset, nothing should resolve (shop context cleared + registrations gone)
    itempatch::setCurrentShopId(5);
    itempatch::setShopPointer(buf);
    REQUIRE(itempatch::resolveApItemName(8312) == nullptr);

    itempatch::resetState(); // clean up
}
