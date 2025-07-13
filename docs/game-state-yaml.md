# Game State YAML System

The Game State Registry is an authoratative, YAML-based system for managing the game state flag definitions and descriptions ("Meanings") in Okami.

## Overview

Okami HD tracks the majority of game state through thousands of bit flags organized in many different bitfields, generally grouped by relevant map or context. This includes things like:

- Whether you've talked to specific NPCs and in what situations
- Which chests you've opened
- What quests you've completed
- Which areas you've restored
- and many others

## For Players: What This Means

**You don't need to understand this system to play** - it works automatically in the background. However, if you're curious about how the game tracks your progress or want to help with reverse engineering, this system makes it much easier to understand what's happening.

## For Contributors: Understanding the Structure

### Core Components

- **`IgameStateRegistry`**: Interface defining the registry, to allow mocking.
- **`GameStateRegistry`**: Access and data controller, based on YAML files.
- **`MapStateConfig`**: Map-specific flags structure
- **`GlobalConfig`**: Global flag structure

### File Structure

The game data lives in `src/library/game-data/` with this structure:

```
src/library/game-data/
├── global.yml                    # Global state definitions
├── maps/                         # Map-specific definitions
│   ├── KamikiVillage.yml
│   ├── HanaValley.yml
│   └── ...
└── schema/                       # Validation schemas
    ├── global.schema.yml
    └── map-state.schema.yml
```

## YAML File Format

Each flag category follows this simple pattern:

```yaml
category_name:
    0: "Description of what bit 0 means"
    1: "Description of what bit 1 means"
    15: "Description of what bit 15 means"
    42: "Description of what bit 42 means"
    # Numbers can be non-sequential - only document known flags
```

### Real Example

Here's what a portion of `KamikiVillage.yml` might look like:

```yaml
worldStateBits:
    2: "Spoke to Sakuya for the first time"
    5: "Guardian Sapling quest started"
    12: "Talked to Mr. Orange about dancing"
    18: "Completed Destroy the Boulder quest"

collectedObjects:
    0: "Chest behind Susano's house (Holy Bone S)"
    3: "Buried treasure near river (Traveler's Charm)"
    7: "Dig spot near Guardian Sapling (Turnip)"

npcs:
    0: "Mr. Orange has more dialog available"
    1: "Mushi available for conversation"
    4: "Kushi has new dialog after Orochi events"
```

## Contributing to the YAML System

### Discovering New Mappings

The mod includes built-in memory monitoring that will show you when undocumented flags change (see [Memory Mapping Guide](memory-mapping.md) for more details):

```
[16:26:57] [WARN] [Undocumented] MapState[9] (Kusa Village) worldStateBits[42] changed 0→1
```

When you see messages like this:

1. **Note what you just did** in the game (talked to NPC, opened chest, etc.)
2. **Record the details**: map name, category, bit index, and your action
3. **Test it again** on a different save file to confirm
4. **Add it to the appropriate YAML file**

### Adding New Descriptions

1. **Find the right file**: Map-specific flags go in `maps/MapName.yml`, game-wide flags go in `global.yml`

2. **Choose the right category**: Should be reported in the undocumented log line (In the example above, this would be `worldStateBits`)

3. **Write a clear description**:
   ```yaml
   # Good
   42: "Spoke to Merchant about rare items quest"
   
   # Bad
   42: "NPC dialog flag"
   ```

4. **Validate your changes**:
   ```bash
   python scripts/validate_game_data.py src/library/game-data/
   ```

### Testing Your Changes

The YAML system loads automatically when the mod starts. To test your changes:

1. **Make your edits** to the YAML files
2. **Restart the game** (the system loads once at startup)
3. **Check the DevTools** - Your descriptions should appear in the UI
4. **Verify functionality** - Make sure the flags still work as expected

## Schema Validation

The system includes automatic validation to catch common mistakes:

- **Duplicate indices** within the same category
- **Invalid YAML syntax** (indentation, missing colons)
- **Missing required categories** in global.yml
- **Malformed entries** that can't be parsed

### Running Validation

```bash
# Validate all YAML files
python scripts/validate_game_data.py src/library/game-data/

# The build system also runs this automatically
```

## Advanced Usage

### For Developers

The YAML system is accessed through the `GameStateRegistry` class:

```cpp
// Get a description for a specific flag
auto& registry = GameStateRegistry::instance();
std::string_view desc = registry.getMapDescription(
    MapTypes::KamikiVillage, 
    "worldStateBits", 
    42
);

// Get all config for a map
const auto& config = registry.getMapConfig(MapTypes::HanaValley);
```

### Performance Notes

- **YAML files are cached** after first load for performance
- **Thread-safe access** allows multiple systems to query simultaneously  
- **Lazy loading** means map configs only load when first accessed
- **Validation runs at build time**, not runtime, so it doesn't affect game performance

## Troubleshooting

### Common Issues

**"Empty description returned"**
- Check that the YAML file exists for that map
- Verify the category name matches exactly (case-sensitive)
- Ensure the bit index exists in the file

**"YAML validation failed"**
- Check indentation (use spaces, not tabs)
- Verify all entries have the format `number: "description"`
- Don't quote the numbers, only the descriptions

**"Changes not appearing in-game"**
- Restart the game - YAML files only load at startup
- Check the console for parsing errors
- Verify file permissions allow reading
