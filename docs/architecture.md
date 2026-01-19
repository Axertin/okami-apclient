# Code Architecture Guide

This document explains how the okami-apclient codebase is organized and how its pieces fit together. If you're looking to contribute or just want to understand what's going on under the hood, this is the place to start.

## What This Project Is

okami-apclient is an Archipelago client mod for Okami HD (Steam). It enables multiworld randomizer gameplay by:

- Detecting when the player completes "checks" (locations in AP terminology) and reporting them to the server
- Receiving "rewards" (items in AP terminology) from the server and granting them in-game
- Replacing container contents and shop inventories with randomized items (WIP)

The mod is built on the WOLF framework, which handles DLL injection into the game and provides utilities for memory access, function hooking, and ImGui rendering.

**Current status**: v0.x.x pre-release. Core functionality works, but some randomization features are still in progress.

## Terminology Note

This codebase uses slightly different terminology than standard Archipelago, to clarify the difference between actual game inventory items and the concept of an "Item" as known by AP:

| AP Term | Mod Term | Managed By |
|---------|----------|------------|
| Locations | Checks | CheckMan |
| Items | Rewards | RewardMan |

We'll use the mod's terminology throughout this document, but the mapping is straightforward.

## Directory Structure

```
okami-apclient/
├── src/okami-apclient/           # Main mod source
│   ├── okami-apclient.cpp        # Entry point (WOLF_MOD_ENTRY_CLASS)
│   ├── archipelagosocket.*       # AP server communication
│   ├── checkman.*                # Check detection and sending
│   ├── rewardman.*               # Reward queue and granting
│   ├── loginwindow.*             # ImGui connection UI
│   ├── gamestate_accessors.*     # Game memory accessor setup
│   ├── isocket.h                 # Abstract socket interface (for testing)
│   ├── checks/                   # Check detection subsystems
│   │   ├── check_types.hpp       # Check ID scheme definitions
│   │   ├── gamestate_monitors.*  # Bitfield change detection
│   │   ├── containers.*          # Container randomization (partial)
│   │   └── shops.*               # Shop randomization (WIP)
│   └── rewards/                  # Reward granting subsystems
│       ├── reward_types.hpp      # Reward category definitions
│       ├── game_items.*          # Inventory items + progressive weapons
│       ├── brushes.*             # Brush techniques (some progressive)
│       └── event_flags.*         # Story progression flags
│
├── include/okami/                # Game data definitions
│   ├── gamestate/                # Per-map flag definitions
│   ├── structs.hpp               # Core game memory structures
│   ├── offsets.hpp               # Memory addresses
│   └── bitfield.hpp              # Bitfield utilities
│
├── include/wolf_framework.hpp    # WOLF framework API (single header)
│
├── tests/                        # Unit tests (Catch2)
│   ├── mocks/                    # Test mocks for socket, etc.
│   └── harness/                  # Test utilities
│
├── external/                     # Dependencies
│   ├── apclientpp/               # Archipelago protocol client
│   ├── websocketpp/              # WebSocket library
│   ├── imgui/                    # UI framework
│   └── wswrap/                   # WebSocket wrapper
│
├── cmake/                        # CMake utilities
├── scripts/                      # Build-time helpers
└── docs/                         # You are here
```

## Key Components

### APClientMod (Entry Point)

**File**: `okami-apclient.cpp`

This is where everything starts. The mod registers with WOLF using `WOLF_MOD_ENTRY_CLASS(APClientMod)`, which provides three lifecycle hooks:

