# Game State Registry

The Game State Registry is an authoratative, YAML-based system for managing the game state flag definitions and descriptions ("Meanings") in Okami.

## Overview

Okami HD tracks the majority of game state through thousands of bit flags organized in many different bitfields, generally grouped by relevant map or context. The Game State Registry provides a human-readable way to define and describe these bit flags, organized into:

- **Global State Flags**: Common across all maps (brushes, restored areas, etc.)
- **Map-Specific Flags**: Unique to individual game maps (events, collectibles, dialog, etc.)

## Architecture

### Core Components

- **`IgameStateRegistry`**: Interface defining the registry, to allow mocking.
- **`GameStateRegistry`**: Access and data controller, based on YAML files.
- **`MapStateConfig`**: Map-specific flags structure
- **`GlobalConfig`**: Global flag structure

### File Structure

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

## YAML Format

All YAML files follow a generally consistent format, where bitflags are organized into named categories:

```yaml
category_name:
    0: "description for flag 0"
    1: "description for flag 1"
    43: "description for flag 43"
    # And so on
```

### Adding New Flag Descriptions

When you discover what a flag does:

1. Test the flag thoroughly to understand its behavior
2. Choose a clear, descriptive name
3. Add it to the appropriate YAML file
4. Run validation to ensure correctness
5. Consider documenting the discovery process in commit messages

## Schema Verification

The registry uses JSON Schema validation to ensure the YAML files are correctly formatted and do not have erronous conditions (like duplicate indicies).

### Running Validation

```bash
python scripts/validate_game_data.py src/library/game-data/
```

## Troubleshooting

### Common Issues

**Empty descriptions returned**
- Check if the YAML file exists and contains the category
- Verify the bit index exists in the file
- Ensure the map name normalization is correct (alphanumeric only)

**Validation failures**
- Check YAML syntax (indentation, colons, quotes)
- Verify numeric indices are not quoted in YAML
- Ensure all required categories are present in global.yml

**Loading errors**
- Check file permissions and path accessibility
- Verify YAML-cpp library is properly linked
- Check console output for specific error messages
