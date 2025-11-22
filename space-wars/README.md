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

2. **Configure Network Settings:**
   - Copy the example configuration file: `cp config.example.txt config.txt`
   - Edit `config.txt` with your network settings (see Configuration section below)

3. Run the executable:
```bash
./build/bin/SpaceWars
```

4. The game will read the configuration file and attempt to connect automatically.

## Configuration

The game uses a configuration file (`config.txt`) to specify network settings. The configuration file must be located in one of the following places:

1. **Current working directory** - The directory from which you run the executable (recommended for development)
2. **Same directory as the executable** - Place `config.txt` next to the `SpaceWars` executable file (recommended for distribution)

**Example:** If you run the game with `./build/bin/SpaceWars`, the game will look for `config.txt` in:
- The current working directory (where you ran the command)
- Or `./build/bin/config.txt` (same directory as the executable)

### Configuration File Format

The configuration file uses a simple key-value format. Each line contains a key, an equals sign, and a value. Comments start with `#` and empty lines are ignored.

**Required fields:**
- `host_ip`: IP address where this player binds (receives messages)
- `host_port`: Port number where this player binds (receives messages)
- `client_ip`: IP address of the peer player (where to send messages)
- `client_port`: Port number of the peer player (where to send messages)

**Example configuration file (`config.txt`):**
```
host_ip=127.0.0.1
host_port=5555
client_ip=127.0.0.1
client_port=5556
```

### Network Setup Examples

#### Two Players on Same Computer (Localhost)

**Player 1's `config.txt`:**
```
host_ip=127.0.0.1
host_port=5555
client_ip=127.0.0.1
client_port=5556
```

**Player 2's `config.txt`:**
```
host_ip=127.0.0.1
host_port=5556
client_ip=127.0.0.1
client_port=5555
```

#### Two Players on Different Computers

**Player 1 (IP: 192.168.1.100) `config.txt`:**
```
host_ip=192.168.1.100
host_port=5555
client_ip=192.168.1.101
client_port=5556
```

**Player 2 (IP: 192.168.1.101) `config.txt`:**
```
host_ip=192.168.1.101
host_port=5556
client_ip=192.168.1.100
client_port=5555
```

**Note:** 
- Both players must be on the same network or have appropriate port forwarding configured
- Firewall settings must allow traffic on the specified ports
- The `host_ip` should be the IP address of the computer running the game
- The `client_ip` should be the IP address of the other player's computer
- Ports must be different for each player on the same computer

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
- Verify the `config.txt` file exists in the same directory as the executable
- Check that all required fields (host_ip, host_port, client_ip, client_port) are present and valid
- Verify both players are on the same network
- Check firewall settings (ports must be open)
- Ensure IP addresses and ports are correctly configured
- Try using `localhost` or `127.0.0.1` for local testing
- Check that ports are not already in use by another application

**Configuration file errors:**
- Ensure `config.txt` exists (copy from `config.example.txt` if needed)
- Verify IP addresses are in valid format (e.g., `192.168.1.100`)
- Verify ports are numbers between 1 and 65535
- Check for typos in key names (must be: host_ip, host_port, client_ip, client_port)

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

