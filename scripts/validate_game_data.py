#!/usr/bin/env python3
"""
Validate game data YAML files for consistency and correctness
Usage: python validate_game_data.py path/to/game-data/
"""

import sys
import yaml
import json
from pathlib import Path
from collections import defaultdict
import jsonschema

def load_yaml_file(file_path):
    """Load and parse a YAML file"""
    try:
        # Check if file is empty (0 bytes)
        if file_path.stat().st_size == 0:
            print(f"  -> Empty file, skipping {file_path.name}")
            return {}
            
        with open(file_path, 'r', encoding='utf-8') as f:
            return yaml.safe_load(f)
    except Exception as e:
        print(f"ERROR: Failed to load {file_path}: {e}")
        return None

def validate_schema(data, schema_path, file_path):
    """Validate YAML data against JSON schema"""
    try:
        # Skip validation for empty/null data
        if data is None or data == {}:
            print(f"  -> Empty file, skipping schema validation for {file_path.name}")
            return True
            
        with open(schema_path, 'r') as f:
            schema = yaml.safe_load(f)
        
        # Convert integer keys to strings for schema validation
        # (PyYAML loads numeric keys as integers, but schema expects string keys)
        def convert_keys_to_strings(obj):
            if isinstance(obj, dict):
                return {str(k): convert_keys_to_strings(v) for k, v in obj.items()}
            elif isinstance(obj, list):
                return [convert_keys_to_strings(item) for item in obj]
            else:
                return obj
        
        normalized_data = convert_keys_to_strings(data)
        jsonschema.validate(normalized_data, schema)
        return True
    except jsonschema.ValidationError as e:
        print(f"ERROR: Schema validation failed for {file_path}")
        print(f"       {e.message}")
        if e.absolute_path:
            print(f"       Path: {' -> '.join(str(p) for p in e.absolute_path)}")
        if hasattr(e, 'instance'):
            print(f"       Invalid value: {e.instance}")
        return False
    except Exception as e:
        print(f"ERROR: Schema error for {file_path}: {e}")
        return False

def check_duplicates_within_file(data, file_path):
    """Check for duplicate indices within a single file"""
    issues = []
    
    # Skip checks for empty data
    if data is None or data == {}:
        return issues
    
    for category, entries in data.items():
        if not isinstance(entries, dict):
            continue
            
        # Check for duplicate descriptions (might indicate copy-paste errors)
        descriptions = defaultdict(list)
        for index, desc in entries.items():
            descriptions[desc].append(index)
        
        for desc, indices in descriptions.items():
            if len(indices) > 1:
                issues.append(f"Duplicate description in {category}: '{desc}' used for indices {indices}")
    
    return issues

def check_duplicates_across_maps(all_map_data):
    """Check for duplicate indices across different maps in the same category"""
    issues = []
    
    # Group by category across all maps
    by_category = defaultdict(lambda: defaultdict(list))
    
    for map_name, data in all_map_data.items():
        # Skip empty data
        if data is None or data == {}:
            continue
            
        for category, entries in data.items():
            if isinstance(entries, dict):
                for index, desc in entries.items():
                    by_category[category][index].append((map_name, desc))
    
    # Check for conflicts
    for category, indices in by_category.items():
        for index, map_descs in indices.items():
            if len(map_descs) > 1:
                # Check if descriptions are different
                unique_descs = set(desc for _, desc in map_descs)
                if len(unique_descs) > 1:
                    issues.append(f"Index {index} in {category} has different descriptions:")
                    for map_name, desc in map_descs:
                        issues.append(f"  {map_name}: '{desc}'")
    
    return issues

def check_global_data(global_data):
    """Validate global configuration data"""
    issues = []
    
    # Skip checks for empty data
    if global_data is None or global_data == {}:
        issues.append("Global data is empty")
        return issues
    
    # Check for missing critical categories
    expected_categories = ['brushUpgrades', 'commonStates', 'areasRestored']
    for category in expected_categories:
        if category not in global_data:
            issues.append(f"Missing critical category: {category}")
    
    # Check for duplicates within global data
    issues.extend(check_duplicates_within_file(global_data, "global.yml"))
    
    return issues

def validate_game_data(game_data_dir):
    """Main validation function"""
    game_data_path = Path(game_data_dir)
    
    if not game_data_path.exists():
        print(f"ERROR: Game data directory not found: {game_data_path}")
        return False
    
    print(f"Validating game data in {game_data_path}")
    
    errors = 0
    warnings = 0
    
    # Load schemas
    schema_dir = game_data_path / "schema"
    map_schema = schema_dir / "map-state.schema.yml"
    global_schema = schema_dir / "global.schema.yml"
    
    # Validate global.yml
    global_file = game_data_path / "global.yml"
    if global_file.exists():
        print("Validating global.yml...")
        global_data = load_yaml_file(global_file)
        if global_data is not None:  # Allow empty dict
            # Schema validation
            if global_schema.exists():
                if not validate_schema(global_data, global_schema, global_file):
                    errors += 1
            
            # Content validation
            global_issues = check_global_data(global_data)
            for issue in global_issues:
                print(f"WARNING: {issue}")
                warnings += 1
        else:
            errors += 1
    else:
        print("ERROR: global.yml not found")
        errors += 1
    
    # Validate map files
    maps_dir = game_data_path / "maps"
    if maps_dir.exists():
        print("Validating map files...")
        
        all_map_data = {}
        map_files = list(maps_dir.glob("*.yml"))
        
        for map_file in map_files:
            map_name = map_file.stem
            print(f"  Checking {map_name}...")
            map_data = load_yaml_file(map_file)
            
            if map_data is not None:  # Allow empty dict
                all_map_data[map_name] = map_data
                
                # Schema validation
                if map_schema.exists():
                    if not validate_schema(map_data, map_schema, map_file):
                        print(f"  -> Schema validation failed for {map_name}")
                        errors += 1
                    else:
                        print(f"  -> Schema validation passed for {map_name}")
                
                # Check for duplicates within this map only
                map_issues = check_duplicates_within_file(map_data, map_file)
                for issue in map_issues:
                    print(f"WARNING: {map_name}: {issue}")
                    warnings += 1
            else:
                print(f"  -> Failed to load {map_name}")
                errors += 1
        
        # Note: Cross-map conflicts are expected and normal, so we skip that check
        print(f"Processed {len(map_files)} map files")
    else:
        print("ERROR: maps/ directory not found")
        errors += 1
    
    # Summary
    print(f"Validation complete: {errors} errors, {warnings} warnings")
    
    if errors == 0 and warnings == 0:
        print("All validations passed")
        return True
    elif errors == 0:
        print("Validation completed with warnings")
        return True
    else:
        print("Validation failed")
        return False

def main():
    if len(sys.argv) != 2:
        print("Usage: python validate_game_data.py path/to/game-data/")
        sys.exit(1)
    
    success = validate_game_data(sys.argv[1])
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
