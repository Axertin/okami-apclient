// Regression tests for brush grant + check-detection.
//
// These tests pin down two classes of bugs that previously slipped past the
// existing self-consistent unit tests:
//
//  1. Bit-ordering: granting a brush must land at the *exact* memory bit the
//     game reads. The game uses LSB-first-within-byte indexing on the brush
//     bitfield (`bytes[idx/8] & (1 << (idx%8))`), which differs from
//     BitField<N>'s MSB-first-within-32-bit-word convention. A previous fix
//     using BitField::Set silently set the wrong bits — Rejuvenation (idx 22)
//     was landing at vine_base (idx 19), Power Slash (idx 12) at thunderbolt
//     (idx 9). The "Self-consistent IsSet/Set" tests passed despite this.
//     Here we assert raw byte/mask values to lock the convention.
//
//  2. BrushData / WorldStateData mirroring: the game synchronizes the
//     WorldStateData copy from the BrushData source. Granting must write
//     BOTH or the bits get clobbered.
//
//  3. BrushMan hook: SET ops must fire a check + block; clear ops (1/2/3)
//     must NOT fire and must NOT block. The hook receives bitIndex in the
//     game's natural BrushOverlay convention (no further conversion needed).

#include <cstdint>
#include <memory>

#include <catch2/catch_test_macros.hpp>
#include <okami/brushes.hpp>
#include <okami/offsets.hpp>
#include <okami/structs.hpp>

#include "checks/brushes.hpp"
#include "checks/check_types.hpp"
#include "gamestate_accessors.hpp"
#include "rewardman.h"
#include "wolf_framework.hpp"

namespace
{

// Read raw bytes at a module-relative offset out of the mock memory block.
const uint8_t *rawBytes(uintptr_t offset)
{
    return wolf::mock::mockMemory.data() + offset;
}

// Game convention: bit X is in bytes[X/8] at mask (1 << (X%8)).
bool gameBitSet(const uint8_t *bytes, unsigned int bitIndex)
{
    return (bytes[bitIndex / 8] & static_cast<uint8_t>(1u << (bitIndex % 8))) != 0;
}

class BrushFixture
{
  protected:
    std::unique_ptr<RewardMan> rewardMan_;

    void setUp()
    {
        wolf::mock::reset();
        wolf::mock::reserveMemory(0xC00000 + 1024);
        apgame::initialize();
        rewardMan_ = std::make_unique<RewardMan>(nullptr);
        // Grants are gated on grantingEnabled_ which onPlayStart flips on.
        wolf::mock::triggerPlayStart();
    }

    void tearDown()
    {
        rewardMan_.reset();
        wolf::mock::reset();
    }
};

} // namespace

// ============================================================================
// Bit-ordering: pin down exactly which physical bit each grant sets
// ============================================================================

TEST_CASE_METHOD(BrushFixture, "Granting Rejuvenation sets byte 2 mask 0x40", "[brush][regression][bit-ordering]")
{
    setUp();

    // Rejuvenation (BrushOverlay::rejuvenation = 22): byte 22/8 = 2, bit 22%8 = 6, mask 0x40.
    constexpr int64_t kRejuvenationApId = 0x100 + static_cast<int>(okami::BrushOverlay::rejuvenation);

    auto result = rewardMan_->grantReward(kRejuvenationApId);
    REQUIRE(result.has_value());

    const auto *src = rawBytes(okami::main::usableBrushes);
    CHECK(src[2] == 0x40);
    CHECK(src[0] == 0x00);
    CHECK(src[1] == 0x00);
    CHECK(src[3] == 0x00);

    // Old buggy code (BitField::Set) wrote 0x80000000>>22 = 0x200, landing as
    // byte 1 mask 0x02 — that's vine_base/thunderbolt/etc, not rejuvenation.
    // Assert we did NOT regress.
    CHECK((src[1] & 0x02) == 0);

    tearDown();
}

