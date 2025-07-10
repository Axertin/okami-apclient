# Contributing to Okami APClient

Thanks for your interest in contributing! There are many ways to help improve this project, whether you write code or not.

## Ways to Contribute

### **Found a Bug?**
Use our [Bug Report template](https://github.com/Axertin/okami-apclient/issues/new?template=bug_report.md) to let us know what's wrong.

### **Discovered Memory Mappings?**
Help us understand Okami's save data! Use the [Memory ID Mapping template](https://github.com/Axertin/okami-apclient/issues/new?template=memory_id_mapping.md) to report what game flags mean.

### **Want a New Feature?**
Use our [Feature Request template](https://github.com/Axertin/okami-apclient/issues/new?template=feature_request.md) to suggest improvements to the client.

### **Discussion & Help**
Join us in the [Archipelago Discord](https://discord.com/channels/731205301247803413/1196620860405067848) for general discussion and questions.

### **Want to Code?**
Keep reading! We'd love your help.

## Getting Started with Development

### Prerequisites
- Visual Studio 2019/2022 (or Windows SDK + Clang/MSVC)
- CMake 3.21+
- Ninja
- Git with submodules support

### Quick Setup
```bash
git clone --recursive https://github.com/Axertin/okami-apclient.git
cd okami-apclient
cmake --preset x64-clang-debug
cmake --build --preset x64-clang-debug
```

See the [README](README.md) for detailed build instructions.

## Code Contributions

### Before You Start
1. **Check existing issues** - Someone might already be working on it
2. **Open an issue first** for larger changes to discuss the approach
3. **Fork the repository** or create a feature branch

### Development Workflow
1. **Create a branch** - `git checkout -b feature/your-feature-name`
2. **Make your changes** - Keep commits focused and atomic
3. **Format your code** - Run `./format.sh` (use Git Bash on Windows)
4. **Test your changes** - Make sure the mod loads and works as expected
5. **Submit a pull request** - Use the PR template when available

### Code Guidelines
- **Formatting**: We use clang-format. Run `./format.sh` before committing
- **Testing**: Ensure your changes don't crash the game. Unit tests are welcome where they make sense
- **Architecture**: Keep game-memory specific code in the library target unless it's directly Archipelago-related
- **Documentation**: Update relevant docs if you're changing behavior

### What to Test
- Mod loads without crashing
- Your feature works as intended
- Existing functionality still works
- Connection to Archipelago servers (if networking-related)

## Detailed Documentation
For more in-depth information, check the `docs/` folder:
- **[Development Guide](docs/development.md)** - Detailed setup, architecture, and coding standards
- **[Memory Mapping Guide](docs/memory-mapping.md)** - How to contribute reverse engineering discoveries

## Community Guidelines

- **Be respectful** - We're all here to have fun with Okami and Archipelago
- **Be patient** - This is a volunteer project, responses may take time
- **Search first** - Check if your issue or question has been addressed before
- **Provide context** - Help us understand your problem or suggestion

## Questions?

- **General discussion**: [Archipelago Discord Thread](https://discord.com/channels/731205301247803413/1196620860405067848)
- **Bug reports**: Use the issue templates
- **Development questions**: Open a discussion or ask in Discord

Thanks for contributing!
