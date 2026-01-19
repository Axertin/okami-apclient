# Mod Flow Guide

This document explains how the mod works at runtime - what happens when you play the game with the Archipelago client connected.

If you haven't already, you might want to skim the [Architecture Guide](architecture.md) first to understand the major components.

## The Game Tick

Everything revolves around the game tick - a callback that runs every frame. Here's what happens each tick:

```
wolf::onGameTick()
  │
  ├─ processMainThreadTasks()   ← Execute any queued tasks from network thread
  │
  ├─ socket.poll()              ← Non-blocking APClient poll
  │
  ├─ rewardMan.processQueuedRewards()   ← Grant any pending rewards
  │
  └─ checkMan.poll()            ← Poll containers for state changes
```

This keeps everything synchronized: network events are processed, rewards are granted, and check sources are polled - all on the main thread where it's safe to touch game state.

---

## The Check System (Game → Server)

Checks represent in-game events that correspond to AP locations. When the player does something significant (opens a chest, learns a brush technique, etc.), the mod detects it and tells the server.

### Check Categories

The check ID scheme encodes what type of check it is and any relevant parameters. Here's how IDs are calculated:

| Category | ID Formula | Range | Example |
|----------|------------|-------|---------|
| Item Pickup | `100000 + itemId` | 100000-100255 | Picking up a consumable |
| Brush Acquisition | `200000 + brushIndex` | 200000-200021 | Learning a technique |
| Shop Purchase | `300000 + (shopId × 1000) + slot` | 300000-399999 | Buying from a merchant |
| World State | `400000 + (mapId × 10000) + bitIndex` | 400000-499999 | Map environment flags |
| Collected Object | `500000 + (mapId × 10000) + bitIndex` | 500000-599999 | Stray beads, etc. |
| Area Restored | `600000 + (mapId × 10000) + bitIndex` | 600000-699999 | Guardian saplings |
| Global Flag | `700000 + bitIndex` | 700000-700031 | Game-wide status bits |
| Game Progress | `800000 + bitIndex` | 800000-800031 | Story milestones |
| Container | `900000 + (levelId << 8) + spawnIdx` | 900000-999999 | Chests, jars (WIP) |

**Note**: This ID scheme is subject to change as we finalize the APWorld.

### Detection Methods

Checks are detected in a few different ways depending on the source:

#### Passive (Bitfield Monitors)

For game state flags, we watch memory directly. The WOLF framework provides bitfield monitors that call us back when bits transition from 0 to 1.

```
Memory location changes: bit 5 goes 0 → 1
  │
  └─ Monitor callback fires
      │
      └─ CheckMan computes check ID and sends it
```

This covers:

- Game progress bits (story milestones)
- Global flags (quest completion, etc.)
- Per-map world state bits
- Per-map collected objects
- Per-map area restorations

#### Active (Hooks)

For containers and shops, we hook into game functions directly:

- **Containers**: Hook the spawn table populator when levels load
- **Shops**: Hook shop loading and purchase functions (WIP)

### Deduplication

CheckMan maintains a set of already-sent check IDs. This prevents double-sends if:

- The same bit gets set multiple times
- A container gets polled before we remove it from tracking
- We reconnect and the server already has the check

### Enable/Disable

Check sending is automatically disabled:

- When in menus (prevents accidental triggers from menu navigation)
- While rewards are being granted (prevents feedback loops - see below)

---

## The Reward System (Server → Game)

Rewards are items the server sends to the player. They arrive asynchronously on the network thread and get granted on the main thread.

### The Queue

```
APClient receives item (network thread)
  │
  └─ queueReward(apItemId)  ← Thread-safe, just adds to queue
                 │
                 ▼
     [Next game tick, main thread]
                 │
                 └─ processQueuedRewards()
                     │
                     ├─ Disable check sending
                     │
                     ├─ For each queued reward:
                     │   └─ grantReward() → dispatch to handler
                     │
                     └─ Re-enable check sending
```

### Reward Categories

Rewards are categorized by their AP item ID:

| Range | Category | Handling |
| ------- | ---------- | ---------- |
| 0x00-0xFF | Game Item | Direct inventory placement (AP ID = game item ID) |
| 0x100-0x115 | Brush | Technique unlock |
| 0x300-0x302 | Progressive Weapon | Grants next upgrade stage |
| 0x303-0x308 | Event Flag | Sets story progression bit |

### Progressive Items

Some rewards are "progressive" - receiving multiple copies grants increasingly powerful versions:

**Progressive Weapons** (0x300-0x302):

- Mirror: Trinity Mirror → Solar Flare
- Rosary: Devout Beads → Life Beads → Exorcism Beads → Resurrection Beads → Tundra Beads
- Sword: Tsumugari → Seven Strike → Blade of Kusanagi → Eighth Wonder → Thunder Edge

**Progressive Brushes** (0x102-0x103):

- Power Slash: Base → Power Slash 2 → Power Slash 3
- Cherry Bomb: Base → Cherry Bomb 2 → Cherry Bomb 3

The reward handlers check current inventory/upgrade state and grant the next tier.

### Feedback Loop Prevention

