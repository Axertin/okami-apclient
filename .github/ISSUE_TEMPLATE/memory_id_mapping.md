---
name: Memory ID Mapping
about: Report the meaning of a memory ID/bit flag in Okami HD
title: "[Memory ID] Map [MapID] Bit [BitIndex] - [Brief Description]"
labels: memory-mapping, reverse-engineering
assignees: ''

---

## Console Log Entry
**Copy the exact log line(s) here:**
```
[timestamp] [WARN] [Undocumented] BitField MapBits[X] (Map Name) index YYYY was changed from Z to W
```

## Quick Info (auto-filled from log)
**Map ID:** <!-- Extract from log, e.g., MapBits[9] = Kusa Village -->
**Map Name:** <!-- Extract from log -->
**Bit Index:** <!-- Extract from log -->
**Change:** <!-- Extract from log: from X to Y -->
**Category:** `worldStateBits` <!-- Most common, change if different -->

## What You Were Doing
**Action taken when bit changed:**
<!-- What did you just do in-game when this bit flipped? -->

**Game context:**
<!-- Were you in a cutscene, talking to an NPC, picking up an item, etc.? -->

## Proposed Meaning
**Short description:** <!-- e.g., "Talked to Mr Orange for the first time" -->

**Detailed description:** <!-- More context about what this represents -->

## Verification (Optional)
**Can you reproduce it?**
- [ ] Yes, consistently
- [ ] Yes, but only sometimes  
- [ ] No, was a one-time event
- [ ] Haven't tested yet

**Additional testing:**
<!-- If you did more testing, describe what you found -->

## Log File Context
**Paste 5-10 lines before/after the bit change for context:**
```
<!-- This helps identify patterns and related bit changes -->
```

---

**Notes:**
<!-- Any other observations, related quest info, or patterns you noticed -->

---

<!-- Quick Reference for Categories:
- worldStateBits: Quest progress, cutscenes, general game state  
- userIndices: Counters, timers, numeric values
- collectedObjects: Chests, items, pickups, destructible objects
- areasRestored: Cursed areas purified
- treesBloomed: Individual trees restored  
- cursedTreesBloomed: Cursed trees specifically
- fightsCleared: Demon gates, battles, bosses
- npcs: NPC interaction states
- mapsExplored: Room/area discovery flags
-->