TEST_CASE_METHOD(BrushFixture, "Granting Power Slash sets byte 1 mask 0x10", "[brush][regression][bit-ordering]")
{
    setUp();

    // Power Slash (BrushOverlay::power_slash = 12): byte 12/8 = 1, bit 12%8 = 4, mask 0x10.
    constexpr int64_t kPowerSlashApId = 0x10C;

    auto result = rewardMan_->grantReward(kPowerSlashApId);
    REQUIRE(result.has_value());

    const auto *src = rawBytes(okami::main::usableBrushes);
    CHECK(src[1] == 0x10);
    CHECK(src[0] == 0x00);
    CHECK(src[2] == 0x00);
    CHECK(src[3] == 0x00);

    // Old buggy code wrote 0x80000000>>12 = 0x80000 = byte 2 mask 0x08 (vine_base).
    CHECK((src[2] & 0x08) == 0);

    tearDown();
}

TEST_CASE_METHOD(BrushFixture, "Granting bit 1 sets byte 0 mask 0x02", "[brush][regression][bit-ordering]")
{
    setUp();

    // sunrise_default = 1: byte 0, bit 1, mask 0x02.
    constexpr int64_t kBit1ApId = 0x101;

    auto result = rewardMan_->grantReward(kBit1ApId);
    REQUIRE(result.has_value());

    const auto *src = rawBytes(okami::main::usableBrushes);
    CHECK(src[0] == 0x02);

    tearDown();
}

TEST_CASE_METHOD(BrushFixture, "Granting Catwalk (bit 30) sets byte 3 mask 0x40", "[brush][regression][bit-ordering]")
{
    setUp();

    // Catwalk = 30 (top of brush range): byte 3, bit 6, mask 0x40.
    constexpr int64_t kCatwalkApId = 0x100 + static_cast<int>(okami::BrushOverlay::catwalk);

    auto result = rewardMan_->grantReward(kCatwalkApId);
    REQUIRE(result.has_value());

    const auto *src = rawBytes(okami::main::usableBrushes);
    CHECK(src[3] == 0x40);

    tearDown();
}

// ============================================================================
// Source + copy mirroring: granting writes BOTH BrushData and WorldStateData
// ============================================================================

TEST_CASE_METHOD(BrushFixture, "Grant writes both BrushData source and WorldStateData copy", "[brush][regression][sync]")
{
    setUp();

    constexpr int64_t kRejuvenationApId = 0x116;
    auto result = rewardMan_->grantReward(kRejuvenationApId);
    REQUIRE(result.has_value());

    // The game synchronizes WorldStateData from BrushData; if we only wrote
    // the copy it'd be wiped on the next sync. Both must hold the bit.
    const auto *brushDataUsable = rawBytes(okami::main::usableBrushes);
    const auto *brushDataObtained = rawBytes(okami::main::obtainedBrushes);

    CHECK(gameBitSet(brushDataUsable, 22));
    CHECK(gameBitSet(brushDataObtained, 22));

    // WorldStateData is at collectionData + offsetof(world).
    const auto *worldUsable = reinterpret_cast<const uint8_t *>(apgame::usableBrushTechniques.get_ptr());
    const auto *worldObtained = reinterpret_cast<const uint8_t *>(apgame::obtainedBrushTechniques.get_ptr());
    CHECK(gameBitSet(worldUsable, 22));
    CHECK(gameBitSet(worldObtained, 22));

    tearDown();
}

// ============================================================================
// Celestial Brush Locked is cleared by BrushMan::tick() (per-tick gate clear)
//
// On a fresh playthrough, the game sets the "Celestial Brush Locked" bit
// AFTER the post-load grant phase — during the intro→River-of-the-Heavens
// scene transition. A clear that fires only at grant time runs too early and
// leaves the player permanently locked out of the brush UI for the rest of
// the session. The fix is a per-tick clear in BrushMan::tick() called from
// CheckMan::poll().
// ============================================================================

