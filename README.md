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

4. Running the mod is different depending on your platform. See **Usage**, below.

**Note**: The mod produces debug logs in `Okami/logs/`, date and timestamped. Providing a log will make it a lot easier to diagnose any issues you're having!

## Usage

Depending on your platform, launching the mod is a little different:

### Windows

Simply run `okami-loader.exe`. The loader will launch the game if needed, inject the mod, then close.

### Linux / Steam Deck

Running the mod on Linux via Proton is a little more involved:

1. Ensure you have the `protontricks` and `wine` packages installed.
2. Make sure Okami is set to use Proton 9.0-4
    1. Right-click on game in library
    2. Click `Properties...`
    3. Click `Compatibility`
    4. Check `Force Use of a specific Steam compatibility Tool`
    5. Select `Proton 9.0-4` from the dropdown
3. Launch the game through Steam
4. **After the game window appears**, open a terminal, `cd` to your okami install directory, and execute the following:

    ```bash
    protontricks-launch --appid 587620 okami-loader.exe
    ```

5. The loader should print some stuff in the terminal, then exit. You should now see the mod's ImGui overlay on the game window.

The reason `protontricks` is required is to place the okami-loader.exe process in the same Wine prefix as the game. This allows the two processes to interact with one another in the DOS-like environment (and the loader to inject the mod into the game). `587620` is Okami HD's Steam App ID.

### Useful Keybindings

- `END`, `ALT`, and `SUPER` (`WIN`) will unlock your cursor from the game
- `HOME` will toggle the visibility of the entire ImGui UI
- `F2` will toggle the Archipelago login window (it won't appear if the entire UI is invisible)

## Building From Source

### Prerequisites

- **Visual Studio 2019/2022** (Or Windows SDK + CLang/MSVC separately)
  - When cross-compiling from Linux, you will instead need `mingw64-gcc-c++` and `mingw64-winpthreads-static`
- **CMake 3.21+**
- **Ninja**
- **Your favorite text editor** (hopefully one with CMake support to make your life easier)

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
          "CMAKE_MAKE_PROGRAM": "C:/path/to/ninja/ninja.exe",
          "CMAKE_INSTALL_PREFIX": "C:/path/to/Okami"
        }
      }
    ]
  }
  ```

This example also shows how to override the install path to your own okami install directory so that cmake install targets work on your machine.

To build:

  ```bash
  # Debug build
  cmake --preset x64-clang-debug
  cmake --build --preset x64-clang-debug

  # Release build  
  cmake --preset x64-clang-release
  cmake --build --preset x64-clang-release
  ```

Of course, if you're cross-compiling, use the cross-compile targets.

## Contributing

Contributions are welcome! Please:

1. Read [CONTRIBUTING.md](CONTRIBUTING.md)
2. Fork the repository
3. Create a feature branch from `master`
4. Test your changes
5. Run `format.sh` (use Git Bash on Windows)
   1. CI uses `clang-format` v20. You probably want to match this to ensure you're formatting the same as the CI does.
6. Submit a pull request

## Project Structure

- `src/client/`: Main mod DLL
- `src/library/`: Library for game memory offsets, types, and structures
- `src/loader/`: Okami Injector and DLL loader
- `include/okami/`: Game-specific helpers, memory structures, and initializations
- `external/`: Dependency Git submodules
- `cmake/`: Build system utilities

## Acknowledgements

- All of the wonderful contributors to the [Okami Reverse Engineering Wiki](https://okami.speedruns.wiki/Reverse_Engineering)
- Loader and APClient icon: **@sidorak26** on the Archipelago Discord

## License

This project is under the MIT License - see the [LICENSE](LICENSE) file for details.
