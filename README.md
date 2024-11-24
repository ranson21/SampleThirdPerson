# Sample Third Person Game

This is an Unreal Engine 5.5 third-person game project developed on Linux.

## Prerequisites

- Unreal Engine 5.5 (built from source on Linux)
- Git
- build-essential package
- CMake (version 3.10 or higher)
- Required dependencies for Unreal Engine:
  ```bash
  sudo apt-get update && sudo apt-get install \
    build-essential \
    mono-complete \
    git \
    cmake \
    python3 \
    python3-pip \
    clang \
    libcurl4-openssl-dev \
    libssl-dev \
    ninja-build \
    pkg-config \
    libc++-dev \
    libc++abi-dev
  ```

## Getting Started

### First Time Setup

1. Clone the repository:
   ```bash
   git clone https://github.com/ranson21/SampleThirdPerson.git
   cd SampleThirdPerson
   ```

2. Generate project files:
   ```bash
   # Replace with your UE path
   ~/UnrealEngine/Engine/Build/BatchFiles/Linux/GenerateProjectFiles.sh SampleThirdPerson.uproject
   ```

3. Build the project:
   ```bash
   # For development build
   make SampleThirdPersonEditor

   # Or for development build with faster compilation
   make SampleThirdPersonEditor -j$(nproc)
   ```

### Running the Project

1. Launch the editor:
   ```bash
   ./SampleThirdPerson.uproject
   ```

   Or from command line:
   ```bash
   ~/UnrealEngine/Engine/Binaries/Linux/UnrealEditor SampleThirdPerson.uproject
   ```

## Development Workflow

### Building

- Development build:
  ```bash
  make SampleThirdPersonEditor
  ```

- Clean build:
  ```bash
  make clean
  make SampleThirdPersonEditor
  ```

### Common Issues

1. **Missing Modules**: If you get missing modules errors, try:
   ```bash
   # Regenerate project files
   ~/UnrealEngine/Engine/Build/BatchFiles/Linux/GenerateProjectFiles.sh SampleThirdPerson.uproject
   ```

2. **Build Errors**: Make sure all dependencies are installed and you're using the correct version of Unreal Engine.

3. **Editor Crashes**: Check the logs in `~/UnrealEngine/Engine/Saved/Logs/`

### VSCode Setup (Optional)

1. Install recommended extensions:
   - C/C++
   - C++ Intellisense
   - Unreal Engine 4 Snippets
   - EditorConfig for VS Code

2. Setup includes in `.vscode/c_cpp_properties.json`:
   ```json
   {
       "configurations": [
           {
               "name": "Linux",
               "includePath": [
                   "${workspaceFolder}/**",
                   "~/UnrealEngine/Engine/Source/**"
               ],
               "defines": [],
               "compilerPath": "/usr/bin/clang",
               "cStandard": "c17",
               "cppStandard": "c++17",
               "intelliSenseMode": "linux-clang-x64"
           }
       ],
       "version": 4
   }
   ```

## Project Structure

- `Source/` - C++ source code
- `Content/` - Game content and assets
- `Config/` - Project configuration files

## Contributing

1. Create a new branch for your feature
2. Make your changes
3. Test thoroughly
4. Submit a pull request

## Performance Tips

- Use `make -j$(nproc)` for parallel compilation
- Consider using a RAMDisk for the Engine's DerivedDataCache
- If using VSCode, add the Engine's source directories to the exclude list to improve performance

## Debugging

1. Launch with logging:
   ```bash
   ./SampleThirdPerson.uproject -log
   ```

2. Enable verbose logging:
   ```bash
   ./SampleThirdPerson.uproject -verbose
   ```

3. Debug build:
   ```bash
   make SampleThirdPersonEditor BUILDCONFIG=Debug
   ```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

**Abby Ranson**

- GitHub: [@ranson21](https://github.com/ranson21)
- Email: abby@abbyranson.com