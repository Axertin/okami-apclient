
#pragma once
#include <cstdint>

namespace okami
{
    inline float *AmmyPosXPtr;
    inline float *AmmyPosYPtr;
    inline float *AmmyPosZPtr;
    inline int16_t *AmmyCurrentHealthPtr;
    inline int16_t *AmmyMaxHealthPtr;
    inline uint8_t *HealthUpgradeCountPtr;
    inline int32_t *AmmyCurrentInkPtr;
    inline int32_t *AmmyMaxInkPtr;
    inline int32_t *BrushTargetInkPtr;
    inline int32_t *BrushCurrentInkPtr;
    inline uint8_t *InkUpgradeCountPtr;
    inline uint8_t *CanvasBrushStrokesPtr;
    inline int16_t *AmmyCurrentFoodPtr;
    inline int16_t *AmmyMaxFoodPtr;
    inline uint8_t *FoodUpgradeCountPtr;
    inline int16_t *AmmyGodhoodPtr;
    inline uint32_t *AmmyCurrentMoneyPtr;
    inline uint32_t *AmmyDisplayedMoneyPtr;
    inline uint32_t *AmmyMaxMoney; // Array of length 4
    inline uint32_t *AmmyTotalMoneyPtr;
    inline uint8_t *MoneyUpgradeCountPtr;
    inline int16_t *AmmyCurrentPraisePtr;
    inline int16_t *PraiseUpgradeBarPtr;
    inline int16_t *HealthUpgradeBarPtr;
    inline int16_t *FoodUpgradeBarPtr;
    inline int16_t *MoneyUpgradeBarPtr;
    inline int16_t *InkUpgradeBarPtr;
    inline int16_t *AmmyTotalPraisePtr;
    inline int16_t *AmmyCurrentDemonFangsPtr;
    inline uint16_t *AmmyTotalDemonFangsPtr;
}
