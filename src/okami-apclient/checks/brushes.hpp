#pragma once

#include <cstdint>
#include <functional>

#include <okami/bitfield.hpp>
#include <wolf_framework.hpp>

namespace checks
{

class BrushMan
{
  public:
    using CheckCallback = std::function<void(int64_t)>;

    explicit BrushMan(CheckCallback checkCallback);
    ~BrushMan();

    BrushMan(const BrushMan &) = delete;
    BrushMan &operator=(const BrushMan &) = delete;
    BrushMan(BrushMan &&) = delete;
    BrushMan &operator=(BrushMan &&) = delete;

    void initialize();
    void shutdown();
    void reset();

  private:
    void clearBrushBit(unsigned int bitIndex);

    CheckCallback checkCallback_;

    wolf::BitfieldMonitorHandle usableBrushesMonitor_ = nullptr;
    wolf::BitfieldMonitorHandle obtainedBrushesMonitor_ = nullptr;

    // Accessors for clearing brush bits at source (BrushData) addresses
    wolf::MemoryAccessor<okami::BitField<32>> usableBrushSource_;
    wolf::MemoryAccessor<okami::BitField<32>> obtainedBrushSource_;

    bool initialized_ = false;
};

} // namespace checks