TEST_CASE("BrushMan::tick clears mapStateBits[0] index 10 (Celestial Brush Locked)", "[brush][regression][ui-gate]")
{
    wolf::mock::reset();
    checks::detail::resetBrushHookRegistrationForTests();
    wolf::mock::reserveMemory(0xC00000 + 1024);
    apgame::initialize();

    checks::BrushMan brushMan([](int64_t) {});
    brushMan.initialize();

    // Game sets the lock during a scene transition.
    apgame::worldStateData->mapStateBits[0].Set(10);
    REQUIRE(apgame::worldStateData->mapStateBits[0].IsSet(10));

    brushMan.tick();
    CHECK_FALSE(apgame::worldStateData->mapStateBits[0].IsSet(10));

    wolf::mock::reset();
}

TEST_CASE("Log 1 race: grant runs before game sets lock; subsequent tick clears it", "[brush][regression][ui-gate]")
{
    // Reproduces the failure mode from the user's first log: on a fresh
    // playthrough the grant flow runs while bit 10 is still 0, then the game
    // sets bit 10 during the intro cutscene transition. Without a continuous
    // clear the bit stays set forever and the brush UI is permanently locked.
    wolf::mock::reset();
    checks::detail::resetBrushHookRegistrationForTests();
    wolf::mock::reserveMemory(0xC00000 + 1024);
    apgame::initialize();

    RewardMan rewardMan(nullptr);
    wolf::mock::triggerPlayStart();

    checks::BrushMan brushMan([](int64_t) {});
    brushMan.initialize();

    // 1. Grant runs while bit 10 is still 0 (no-op for any grant-time clear).
    REQUIRE(apgame::worldStateData->mapStateBits[0].IsSet(10) == false);
    REQUIRE(rewardMan.grantReward(0x116).has_value()); // Rejuvenation

    // 2. Game sets bit 10 during the next scene transition.
    apgame::worldStateData->mapStateBits[0].Set(10);
    REQUIRE(apgame::worldStateData->mapStateBits[0].IsSet(10));

    // 3. Next BrushMan::tick (fired from CheckMan::poll) clears it.
    brushMan.tick();
    CHECK_FALSE(apgame::worldStateData->mapStateBits[0].IsSet(10));

    wolf::mock::reset();
}

TEST_CASE("BrushMan::tick is a no-op when uninitialized", "[brush][regression][ui-gate][lifecycle]")
{
    wolf::mock::reset();
    checks::detail::resetBrushHookRegistrationForTests();
    wolf::mock::reserveMemory(0xC00000 + 1024);
    apgame::initialize();

    checks::BrushMan brushMan([](int64_t) {});
    // Don't initialize — simulates a teardown/before-init state.

    apgame::worldStateData->mapStateBits[0].Set(10);
    brushMan.tick();
    // Tick should not silently clobber state when BrushMan isn't active.
    CHECK(apgame::worldStateData->mapStateBits[0].IsSet(10));

    wolf::mock::reset();
}

// ============================================================================
// Progressive brush progression: base then upgrades, in order
// ============================================================================

TEST_CASE_METHOD(BrushFixture, "Power Slash progression: base, then upgrade bits 0 and 10", "[brush][regression][progressive]")
{
    setUp();

    constexpr int64_t kPowerSlashApId = 0x10C;
    const auto *src = rawBytes(okami::main::usableBrushes);
    const auto *upgrades = reinterpret_cast<const uint8_t *>(apgame::brushUpgrades.get_ptr());

    // 1st grant: base (bit 12).
    REQUIRE(rewardMan_->grantReward(kPowerSlashApId).has_value());
    CHECK(gameBitSet(src, 12));
    CHECK_FALSE(gameBitSet(upgrades, 0));
    CHECK_FALSE(gameBitSet(upgrades, 10));

    // 2nd grant: first upgrade (bit 0 in brushUpgrades).
    REQUIRE(rewardMan_->grantReward(kPowerSlashApId).has_value());
    CHECK(gameBitSet(upgrades, 0));
    CHECK_FALSE(gameBitSet(upgrades, 10));

    // 3rd grant: second upgrade (bit 10 in brushUpgrades).
    REQUIRE(rewardMan_->grantReward(kPowerSlashApId).has_value());
    CHECK(gameBitSet(upgrades, 10));

    // 4th grant: max level — succeeds as no-op.
    REQUIRE(rewardMan_->grantReward(kPowerSlashApId).has_value());

    tearDown();
}