- `earlyGameInit()` - Currently empty; reserved for future use
- `lateGameInit()` - Main initialization (see [Initialization Sequence](#initialization-sequence))
- `shutdown()` - Cleanup when the game exits

The entry point owns the other major components (`CheckMan`, `RewardMan`) and registers the game tick handler that drives the mod's per-frame logic.

### ArchipelagoSocket

**Files**: `archipelagosocket.h`, `archipelagosocket.cpp`

Singleton managing the WebSocket connection to the AP server. Uses the apclientpp library internally.

**Responsibilities**:

- Connection management (connect, disconnect, reconnect)
- Protocol message handling (room info, slot connected, items received, etc.)
- Thread-safe task queue for cross-thread communication
- Location scouting for shop randomization
- Item index persistence (saves progress to disk per-session)

The socket exposes a simple interface to the rest of the mod:

- `connect(server, slot, password)` / `disconnect()`
- `sendLocation(id)` / `sendLocations(ids)`
- `scoutLocationsSync(locations, timeout)` - blocking scout request
- `poll()` - called every frame to process network events

### CheckMan

**Files**: `checkman.h`, `checkman.cpp`

Orchestrates all check detection systems and handles deduplication.

**Check sources**:

- **Memory monitors**: Watch game bitfields for 0→1 transitions (game progress, global flags, per-map state)
- **Container handler**: Detects when the player picks up randomized container items
- **Shop handler**: Detects shop purchases (WIP)

CheckMan maintains a set of already-sent check IDs to prevent duplicates. It also handles synchronization with the server on connect (re-sending any checks the server doesn't have).

Check sending can be enabled/disabled:

- Disabled when in menus (no accidental triggers)
- Disabled during reward granting (prevents feedback loops)

### RewardMan

**Files**: `rewardman.h`, `rewardman.cpp`

Queue-based reward processor. Items arrive from the network thread and get queued for processing on the main thread.

**Workflow**:

1. `queueReward(apItemId)` - Thread-safe, called from APClient callbacks
2. `processQueuedRewards()` - Called on game tick, dispatches to category handlers

**Category handlers** (in `rewards/`):

- `game_items.cpp` - Direct inventory items + progressive weapons (Mirror, Rosary, Sword)
- `brushes.cpp` - Brush techniques, including progressive upgrades for Power Slash and Cherry Bomb
- `event_flags.cpp` - Story progression flags

RewardMan uses a callback to disable check sending while granting rewards. This prevents feedback loops where granting an item triggers game state changes that look like new checks.

### ContainerMan (WIP)

**Files**: `checks/containers.h`, `checks/containers.cpp`

Hooks the game's spawn table populator to replace container contents with dummy items, then detects when those dummies are picked up.

**Current status**: Core hooking and detection works, but the mod currently randomizes ALL containers when connected. Proper integration with `slot_data` (to know which containers are actually randomized) is pending.

### ShopMan (WIP)

**Files**: `checks/shops.h`, `checks/shops.cpp`

Infrastructure for shop randomization. Six hook points are installed, and there's code for building custom shop inventory files (ISL format).

**Current status**: Hooks are in place but functionality is stubbed. Waiting on the Okami APWorld to define the shop randomization data format in `slot_data`.

## Threading Model

The mod uses a minimal-locking approach with message passing between threads.

**Thread boundaries**:

- **Main thread**: Game loop, ImGui, reward granting, check polling
- **APClient thread**: WebSocket I/O (internal to apclientpp)

**Synchronization**:

- `clientMutex_` - Protects the APClient instance
- `taskMutex_` - Protects the main-thread task queue
- `queueMutex_` - Protects the reward queue
- `scoutMutex_` + condition variable - Coordinates scout requests
- Atomic flags for connection state (allows non-blocking reads)

**Task queue pattern**: APClient callbacks queue lambdas to run on the main thread. The game tick handler processes these via `processMainThreadTasks()`.

## WOLF Framework Integration

WOLF provides the infrastructure for game modding. Key features we use:

**Lifecycle**:

```cpp
WOLF_MOD_ENTRY_CLASS(APClientMod)  // Registers init/shutdown hooks
wolf::onPlayStart(callback);        // Gameplay started
wolf::onReturnToMenu(callback);     // Returned to menu
wolf::onGameTick(callback);         // Every frame
```

**Memory access**:

```cpp
wolf::MemoryAccessor<T>(moduleName, address);     // Safe memory reads
wolf::createBitfieldMonitor(address, callback);   // Watch for bit changes
wolf::hookFunction(moduleName, offset, newFunc, outOldFunc);
```

**ImGui**:

```cpp
wolf::registerGuiWindow(name, renderFunc, visible);
// Plus various setup/teardown macros
```

**Logging**:

```cpp
wolf::logInfo(fmt, args...);
wolf::logDebug(fmt, args...);
wolf::logWarning(fmt, args...);
wolf::logError(fmt, args...);
```

## Initialization Sequence

Here's what happens when the game loads:

```
WOLF Framework Starts
  │
  ├─ APClientMod::earlyGameInit()  ← Empty, reserved for future use
  │
  └─ APClientMod::lateGameInit()
      │
      ├─ apgame::initialize()
      │    └─ Set up MemoryAccessor objects to game state
      │
      ├─ Create CheckMan with socket reference
      │
      ├─ Create RewardMan with check-disabling callback
      │
      ├─ Inject managers into socket for cross-communication
      │
      ├─ loginwindow::initialize()
      │    └─ Load saved connection info, register ImGui window
      │
      ├─ checkMan->initialize()
      │    ├─ Register play/menu callbacks
      │    ├─ Create memory monitors for bitfield checks
      │    ├─ Create ContainerMan with spawn table hook
      │    └─ Create ShopMan with shop-related hooks
      │
      └─ Register game tick handler
           └─ Called every frame to process tasks, poll, grant rewards
```

## Dependencies

| Dependency | Purpose |
| ------------ | --------- |
| WOLF framework | Game modding infrastructure (memory access, hooks, ImGui) |
| apclientpp | Archipelago protocol client library |
| websocketpp + wswrap | WebSocket transport |
| ImGui | Login window UI |
| nlohmann-json | JSON parsing |
| asio | Async I/O (used by websocketpp) |
| OpenSSL | TLS for secure WebSocket connections |
| Catch2 | Unit testing framework |

## Testing

Tests live in `tests/` and use Catch2. They're organized by component:

- `test_checkman.cpp` - Check detection and deduplication
- `test_rewardman.cpp` - Reward queue and granting
- `test_containers.cpp` - Container hook logic
- `test_shops.cpp` - Shop infrastructure
- `test_gamestate_monitors.cpp` - Bitfield monitoring
- `test_reward_handlers.cpp` - Individual reward category handlers

The `mocks/` directory contains test doubles for the socket interface and other dependencies.

## What's Next?

If you're looking to understand the runtime behavior of the mod, check out [Mod Flow](mod-flow.md).

If you're working on the Okami APWorld and need to understand what the client expects from the server, see [Server Protocol](server-protocol.md).
