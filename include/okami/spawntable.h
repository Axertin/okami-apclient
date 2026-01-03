#pragma once
#include <cstdint>

namespace okami
{

// Spawn data configuration for a single container
struct ContainerData
{
    uint8_t item_id;           // +0x00: Item ID to spawn
    uint8_t item_type;         // +0x01: Item type (dir to lookup in for item data, 0xA -> data_pc/it/)
    uint8_t _unk_1;            // +0x02: Padding?
    uint8_t container_state;   // +0x03: State bitfield
    uint8_t scale_x;           // +0x04: Scale X (divided by 100.0)
    uint8_t scale_y;           // +0x05: Scale Y (divided by 100.0)
    uint8_t scale_z;           // +0x06: Scale Z (divided by 100.0)
    uint8_t rotation_x;        // +0x07: Rotation X
    uint8_t rotation_y;        // +0x08: Rotation Y
    uint8_t rotation_z;        // +0x09: Rotation Z
    int16_t position_x;        // +0x0A: Position X
    int16_t position_y;        // +0x0C: Position Y
    int16_t position_z;        // +0x0E: Position Z
    uint8_t container_subtype; // +0x10: Container appearance (0-19)
    uint8_t _unk_2[0x08];      // +0x12: Unknown
    uint8_t _pad[0x10];        // +0x18: Rest of structure (0x28 total)
};

// Single entry in spawn table (one of 128)
// Size: 0x58 bytes (88 bytes)
struct SpawnTableEntry
{
    void *created_entity;      // +0x00: Output - spawned entity pointer (written by spawn functions)
    uint8_t spawn_type_1;      // +0x08: Primary type (1 = chest path, 2 = item path)
    uint8_t pad_09;            // +0x09
    uint8_t spawn_type_2;      // +0x0A: Secondary type / sub-dispatch
    char _pad_0B[0x05];        // +0x0B
    void *spawned_entity;      // +0x10: Entity pointer (chest stores here after spawn)
    uint16_t counter;          // +0x18: Respawn timer
    char _pad_1A[0x06];        // +0x1A
    ContainerData *spawn_data; // +0x20: Spawn configuration pointer
    char _pad_28[0x02];        // +0x28
    uint16_t flags;            // +0x2A: Entry flags (bit 0 = enabled, bit 1 = temp, bit 2 = clear after)
    char _pad_2C[0x1C];        // +0x2C: Unknown padding
    void *parent_entity;       // +0x48: Parent entity pointer (for item→chest linking)
    void *list_ptr_2;          // +0x50: Secondary link pointer
};

struct SpawnTableHeader
{                         // (32 bytes)
    uint32_t initialized; // +0x00
    uint32_t state;       // +0x04
    void *data_ptr;       // +0x08: 0x0000027439DE4520
    uint32_t count;       // +0x10:
    uint32_t capacity;    // +0x14: 0 (or maybe part of next field)
    void *end_ptr;        // +0x18: 0x0000027439DE4524 (data_ptr + 4 bytes)
};

// Global spawn table buffer
// Total size: 0x2C20 + 0x500 = 0x3120 bytes
struct SpawnTable
{
    SpawnTableHeader header;      // +0x00: Header (32 bytes)
    SpawnTableEntry entries[128]; // +0x20: 128 spawn entries (0x58 bytes each) = 0x2C00
    // Total main table: 0x20 + 0x2C00 = 0x2C20
    ContainerData dynamic_data[32]; // +0x2C20: Dynamic ContainerData buffer (32 slots × 0x28) (probably render limit?)
};

// Global spawn table address
constexpr uintptr_t SPAWN_TABLE_OFFSET = 0xB66800; // main.dll+0xB66800
} // namespace okami
