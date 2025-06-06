# Okami-apclient


### Building the Project

This project uses [vcpkg](https://github.com/microsoft/vcpkg), CMake presets, Ninja, and Clang. When opening in Visual Studio, accept the .vsconfig prompt to install the Clang toolchain.

If Ninja is already in your `PATH`, no setup is required. If you're using Visual Studio, just make sure the Ninja component is installed (it will usually be auto-detected). If Ninja is installed but not on your `PATH`, you can tell CMake where to find it using:

- A personal `CMakeUserPresets.json`:
  ```json
  {
    "version": 3,
    "configurePresets": [
      {
        "name": "my-local-debug",
        "inherits": "x64-clang-debug",
        "cacheVariables": {
          "CMAKE_MAKE_PROGRAM": "D:/Tools/Ninja/ninja.exe"
        }
      },
      {
        "name": "my-local-release",
        "inherits": "x64-clang-release",
        "cacheVariables": {
          "CMAKE_MAKE_PROGRAM": "D:/Tools/Ninja/ninja.exe"
        }
      }
    ]
  }
- An environment variable:
    ```$env:CMAKE_MAKE_PROGRAM = "<Path/To/Ninja>/ninja.exe"```

To build:

```bash
cmake --preset x64-clang-debug
cmake --build --preset x64-clang-debug
```