# Server Protocol Guide

This document describes what the okami-apclient expects from the Archipelago server. If you're working on an Okami APWorld, this is the contract your world needs to fulfill.

The client uses the standard Archipelago protocol via the apclientpp library. This document focuses on Okami-specific details rather than rehashing the general AP protocol (see the [Archipelago Network Protocol](https://github.com/ArchipelagoMW/Archipelago/blob/main/docs/network%20protocol.md) docs for that).

## Connection Overview

- **Transport**: WebSocket (ws:// for localhost, wss:// for remote servers)
- **Default port**: 38281
- **Message format**: JSON (handled by apclientpp)

The client automatically upgrades to secure WebSocket for non-localhost connections.

## Connection Handshake

```
Client                                    Server
   │                                         │
   │──────── WebSocket Connect ─────────────>│
   │                                         │
   │<──────── RoomInfo ──────────────────────│
   │                                         │
   │──────── ConnectSlot ───────────────────>│
   │         (slot, password,                │
   │          item_handling=0b111)           │
   │                                         │
   │<──────── Connected ─────────────────────│
   │         (checked_locations, slot_data)  │
   │                                         │
   │──────── StatusUpdate(PLAYING) ─────────>│
   │                                         │
```

**Item handling bits** (`0b111 = 7`):

- Bit 0: Receive items for self
- Bit 1: Receive items for others
- Bit 2: Starting inventory

We request all three.

## Messages the Client Sends

### ConnectSlot

Initial connection with credentials.

```json
{
  "cmd": "Connect",
  "password": "...",
  "game": "Okami HD",
  "name": "PlayerSlot",
  "uuid": "...",
  "version": { ... },
  "items_handling": 7,
  "tags": []
}
```

### LocationChecks

Reports completed locations (checks in mod terminology).

```json
{
  "cmd": "LocationChecks",
  "locations": [100042, 200003, 500010015]
}
```

Locations are sent in batches when possible. The client deduplicates locally before sending.

### LocationScouts

Requests item information for locations without completing them. Used for shop display and to replace container items on spawn.

```json
{
  "cmd": "LocationScouts",
  "locations": [300001, 300002, 300003],
  "create_as_hint": 0
}
```

The client uses `create_as_hint: 0` (no hint creation) for these purposes, as they aren't actually hints. The client simply needs to know before the location is sent what is there so that it can be displayed correctly.

### StatusUpdate

Sent when gameplay state changes.

```json
{
  "cmd": "StatusUpdate",
  "status": 30
}
```

Status values:

- `30` (CLIENT_PLAYING): Normal gameplay
- `50` (CLIENT_GOAL): Game completed

### Sync

Requests full state resync when desync is detected.

```json
{
  "cmd": "Sync"
}
```

## Messages the Client Expects

### RoomInfo

Triggers the connection handshake. Standard AP format.

### Connected

Contains initial game state:

```json
{
  "cmd": "Connected",
  "team": 0,
  "slot": 1,
  "players": [...],
  "missing_locations": [...],
  "checked_locations": [100001, 100002, ...],
  "slot_data": { ... }
}
```

**`checked_locations`**: List of locations the server already has. The client syncs its local cache with this and resends any missing checks.

**`slot_data`**: Game-specific configuration. See [slot_data](#slot_data-format) below.

### ReceivedItems

Items granted to the player:

```json
{
  "cmd": "ReceivedItems",
  "index": 0,
  "items": [
    {
      "item": 256,
      "location": 100042,
      "player": 2,
      "flags": 0,
      "index": 0
    },
    ...
  ]
}
```

**Index handling**:

- `index: 0` in the message indicates a full inventory sync (reconnect scenario)
- Each item has its own index for ordering
- Gaps in indices trigger desync recovery

The client persists the last processed index to disk (`%APPDATA%\okami-apsaves\{slot}_{seed}.save`) and skips already-processed items on reconnect.

### LocationInfo

Response to LocationScouts:

```json
{
  "cmd": "LocationInfo",
  "locations": [
    {
      "item": 258,
      "location": 300001,
      "player": 1,
      "flags": 0
    },
    ...
  ]
}
```

Used by the shop system to display what items are available before purchase.

### ConnectionRefused / PrintJSON

Error conditions. The client logs these and updates the UI status.

---

## Location ID Scheme

The client expects locations to follow this ID scheme. The APWorld should define locations with matching IDs.

| Category | Formula | Range |
| ---------- | --------- | ------- |
| Item Pickup | `100000 + itemId` | 100000-100255 |
| Brush Acquisition | `200000 + brushIndex` | 200000-200021 |
| Shop Purchase | `300000 + (shopId × 1000) + slot` | 300000-399999 |
| World State | `400000 + (mapId × 10000) + bitIndex` | 400000-499999 |
| Collected Object | `500000 + (mapId × 10000) + bitIndex` | 500000-599999 |
| Area Restored | `600000 + (mapId × 10000) + bitIndex` | 600000-699999 |
| Global Flag | `700000 + bitIndex` | 700000-700031 |
| Game Progress | `800000 + bitIndex` | 800000-800031 |
| Container | `900000 + (levelId << 8) + spawnIdx` | 900000-999999 |

**Note**: This scheme is subject to change as we finalize the APWorld.

### Container ID Encoding

Container IDs pack two values:

- `levelId`: 0-255 (which map the container is in)
- `spawnIdx`: 0-255 (index in that map's spawn table)

Formula: `900000 + (levelId << 8) + spawnIdx`

Example: Level 5, spawn index 42 → `900000 + (5 << 8) + 42 = 901322`

### Shop ID Encoding

Shop IDs pack shop identifier and slot:

- `shopId`: Which shop (numbered 0-N)
- `slot`: Which slot in that shop's inventory (0-N)

Formula: `300000 + (shopId × 1000) + slot`

Example: Shop 3, slot 7 → `300000 + 3000 + 7 = 303007`

---

## Item ID Scheme

The APWorld should define items with these IDs:

| Range | Category | Notes |
| ------- | ---------- | ------- |
| 0x00-0xFF | Game Items | Direct inventory items; AP ID = game item ID |
| 0x100-0x115 | Brushes | `brushIndex = apItemId - 0x100` |
| 0x300 | Progressive Mirror | Trinity Mirror → Solar Flare |
| 0x301 | Progressive Rosary | 5-stage progression |
| 0x302 | Progressive Sword | 5-stage progression |
| 0x303-0x308 | Event Flags | Story progression bits |

### Progressive Items

These items upgrade through multiple tiers when received multiple times:

**Progressive Weapons**:

| AP ID | Name | Stages |
| ------- | ------ | -------- |
| 0x300 | Progressive Mirror | Trinity Mirror (0x13) → Solar Flare (0x14) |
| 0x301 | Progressive Rosary | Devout (0x15) → Life (0x16) → Exorcism (0x17) → Resurrection (0x18) → Tundra (0x19) |
| 0x302 | Progressive Sword | Tsumugari (0x1A) → Seven Strike (0x1B) → Kusanagi (0x1C) → Eighth Wonder (0x1D) → Thunder Edge (0x1E) |

**Progressive Brushes**:

| AP ID | Name | Stages |
| ------- | ------ | -------- |
| 0x102 | Power Slash | Base → PS2 (upgrade bit 0) → PS3 (upgrade bit 10) |
| 0x103 | Cherry Bomb | Base → CB2 (upgrade bit 6) → CB3 (upgrade bit 11) |

---

## slot_data Format

The client parses these Okami-specific fields from `slot_data`:

```json
{
  "slot_data": {
    "SeedNumber": "64715609337703266491",
    "SeedName": "43126361899033089832",
    "TotalLocations": 190,

    "supported_client_version": "0.0.0",

    "randomize_containers": true,
    "randomize_shops": true,
    "randomize_brushes": true,

    ...
    (etc)
  }
}
```

**Note**: This format is not finalized. Missing fields use safe defaults.

---

## Error Recovery

### Connection Timeout

The client waits 10 seconds for connection. If `slot_connected` doesn't arrive, it marks the connection as failed.

### Item Index Gaps

Gaps in received item indices trigger a full resync:

```
Client detects gap → Sync → Resend all checks
```

### Reconnection

On reconnect:

1. Client loads last processed item index from disk
2. Server sends full inventory (index 0)
3. Client skips items up to saved index
4. Client processes new items
5. Client syncs checked_locations and resends any missing

---

## What's Next?

For more context on how the client uses this protocol, see:

- [Architecture Guide](architecture.md) - Code structure
- [Mod Flow Guide](mod-flow.md) - Runtime behavior
