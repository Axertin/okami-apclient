# Memory Mapping Guide

This guide explains how to contribute reverse engineering discoveries to help map Okami's save data and memory structures.

## What is Memory Mapping?

Okami HD stores game state in various bitfields and data structures in memory. By understanding what each bit and memory location represents, we can:
- Find locations to use for Archipelago
- Track game progress accurately
- Debug save file issues
- Understand the game's internal logic

## Real-Time Memory Monitoring

The mod includes a built-in memory monitoring system that automatically detects when undocumented memory locations change and logs them to the console.

### Enabling Memory Monitoring

Memory monitoring is enabled by default in debug builds. You'll see output like:
```
[16:26:57] [WARN] [Undocumented] BitField MapBits[9] (Kusa Village) index 1271 was changed from 1 to 0
[16:26:57] [WARN] [Undocumented] BitField MapBits[9] (Kusa Village) index 1272 was changed from 1 to 0
```

### Understanding the Log Format

Each log entry contains:
- **Timestamp** - When the change occurred
- **Category** - Usually `BitField MapBits[X]` for map state bits
- **Map Info** - Map ID and name in parentheses
- **Bit Index** - The specific bit that changed (0-based)
- **Value Change** - From what to what (usually 0 to 1 or 1 to 0)

### Log Files

All memory changes are automatically saved to timestamped files in the `logs/` directory. This lets you review changes after gameplay sessions.

#### Custom Log Markers
The devtools GUI window includes a button to place custom markers in the log. This is extremely helpful for reviewing sessions later - you can mark significant moments like "about to start quest" or "opening xyz chest" and then easily find the relevant bit changes in the log file.

## Contributing Memory Discoveries

### Quick Contributions

When you see interesting bit changes in the console:

1. **Note what you were doing** - The most important information is what action in-game triggered the bit change
2. **Use the issue template** - Create a [Memory ID Mapping issue](https://github.com/Axertin/okami-apclient/issues/new?template=memory_id_mapping.md)
3. **Copy the exact log line** - This ensures we get the correct map, bit index, and change details
4. **Describe the context** - Were you talking to an NPC, picking up an item, completing a quest, etc.?

### Systematic Discovery

For more thorough reverse engineering:

#### Save File Comparison
1. **Create a save before** triggering something interesting
2. **Perform the action** (talk to NPC, complete quest, etc.)
3. **Note the bit changes** in the console
4. **Create a save after** the action
5. **Compare** - The bit changes should correspond to the difference in save files

#### Memory Watching Sessions
1. **Start a fresh gameplay session** or load a specific save
2. **Keep the console visible** - Press END to toggle the GUI overlay
3. **Use log markers** - Click the marker button in devtools GUI before important actions
4. **Play normally** but watch for undocumented bit changes
5. **Document immediately** - When you see a change, note exactly what you just did
6. **Test reproducibility** - Can you trigger the same bit change again?

## Types of Memory Locations

### WorldStateBits
Most common category. These track:
- **Quest progress** - Quest started, quest completed, dialogue triggered
- **Cutscenes** - Whether specific scenes have been viewed
- **Game state** - Temporary flags, battle states, interaction states
- **Story progression** - Major game events and milestones

### CollectedObjects
Track items and collectibles:
- **Chests** - Whether specific treasure chests have been opened
- **Pickups** - Clover, buried items, breakable objects
- **Rewards** - Quest rewards, boss drops, etc.

### AreasRestored
Environmental changes:
- **Cursed areas** - Areas purified with bloom
- **Restoration** - Broken bridges, dried ponds, etc.
- **Environmental state** - Before/after major changes

### TreesBloomed / CursedTreesBloomed
Individual tree restoration tracking:
- **Normal trees** - Individual trees restored with bloom
- **Cursed trees** - Specifically cursed trees that need special treatment

### FightsCleared
Combat-related flags:
- **Demon gates** - Battles that appear as red smoke barriers
- **Boss fights** - Major boss encounters
- **Tutorial battles** - Combat tutorials and scripted fights

### NPCs
Character interaction states:
- **Dialogue progress** - How many times you've talked to someone
- **Quest states** - NPC-specific quest progression
- **Character availability** - Whether NPCs are present/accessible

### MapsExplored
Area discovery:
- **Room discovery** - Individual rooms or areas within maps
- **First visit** - Whether you've been to an area before

## Current Mapping Status

Check `src/library/devdatamapdata.cpp` to see what's already documented for each map. This file contains our current understanding of what each bit means.

### Well-Documented Maps
- **Kamiki Village** - Most bits are understood
- **Shinshu Field** - Good coverage of major areas
- **Tsuta Ruins** - Dungeon mechanics well-mapped

### Needs More Work
- **Later game areas** - Oni Island, Ark of Yamato, etc.
- **Mini-dungeons** - Small caves and special areas
- **Quest-specific maps** - Areas used for specific story beats

## Best Practices

### During Discovery
- **One action at a time** - Don't do multiple things simultaneously
- **Note exact timing** - The timestamp helps correlate actions with bit changes
- **Include context** - Quest state, time of day, what NPCs you've talked to
- **Test edge cases** - What happens if you do the same thing twice?

### When Reporting
- **Be specific** - "Talked to Merchant" vs "Interacted with NPC"
- **Include surrounding log lines** - Sometimes multiple bits change together
- **Mention related bits** - If you notice patterns across multiple indices
- **Describe reproducibility** - Can others trigger the same change?

### Verification
- **Test on different saves** - Does it work the same way on other save files?
- **Try variations** - Slightly different approaches to the same goal
- **Check persistence** - Does the bit stay set after leaving/returning to the map?

## Tools and Techniques

### Console Controls
- **END** - Toggle GUI overlay (shows/hides GUI)
- **Log markers** - Use the button in devtools GUI to mark important moments in the log

### Memory Analysis
- **Save file comparison** - External tools can diff save files
- **Cheat Engine** - Advanced users can watch memory directly
- **Debugging builds** - More verbose logging in debug configurations

### Pattern Recognition
Look for patterns in bit changes:
- **Sequential bits** - Often related functionality
- **Bit clusters** - Groups that change together
- **Timing patterns** - Bits that set temporarily then clear

## Common Scenarios

### Quest Progression
```
Quest Started → worldStateBits[X] = 1
Talk to NPC 1 → worldStateBits[Y] = 1  
Complete objective → worldStateBits[Z] = 1
Quest Completed → worldStateBits[X] = 0, worldStateBits[A] = 1
```

### Item Collection
```
Approach chest → (no change)
Open chest → collectedObjects[N] = 1
Item obtained → (possibly other bits for inventory)
```

### Area Restoration
```
Use bloom technique → areasRestored[M] = 1
Environmental change → (visual/audio feedback)
New functionality → (possibly other bits enable)
```

## Getting Help

- **Pattern questions** - Ask in [Discord](https://discord.com/channels/731205301247803413/1196620860405067848) if you see interesting patterns
- **Technical issues** - Report console problems as bugs
- **Mapping discussions** - Coordinate with other contributors to avoid duplicate work

## Advanced Topics

### BitField Structure
Memory is organized as bitfields with specific sizes:
- `BitField<512>` - Standard map state (512 bits = 64 bytes)
- `BitField<32>` - Smaller collections (32 bits = 4 bytes)

### Memory Layout
Understanding the general structure helps with discovery:
- Map-specific data is isolated per map
- Similar functionality often uses similar bit patterns
- Save file structure mirrors memory layout

Remember: Every bit you help document makes the Archipelago integration more robust and helps future contributors understand the game better!
