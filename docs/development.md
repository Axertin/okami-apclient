# Development Guide

This guide covers the technical details of developing okami-apclient.

## Prerequisites

### Required Tools
- **Visual Studio 2019/2022** with C++ development workload
  - Or Windows SDK + Clang/MSVC separately
- **CMake 3.21+**
- **Ninja** build system
- **Git** with submodules support

### Recommended: VSCode Setup
For the easiest development experience:

1. **Install VS2022** with C++ development workload (for compiler toolchain)
   - You can also install Ninja, Clang, and the Windows SDK separately, if you prefer.
2. **Install VSCode** or VSCodium, if you prefer
3. **Open folder** in VSCode → repo root directory
   - VSCode will prompt to install recommended extensions - click "Install All"
4. **CMake: Configure** → select preset (`x64-clang-debug`, or your user preset, or similar)
5. **CMake: Build** → select target or "all"

VSCode automatically hides dependency targets and provides a clean interface.

### Alternative: Visual Studio
If you prefer Visual Studio IDE...

When opening the project in Visual Studio, accept the `.vsconfig` prompt to install the Clang toolchain. If it doesn't prompt you, close and reopen the project after the initial load.

For more specific Visual Studio guidance, see [Visual Studio Setup](#visual-studio-setup).

## Building

### Before You Start - Verification
Run these commands to verify your setup:
- `ninja --version` (should show "1.10+" or similar, not "command not found")
- `git --version` 
- `cmake --version` (should show "3.21+" or higher)

### Quick Start
```bash
git clone --recursive https://github.com/Axertin/okami-apclient.git
cd okami-apclient
cmake --preset x64-clang-debug
cmake --build --preset x64-clang-debug
```

### Build Presets
- `x64-clang-debug` - Debug build with Clang
- `x64-clang-release` - Release build with Clang
- `x64-debug` - Debug build with MSVC
- `x64-release` - Release build with MSVC

### Custom Ninja Path
If Ninja isn't in your PATH, create `CMakeUserPresets.json`:
```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "my-local-debug",
      "inherits": "x64-clang-debug",
      "cacheVariables": {
        "CMAKE_MAKE_PROGRAM": "D:/path/to/ninja/ninja.exe"
      }
    }
  ]
}
```

### Visual Studio Setup (Detailed)

Visual Studio's CMake integration can be tricky with dependency-heavy projects. Follow these steps for the best experience:

#### Initial Setup
1. **Open Visual Studio** (if opening from start menu, select "Continue without code")
2. **Open the project**: File → Open → CMake... → select the repository's root `CMakeLists.txt`
   - **Do NOT** open any `.sln` files in the `external/` folder
3. **Accept the `.vsconfig` prompt** to install the Clang toolchain
   - If no prompt appears, close and reopen the project after initial configuration

#### Configuration and Building
3. **Wait for initial configure** to complete (watch the Output window)
4. **Select build preset** in the toolbar: choose `x64-clang-debug` or `x64-clang-release`
5. **Select build target** in the dropdown next to the build button:
   - **Main targets**: `okami-apclient`, `okami-loader`, `okami-tests`
   - **Ignore**: All the dependency targets (imgui examples, vcpkg packages, etc.)
   - **Tip**: The target dropdown will be very long due to dependencies - scroll to find your targets

#### Target Selection Tips
- **Default startup**: The first target alphabetically will be selected initially
- **Main targets to use**:
  - `okami-loader` - The injector executable
  - `okami-apclient` - The main mod DLL
  - `okami-tests` - Unit tests
- **Avoid**: Any targets with names like `example_*`, `*_test`, or vcpkg package names

#### Building
- **Single target**: Select target → press F7 or click Build
- **All targets**: Use the command line instead: `cmake --build --preset x64-clang-debug`
- **Clean build**: Build → Clean All, then rebuild

#### Troubleshooting
- **"Target not found" errors**: Make sure you selected the root `CMakeLists.txt`, not a subdirectory
- **Clang not working**: Ensure the Clang toolchain is installed via the VS Installer
- **Too many targets**: This is normal - dependencies expose many targets that can't be hidden in VS
- **Git submodules missing**: Run `git submodule update --init --recursive`
- **vcpkg errors**: Delete `out/` folder and reconfigure

## Code Style & Formatting

### Automatic Formatting
We use clang-format for consistent code style. **Always run before committing:**
```bash
./format.sh
```
Use Git Bash on Windows if needed.

### Code Standards
- **C++23** standard
- **Descriptive names** - Prefer descriptive names versus super short names
- **Type safety** - Prefer `MemoryAccessor<T>` operations over raw pointers
- **Cross-Platform Support** - Try to avoid certain Windows API calls which aren't implemented in Proton

## Project Structure

### Key Directories
- `src/client/` - Main mod DLL code
- `src/library/` - Library for game memory offsets, types, and structures
- `src/loader/` - Okami Injector and DLL loader
- `include/okami/` - Game-specific helpers, memory structures, and initializations
- `external/` - Dependency Git submodules
- `cmake/` - Build system utilities
- `test/` - Unit tests

### Target Architecture
- **okami-apclient.dll** - Main mod injected into the game (shared library)
- **okami-loader.exe** - Injector that loads the DLL
- **okami-library.lib** - Static library containing game-specific code
- **okami-tests.exe** - Unit test runner

## Dependencies

Dependencies are managed through vcpkg and git submodules:
- **vcpkg packages** - Standard libraries (nlohmann-json, asio, openssl, minhook, etc.)
- **Git submodules** - Custom dependencies (apclientpp, wswrap, websocketpp, imgui)

Key dependencies from the project:
- [apclientpp](https://github.com/black-sliver/apclientpp) - Archipelago client library
- [wswrap](https://github.com/black-sliver/wswrap) - WebSocket wrapper
- [websocketpp](https://github.com/zaphoyd/websocketpp) - WebSocket implementation
- [imgui](https://github.com/ocornut/imgui) - Immediate mode GUI

All dependencies are automatically handled by the build system.

## Memory Management Guidelines

### Game Memory Access
- Use `MemoryAccessor<T>` for typed memory access
- **Never assume memory is valid** - always check bounds
- **Keep game-specific code** in the library target unless directly Archipelago-related

## Testing

### Unit Tests
- Run tests with: `cmake --build --preset x64-clang-debug && ./out/build/x64-clang-debug/okami-tests.exe`
- Tests use Catch2 framework
- **Write tests** for memory access logic and data structures where possible
- **Don't worry about coverage** for game integration code that can't be easily mocked

### Manual Testing
Minimum testing requirements:
1. **Builds without errors**
2. **Mod loads in-game** without crashing
3. **Feature works as intended**
4. **Code is formatted** (`./format.sh` run)

### Test Game Setup
- Use any Okami HD save file
- Test with Archipelago connection if networking-related
- Check console output in the mod for errors

### Log Files
Debug logs are automatically saved to `logs/` directory with timestamps. Check logs when debugging issues.

## Common Development Tasks

### Adding New Memory Mappings
1. Define structures in appropriate header (e.g., `include/okami/data/structs.hpp`)
2. Add MemoryAccessor declarations to `include/okami/memorymap.hpp`
3. Add binding calls to `initVariables()` in `src/library/memorymap.cpp`
4. Update relevant data mappings in `devdatamapdata.cpp` if documented

### Adding New GUI Windows
1. Inherit from `Window` class
2. Implement `draw()` method with ImGui calls
3. Add to window list in `gui.cpp`

### Modifying Game Hooks
- Game function hooks are in `gamehooks.cpp`
- Use MinHook for function interception
- Always call original function unless intentionally blocking

## Performance Considerations

- **Minimize allocations** in hot paths (game loop, memory monitoring)
- **Use static containers** where possible
- **Do not stall the main thread (render(), onGameTick(), etc), ever!** Spawn another thread if you need to.

## CI/CD Integration

### GitHub Actions
- Builds run on Windows with multiple configurations
- Tests are automatically executed
- Artifacts are generated for releases
- **Ensure formatting** - CI will fail if code isn't formatted properly

## Getting Help

- **Build issues** - Check vcpkg and CMake output
- **Runtime crashes** - Check log files and use debugger
- **Community discussion** - [Archipelago Discord](https://discord.com/channels/731205301247803413/1196620860405067848)