// ============================================================================
// BrushMan hook behavior
// ============================================================================

namespace
{

class BrushManFixture
{
  protected:
    std::vector<int64_t> sentChecks_;
    std::unique_ptr<checks::BrushMan> brushMan_;

    void setUp()
    {
        wolf::mock::reset();
        checks::detail::resetBrushHookRegistrationForTests();
        wolf::mock::reserveMemory(0xC00000 + 1024);
        apgame::initialize();
        sentChecks_.clear();
        brushMan_ = std::make_unique<checks::BrushMan>([this](int64_t id) { sentChecks_.push_back(id); });
        brushMan_->initialize();
        brushMan_->setActive(true);
    }

    void tearDown()
    {
        brushMan_.reset();
        wolf::mock::reset();
    }
};

} // namespace

TEST_CASE_METHOD(BrushManFixture, "BrushMan: SET op (operation==0) fires check and blocks", "[brush][regression][brushman]")
{
    setUp();

    // Power Slash bit 12, set operation. Bit is currently unset.
    bool blocked = wolf::mock::triggerBrushEdit(12, 0);
    CHECK(blocked);

    // Hook only queues; tick() drains.
    CHECK(sentChecks_.empty());
    brushMan_->tick();

    REQUIRE(sentChecks_.size() == 1);
    CHECK(sentChecks_[0] == checks::getBrushCheckId(12));
    CHECK(sentChecks_[0] == 200012);

    tearDown();
}

TEST_CASE_METHOD(BrushManFixture, "BrushMan: clear ops (1, 2, 3) ignored and not blocked", "[brush][regression][brushman]")
{
    setUp();

    SECTION("op==1 (clear usable)")
    {
        CHECK_FALSE(wolf::mock::triggerBrushEdit(12, 1));
        CHECK(sentChecks_.empty());
    }

    SECTION("op==2 (clear both)")
    {
        CHECK_FALSE(wolf::mock::triggerBrushEdit(12, 2));
        CHECK(sentChecks_.empty());
    }

    SECTION("op==3 (clear obtained)")
    {
        CHECK_FALSE(wolf::mock::triggerBrushEdit(12, 3));
        CHECK(sentChecks_.empty());
    }

    tearDown();
}

TEST_CASE_METHOD(BrushManFixture, "BrushMan: bitIndex passed through unchanged in check ID", "[brush][regression][brushman]")
{
    setUp();

    // Spot-check several bits to confirm no convention conversion happens.
    wolf::mock::triggerBrushEdit(1, 0);
    wolf::mock::triggerBrushEdit(22, 0);
    wolf::mock::triggerBrushEdit(30, 0);
    brushMan_->tick();

    REQUIRE(sentChecks_.size() == 3);
    CHECK(sentChecks_[0] == 200001);
    CHECK(sentChecks_[1] == 200022);
    CHECK(sentChecks_[2] == 200030);

    tearDown();
}

TEST_CASE_METHOD(BrushManFixture, "BrushMan: inactive hook never blocks or queues", "[brush][regression][brushman][gating]")
{
    setUp();
    brushMan_->setActive(false);

    CHECK_FALSE(wolf::mock::triggerBrushEdit(12, 0));
    brushMan_->tick();
    CHECK(sentChecks_.empty());

    tearDown();
}

