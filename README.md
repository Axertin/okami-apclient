# Okami-apclient

A mod for the Steam release of Ōkami HD that adds [Archipelago](https://archipelago.gg) support and integration, to be used in tandem with the [Okami APWorld](https://github.com/Ragmoa/Archipelago/tree/test_logic).
This mod requires the [WOLF](https://github.com/Axertin/wolf) framework to function.

This project follows [Semantic Versioning 2.0.0](https://semver.org/):
- **v0.x.x** = Pre-release, expect breaking changes and missing core functionality
- **v1.0.0+** = Stable, public-sync-ready releases

**While aspects of the mod may be functional or even playable in their current states, it is not recommended to run this in AP syncs yet!**

## Installing

1. Download the latest release from [the releases page](https://github.com/Axertin/okami-apclient/releases)
2. Unzip the release into the mods/ directory, located in the game's installation directory
3. The file structure should look like this:

   ```
   Okami/
   ├── mods/
   │   └── apclient/
   │       ├── okami-apclient.dll
   │       └── cacert.pem
   ├── wolf-loader.exe
   ├── dinput8.dll
   ├── okami.exe (and the rest of the vanilla files)
   ```

4. Launch WOLF (reference [WOLF's quickstart](https://github.com/Axertin/wolf/blob/master/README.md) for exact instructions by platform)

### Useful Keybindings

- `END`, `ALT`, and `SUPER` (`WIN`) will unlock your cursor from the game
- `HOME` will toggle the visibility of the entire ImGui UI
- `F2` will toggle the Archipelago login window (it won't appear if the entire UI is invisible)

## Building From Source

For development setup and detailed build instructions, see [docs/development.md](docs/development.md).

### Quick Start
```bash
git clone --recursive https://github.com/Axertin/okami-apclient.git
cd okami-apclient
cmake --preset x64-clang-debug
cmake --build --preset x64-clang-debug
```

### Prerequisites
- **Visual Studio 2019/2022** or Windows SDK + Clang/MSVC
- **CMake 3.21+**, **Ninja**, **Git** with submodules support

Dependencies are automatically handled through vcpkg and git submodules.

For first-time contributors, detailed setup instructions, and troubleshooting, see the [development guide](docs/development.md).

## Contributing

Contributions are welcome! Please:

1. Read [CONTRIBUTING.md](CONTRIBUTING.md) and [docs/development.md](docs/development.md)
2. Fork the repository
3. Create a feature branch from `master`
4. Test your changes and run `format.sh` (use Git Bash on Windows)
5. Submit a pull request

## Project Structure

- `src/okami-apclient/` - Main mod DLL
- `tests/` - Unit tests
- `scripts/` - Helper scripts for build-time code generation
- `include/` - WOLF SDK and other external includes
- `external/` - Dependency Git submodules
- `cmake/` - Build system utilities

## Acknowledgements

- All of the wonderful contributors to the [Okami Reverse Engineering Wiki](https://okami.speedruns.wiki/Reverse_Engineering)
- Loader and APClient icon: **@sidorak26** on the Archipelago Discord

## License

This project is under the MIT License - see the [LICENSE](LICENSE) file for details.
