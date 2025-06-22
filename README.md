# Okami-apclient

A mod for the Steam release of Ōkami HD that adds [Archipelago](https://archipelago.gg) support and integration, to be used in tandem with the [Okami APWorld](https://github.com/Ragmoa/Archipelago/tree/test_logic).

**Note**: This mod is in active development. Instability, bugs, and missing features are to be expected.

## Installing

1. Download the latest release from [the releases page](https://github.com/Axertin/okami-apclient/releases)
2. Unzip the release into the game's installation directory
3. The file structure should look like this:

   ```
   Okami/
   ├── mods/
   │   └── apclient/
   │       ├── okami-apclient.dll
   │       └── cacert.pem
   ├── okami-loader.exe
   ├── okami.exe (and the rest of the vanilla files)
   ```

4. Run `okami-loader.exe`, and you're off!

**Note**: The mod produces debug logs in `Okami/logs/`, date and timestamped. Providing a log will make it a lot easier to diagnose any issues you're having!

## Building From Source

### Prerequisites

- **Visual Studio 2019/2022** (Or Windows SDK + CLang/MSVC separately)
- **CMake 3.21+**
- **Ninja**

When opening in Visual Studio, accept the .vsconfig prompt to install the Clang toolchain. VS may not prompt you until you re-open the project after loading it for the first time.

### Dependencies

- [vcpkg](https://github.com/microsoft/vcpkg)
- [apclientpp](https://github.com/black-sliver/apclientpp)
- [wswrap](https://github.com/black-sliver/wswrap)
- [websocketpp](github.com/zaphoyd/websocketpp)
- [imgui](github.com/ocornut/imgui)

This project uses [vcpkg](https://github.com/microsoft/vcpkg) for dependency management. Dependencies are automatically handled through the build system. Ensure you clone submodules recursively (`git clone --recursive git@github.com:Axertin/okami-apclient.git`).

### Building

If Ninja is already in your `PATH`, no setup is required. If you're using Visual Studio, just make sure the Ninja component is installed (it will usually be auto-detected). If Ninja is installed but not on your `PATH`, you can tell CMake where to find it using a personal `CMakeUserPresets.json`:

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
      },
      {
        "name": "my-local-release",
        "inherits": "x64-clang-release",
        "cacheVariables": {
          "CMAKE_MAKE_PROGRAM": "D:/path/to/ninja/ninja.exe"
        }
      }
    ]
  }
  ```

To build:

  ```bash
  # Debug build
  cmake --preset x64-clang-debug
  cmake --build --preset x64-clang-debug

  # Release build  
  cmake --preset x64-clang-release
  cmake --build --preset x64-clang-release
  ```

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch from `develop`
3. Submit a pull request

### Development Workflow

- `master`: Stable code and release candidates
- `develop`: Active development branch
- Feature branches should be created from `develop`

## Project Structure

- `src/client/`: Main mod DLL
- `src/loader/`: Okami Injector and DLL loader
- `include/okami/`: Game-specific helpers, memory structures, and initializations
- `external/`: Dependency Git submodules
- `cmake/`: Build system utilities

## License

This project is under the MIT License - see the [LICENSE](LICENSE) file for details.