TEST_CASE_METHOD(BrushManFixture, "BrushMan: already-obtained bit falls through", "[brush][regression][brushman][gating]")
{
    setUp();

    // Pre-set the obtained bit at the source (bit 12 = byte 1 mask 0x10).
    auto *src = wolf::mock::mockMemory.data() + okami::main::obtainedBrushes;
    src[1] |= 0x10;

    CHECK_FALSE(wolf::mock::triggerBrushEdit(12, 0));
    brushMan_->tick();
    CHECK(sentChecks_.empty());

    tearDown();
}

TEST_CASE_METHOD(BrushManFixture, "BrushMan: out-of-range bitIndex falls through", "[brush][regression][brushman][gating]")
{
    setUp();

    CHECK_FALSE(wolf::mock::triggerBrushEdit(-1, 0));
    CHECK_FALSE(wolf::mock::triggerBrushEdit(32, 0));
    CHECK_FALSE(wolf::mock::triggerBrushEdit(99999, 0));
    brushMan_->tick();
    CHECK(sentChecks_.empty());

    tearDown();
}

TEST_CASE_METHOD(BrushManFixture, "BrushMan: after shutdown, hook becomes no-op", "[brush][regression][brushman][lifecycle]")
{
    setUp();

    brushMan_->shutdown();

    bool blocked = wolf::mock::triggerBrushEdit(12, 0);
    CHECK_FALSE(blocked);
    CHECK(sentChecks_.empty());

    tearDown();
}

TEST_CASE_METHOD(BrushManFixture, "BrushMan: re-initialize after shutdown re-attaches", "[brush][regression][brushman][lifecycle]")
{
    setUp();

    brushMan_->shutdown();
    REQUIRE_FALSE(wolf::mock::triggerBrushEdit(12, 0));

    brushMan_->initialize();
    brushMan_->setActive(true);
    REQUIRE(wolf::mock::triggerBrushEdit(12, 0));
    brushMan_->tick();
    REQUIRE(sentChecks_.size() == 1);

    tearDown();
}

TEST_CASE("BrushMan: destroyed instance does not dangle", "[brush][regression][brushman][lifecycle]")
{
    wolf::mock::reset();
    checks::detail::resetBrushHookRegistrationForTests();
    wolf::mock::reserveMemory(0xC00000 + 1024);
    apgame::initialize();

    int callCount = 0;
    {
        checks::BrushMan tmp([&](int64_t) { ++callCount; });
        tmp.initialize();
        tmp.setActive(true);
        REQUIRE(wolf::mock::triggerBrushEdit(12, 0));
        tmp.tick();
        REQUIRE(callCount == 1);
    }
    // tmp is destroyed; the callback in wolf must safely no-op now.
    CHECK_FALSE(wolf::mock::triggerBrushEdit(12, 0));
    CHECK(callCount == 1); // unchanged

    wolf::mock::reset();
}

TEST_CASE("BrushMan: replacing the active instance routes to the latest", "[brush][regression][brushman][lifecycle]")
{
    wolf::mock::reset();
    checks::detail::resetBrushHookRegistrationForTests();
    wolf::mock::reserveMemory(0xC00000 + 1024);
    apgame::initialize();

    int firstCount = 0;
    int secondCount = 0;

    auto first = std::make_unique<checks::BrushMan>([&](int64_t) { ++firstCount; });
    first->initialize();
    first->setActive(true);

    auto second = std::make_unique<checks::BrushMan>([&](int64_t) { ++secondCount; });
    second->initialize(); // claims g_activeHandler from `first`
    second->setActive(true);

    wolf::mock::triggerBrushEdit(12, 0);
    second->tick();

    CHECK(firstCount == 0);
    CHECK(secondCount == 1);

    second.reset();
    first.reset();
    wolf::mock::reset();
}
