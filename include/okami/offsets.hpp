/**
 * @file okamioffsets.h
 * @brief Memory offset constants for Okami HD game structures
 *
 * This file contains compile-time constants for memory offsets within game modules.
 * All offsets are relative to their respective module base addresses.
 */

#pragma once

#include <cstdint>

namespace okami
{
namespace main
{

//==============================================================================
// CORE GAME DATA (Save File Related)
//==============================================================================

/// Character stats struct (health, food, praise, weapons, etc.)
constexpr uintptr_t characterStats = 0xB4DF90;            // CharacterStats
constexpr uintptr_t characterHealth = 0xB4DF90;           // uint16_t
constexpr uintptr_t characterMaxHealth = 0xB4DF90 + 2;    // uint16_t
constexpr uintptr_t characterFood = 0xB4DF90 + 4;         // uint16_t
constexpr uintptr_t characterMaxFood = 0xB4DF90 + 6;      // uint16_t
constexpr uintptr_t characterPraise = 0xB4DF90 + 10;      // uint16_t
constexpr uintptr_t characterTotalPraise = 0xB4DF90 + 12; // uint16_t

/// Collection data struct (inventory, money, collectibles, world state)
constexpr uintptr_t collectionData = 0xB205D0;           // CollectionData
constexpr uintptr_t collectionMoney = 0xB205D0 + 12;     // uint32_t
constexpr uintptr_t collectionCurrentInk = 0xB205D0 + 8; // uint32_t
constexpr uintptr_t collectionMaxInk = 0xB205D0 + 12;    // uint32_t

/// Tracker data struct (progression flags, options, bestiary)
constexpr uintptr_t trackerData = 0xB21780; // TrackerData

/// Map state data for all maps
constexpr uintptr_t mapData = 0xB322B0; // std::array<MapState, MapTypes::NUM_MAP_TYPES>

/// Dialog/interaction bits for all maps
constexpr uintptr_t dialogBits = 0xB36CF0; // std::array<BitField<512>, MapTypes::NUM_MAP_TYPES>

//==============================================================================
// GAME STATE & UI
//==============================================================================

/// Global game state flags
constexpr uintptr_t globalGameStateFlags = 0xB6B2AC; // BitField<86>

/// Camera field of view
constexpr uintptr_t cameraFOV = 0xB663B0; // float

//==============================================================================
// BRUSH & INK SYSTEM
//==============================================================================

/// Target ink amount for brush usage
constexpr uintptr_t brushTargetInk = 0x8928A4; // int32_t

/// Current ink amount
constexpr uintptr_t brushCurrentInk = 0x8928A8; // int32_t

/// Number of ink upgrades obtained
constexpr uintptr_t inkUpgradeCount = 0xB1F208; // uint8_t

/// Canvas brush stroke count
constexpr uintptr_t canvasBrushStrokes = 0x888C58; // uint8_t

/// Brushes that can be used
constexpr uintptr_t usableBrushes = 0x890A30; // BitField<32>

/// Brushes that have been obtained
constexpr uintptr_t obtainedBrushes = 0x890A38; // BitField<32>

/// Brush upgrade levels array
constexpr uintptr_t brushUpgrades = 0x8909C0 + 0x80; // std::array<uint8_t, 64>

//==============================================================================
// UPGRADE SYSTEMS
//==============================================================================

/// Food upgrade count
constexpr uintptr_t foodUpgradeCount = 0xB1F207; // uint8_t

/// Displayed money amount in UI
constexpr uintptr_t displayedMoney = 0xB1CFE4; // uint32_t

/// Maximum money capacity
constexpr uintptr_t maxMoney = 0x6B22A8; // uint32_t

/// Praise upgrade progress bar
constexpr uintptr_t praiseUpgradeBar = 0xB1F1F4; // int16_t

/// Health upgrade progress bar
constexpr uintptr_t healthUpgradeBar = 0xB1F1F6; // int16_t

/// Food upgrade progress bar
constexpr uintptr_t foodUpgradeBar = 0xB1F1F8; // int16_t

/// Money upgrade progress bar
constexpr uintptr_t moneyUpgradeBar = 0xB1F1FA; // int16_t

/// Ink upgrade progress bar
constexpr uintptr_t inkUpgradeBar = 0xB1F1FC; // int16_t

//==============================================================================
// OBJECT POINTERS
//==============================================================================

/// Amaterasu model object
constexpr uintptr_t ammyModel = 0xB6B2D0; // cAmmyModel*

/// Possible Amaterasu game object
constexpr uintptr_t ammyObject = 0xB6B2D0; // void*

/// Inventory structure pointer
constexpr uintptr_t inventoryStructPtr = 0xB66670; // void*

/// Player class pointer
constexpr uintptr_t playerClassPtr = 0x8909C0; // void*

//==============================================================================
// ITEM & INVENTORY
//==============================================================================

/// Item parameters array
constexpr uintptr_t itemParams = 0x7AB220; // std::array<ItemParam, ItemTypes::NUM_ITEM_TYPES>

//==============================================================================
// ADDITIONAL OFFSETS
//==============================================================================

/// Map ID references (additional)
constexpr uintptr_t exteriorMapIDCopy = 0xB6B248; // uint16_t (lastMapId reference)

} // namespace main
} // namespace okami