Here's a subtle problem: when we grant a reward, it might change game state bits. For example, giving the player an item might set a flag that looks like a new check to our monitors.

To prevent this, check sending is **disabled** while rewards are being granted:

```
processQueuedRewards() starts
  │
  ├─ checkMan.enableSending(false)   ← Checks paused
  │
  ├─ Grant all rewards
  │   └─ (Game state changes here won't trigger checks)
  │
  └─ checkMan.enableSending(true)    ← Checks resume
```

This callback mechanism is set up at initialization - RewardMan gets a lambda that controls CheckMan's sending state.

---

## Container Randomization (WIP)

Containers (chests, breakable jars) are randomized by replacing their contents at spawn time.

### The Flow

```
Level loads
  │
  └─ Spawn table populator hook fires
      │
      ├─ Iterate all spawn entries
      │   │
      │   ├─ Filter: enabled? type == container?
      │   │
      │   ├─ Compute check ID from (levelId, entryIndex)
      │   │
      │   ├─ Replace item with dummy (Chestnut, 0x83)
      │   │
      │   └─ Track entry index for polling
      │
      └─ Level finishes loading with randomized containers
```

```
Player opens container
  │
  └─ Picks up dummy item
      │
      └─ [Game tick: checkMan.poll()]
          │
          ├─ Check spawn entry state machine
          │   └─ 1 (closed) → 3 (opened) → 0 (collected)
          │
          ├─ When state == 0:
          │   ├─ Send container check to server
          │   └─ Remove from tracking
          │
          └─ Server sends actual reward → RewardMan grants it
```

**Current status**: The hooking and detection work, but we currently randomize ALL containers when connected. Proper integration with `slot_data` (to know which containers are actually in the randomization pool) is still pending.

---

## Shop Randomization (WIP)

Shops will be randomized by intercepting shop loading and replacing the stock list with scouted AP items.

### Planned Flow

```
Player approaches shop
  │
  └─ Shop variation hook fires
      │
      └─ scoutShopsForMap(mapId)
          │
          ├─ Query server: "What items are at these shop locations?"
          │
          └─ Store scouted items for later
```

```
Shop menu opens
  │
  └─ ISL (Item Shop List) load hook fires
      │
      ├─ Build custom ISL from scouted items
      │
      └─ Return custom data instead of vanilla
```

```
Player purchases item
  │
  └─ Purchase hook fires
      │
      ├─ Send shop check to server
      │
      └─ Server sends actual reward → RewardMan grants it
```

**Current status**: The hook infrastructure is in place and we have code for building ISL files, but the actual scouting and data mapping is stubbed out. We're waiting on the Okami APWorld to define how shop data will be provided in `slot_data`.

---

## Example Walkthrough: Opening a Randomized Chest

Let's trace through what happens when a player opens a randomized chest:

1. **Level loads**: The spawn table populator runs. ContainerMan's hook sees a container entry, replaces its item with a dummy item, and records the entry index.

2. **Player finds chest**: They see a chest in the world (visual unchanged, just contents swapped).

3. **Player opens chest**: The game's normal logic runs - chest opens, player picks up... a Chestnut.

4. **Check detection**: On the next game tick, `checkMan.poll()` runs. ContainerMan checks the spawn entry state, sees it went to 0 (collected), computes the check ID, and calls `socket.sendLocation(checkId)`.

5. **Server processes**: The AP server receives the location check, determines what item was there, and sends it to the appropriate player.

6. **Reward received**: If it's for us, APClient's item handler fires, calling `rewardMan.queueReward(apItemId)`.

7. **Reward granted**: On the next game tick, `processQueuedRewards()` runs. Check sending is disabled, the reward handler places the actual item in inventory (say, an Inkfinity Stone), and check sending is re-enabled.

8. **Player sees result**: They got a Chestnut from the chest, but also an Inkfinity Stone appeared in their inventory. (UI feedback for this is still being worked on.)

---

## Connection and Reconnection

### Initial Connection

```
User clicks Connect in login window
  │
  └─ socket.connect(server, slot, password)
      │
      ├─ Validate input
      ├─ Build WebSocket URI (ws:// local, wss:// remote)
      ├─ Create APClient instance
      └─ Register protocol handlers
```

```
[Server sends room_info]
  │
  └─ Handler calls ConnectSlot(slot, password)
```

```
[Server sends slot_connected]
  │
  ├─ Load last processed item index from disk
  │   └─ File: %APPDATA%\okami-apsaves\{slot}_{seed}.save
  │
  ├─ Sync with server's checked_locations list
  │   └─ Resend any checks server doesn't have
  │
  └─ Enable check sending
```

### Desync Recovery

Sometimes the client and server get out of sync (network issues, crashes, etc.). The mod detects this via item indices:

```
[Items received with index gap]
  │
  ├─ Expected index: 42
  ├─ Received index: 45
  │
  └─ Desync detected!
      │
      ├─ Request full sync from server
      └─ Resend all checks
```

The item index is saved to disk after each batch of items, so reconnecting picks up where you left off.

---

## What's Next?

If you're working on the Okami APWorld and need to understand the client-server contract, see [Server Protocol](server-protocol.md).
