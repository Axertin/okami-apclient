#include "brushes.hpp"

#include <okami/offsets.hpp>

#include "../gamestate_accessors.hpp"
#include "check_types.hpp"

// NOTE: A more precise approach may be to hook FUN_180208e90 (RVA 0x208e90),
// the constellation animation init function. It is called once when the brush god
// cutscene begins, reads the brush type from entity+0xe35, and sets entity+0xe36 = 1
// (init flag).
// Polling is used for now as it is simpler and requires less RE work.

namespace checks
{

BrushMan::BrushMan(CheckCallback checkCallback) : checkCallback_(std::move(checkCallback))
{
}

BrushMan::~BrushMan()
{
    shutdown();
}

void BrushMan::initialize()
{
    if (initialized_)
        return;

    // Bind source (BrushData) accessors for clearing bits the game sets natively
    usableBrushSource_ = wolf::MemoryAccessor<okami::BitField<32>>("main.dll", okami::main::usableBrushes);
    obtainedBrushSource_ = wolf::MemoryAccessor<okami::BitField<32>>("main.dll", okami::main::obtainedBrushes);

    constexpr size_t byteCount = (32 + 7) / 8; // 32 bits = 4 bytes

    usableBrushesMonitor_ = wolf::createBitfieldMonitor(
        "main.dll", okami::main::usableBrushes, byteCount,
        [this](unsigned int monitorBit, bool oldValue, bool newValue)
        {
            if (!oldValue && newValue)
            {
                unsigned int gameBit = monitorToGameBitIndex(monitorBit);
                checkCallback_(getBrushCheckId(static_cast<int>(gameBit)));
                clearBrushBit(monitorBit);
                wolf::logDebug("[BrushMan] Detected and sent check %lld for brush idx %d (usable)", getBrushCheckId(static_cast<int>(gameBit)), gameBit);
            }
        },
        "UsableBrushes monitor");

    obtainedBrushesMonitor_ = wolf::createBitfieldMonitor(
        "main.dll", okami::main::obtainedBrushes, byteCount,
        [this](unsigned int monitorBit, bool oldValue, bool newValue)
        {
            if (!oldValue && newValue)
            {
                unsigned int gameBit = monitorToGameBitIndex(monitorBit);
                checkCallback_(getBrushCheckId(static_cast<int>(gameBit)));
                clearBrushBit(monitorBit);
                wolf::logDebug("[BrushMan] Detected and sent check %lld for brush idx %d (obtained)", getBrushCheckId(static_cast<int>(gameBit)), gameBit);
            }
        },
        "ObtainedBrushes monitor");

    initialized_ = true;
    wolf::logInfo("[BrushMan] Brush monitors initialized");
}

void BrushMan::clearBrushBit(unsigned int bitIndex)
{
    // Clear at BrushData source (what the monitors watch)
    usableBrushSource_->Clear(bitIndex);
    obtainedBrushSource_->Clear(bitIndex);

    // Clear at WorldStateData copy (what the game engine reads)
    apgame::usableBrushTechniques->Clear(bitIndex);
    apgame::obtainedBrushTechniques->Clear(bitIndex);
}

void BrushMan::shutdown()
{
    if (!initialized_)
        return;

    if (usableBrushesMonitor_)
    {
        wolf::destroyBitfieldMonitor(usableBrushesMonitor_);
        usableBrushesMonitor_ = nullptr;
    }

    if (obtainedBrushesMonitor_)
    {
        wolf::destroyBitfieldMonitor(obtainedBrushesMonitor_);
        obtainedBrushesMonitor_ = nullptr;
    }

    initialized_ = false;
}

void BrushMan::reset()
{
    shutdown();
}

} // namespace checks
