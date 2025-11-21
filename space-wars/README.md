# Space Wars Game

A cross-platform multiplayer arcade game inspired by the 1970's Space Wars arcade game. Two players control spacecraft on separate computers, connected via ZeroMQ messaging, with synchronized gameplay across both screens.

## Features

- Two-player networked gameplay
- Real-time synchronized game state
- Retro vector graphics style
- Smooth spacecraft controls (turn, thrust, fire)
- Visual effects (thrust flames, explosions)
- Screen wrapping for continuous gameplay
- First to 5 points wins

## Requirements

### Dependencies

- **C++23 compatible compiler**
  - macOS: Clang 16+ or GCC 13+
  - Ubuntu Linux: GCC 13+ or Clang 16+
- **SFML 2.5+** - Graphics and window management
- **ZeroMQ (libzmq)** - Network messaging
- **CMake 3.20+** - Build system
- **pkg-config** - For finding ZeroMQ (usually pre-installed on Linux/macOS)

### Platform Support

- macOS 10.15+
- Ubuntu Linux (20.04+)

## Installation

### macOS

#### Using Homebrew (Recommended)

```bash
# Install SFML
brew install sfml

# Install ZeroMQ
brew install zeromq

# Install CMake (if not already installed)
brew install cmake
```

#### Manual Installation

If you prefer to build from source or use other package managers, ensure SFML and ZeroMQ are installed and accessible via CMake/pkg-config.

### Ubuntu Linux

```bash
# Update package list
sudo apt update

# Install build tools and dependencies
sudo apt install build-essential cmake pkg-config

# Install SFML
sudo apt install libsfml-dev

# Install ZeroMQ
sudo apt install libzmq3-dev
```

## Building

### Standard Build

1. Navigate to the project directory:
```bash
cd space-wars
```

2. Create a build directory:
```bash
mkdir build
cd build
```

3. Configure with CMake:
```bash
cmake ..
```

4. Build the project:
```bash
cmake --build .
```

Or use make directly:
```bash
make
```

5. The executable will be in `build/bin/SpaceWars`

### Build Options

- **Release build (default):**
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

- **Debug build:**
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

- **Specify install prefix:**
```bash
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
```

## Running

1. Build the project (see Building section above)

2. Run the executable:
```bash
./build/bin/SpaceWars
```

3. **Network Setup:**
   - Each player launches the game on their computer
   - When prompted, enter the IP address and port of the other player's computer
   - The game will establish a connection and synchronize game state

### Example Network Configuration

- **Player 1:** Enter Player 2's IP address (e.g., `192.168.1.100`) and port (e.g., `5555`)
- **Player 2:** Enter Player 1's IP address (e.g., `192.168.1.101`) and port (e.g., `5555`)

**Note:** Both players must be on the same network or have appropriate port forwarding configured.

## Controls

- **Arrow Keys:**
  - Left Arrow: Turn spacecraft left
  - Right Arrow: Turn spacecraft right
  - Up Arrow: Apply thrust (move forward)

- **Spacebar:** Fire weapon

## Gameplay

- Two spacecraft start at the center of the screen, facing away from each other
- Use thrust to move and turn to aim
- Fire projectiles to hit your opponent
- Score increases by 1 when you hit your opponent
- Destroyed spacecraft respawn at a random location
- First player to reach 5 points wins
- Spacecraft wrap around screen edges (continuous playing field)
- Projectiles disappear when they reach screen edges

## Troubleshooting

### Build Issues

**SFML not found:**
- Ensure SFML is installed (see Installation section)
- On macOS with Homebrew, CMake should find it automatically
- On Linux, ensure `libsfml-dev` is installed

**ZeroMQ not found:**
- Ensure ZeroMQ is installed (see Installation section)
- Verify `pkg-config` can find it: `pkg-config --modversion libzmq`

**C++23 not supported:**
- Update your compiler to a version that supports C++23
- GCC 13+ or Clang 16+ required

### Runtime Issues

**Network connection fails:**
- Verify both players are on the same network
- Check firewall settings (port must be open)
- Ensure correct IP address and port are entered
- Try using `localhost` or `127.0.0.1` for local testing

**Game runs slowly:**
- Ensure you're using a Release build (not Debug)
- Check that graphics drivers are up to date

## Development

### Project Structure

```
space-wars/
├── CMakeLists.txt      # Build configuration
├── README.md           # This file
└── src/                # Source code
    ├── main.cpp        # Entry point
    └── ...             # Other source files
```

### Building for Development

For development with debugging symbols:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

## License

[Add license information here]

## Contributing

[Add contributing guidelines here]

## Version

Version 0.1

