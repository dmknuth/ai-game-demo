# Task List: Space Wars Game Implementation

Based on PRD: `prd-space-wars.md`

## Relevant Files

- `CMakeLists.txt` - Build configuration file for CMake, includes SFML and ZeroMQ dependencies
- `README.md` - Project documentation with setup and build instructions
- `src/main.cpp` - Application entry point, initializes and runs the game
- `src/Game.h` - Main game class header, manages game loop and overall game state
- `src/Game.cpp` - Main game class implementation
- `src/Spacecraft.h` - Spacecraft entity class header, defines spacecraft properties and behavior
- `src/Spacecraft.cpp` - Spacecraft entity class implementation
- `src/Projectile.h` - Projectile entity class header, defines projectile properties and behavior
- `src/Projectile.cpp` - Projectile entity class implementation
- `src/GameState.h` - Game state management header, contains synchronized game state data
- `src/GameState.cpp` - Game state management implementation
- `src/NetworkManager.h` - Network communication header, handles ZeroMQ connections and messaging
- `src/NetworkManager.cpp` - Network communication implementation
- `src/Renderer.h` - Rendering system header, contains rendering functions and utilities
- `src/Renderer.cpp` - Rendering system implementation
- `src/InputHandler.h` - Input handling header, processes keyboard input
- `src/InputHandler.cpp` - Input handling implementation
- `src/Constants.h` - Game constants header, defines physics values, window size, etc.
- `src/ConfigReader.h` - Configuration file reader header, handles parsing network configuration
- `src/ConfigReader.cpp` - Configuration file reader implementation

### Notes

- This is a C++ project using SFML and ZeroMQ libraries
- All source files should be in the `src/` directory
- Header files (.h) and implementation files (.cpp) should be paired
- Use CMake for cross-platform build configuration
- Test compilation on both macOS and Ubuntu Linux

## Instructions for Completing Tasks

**IMPORTANT:** As you complete each task, you must check it off in this markdown file by changing `- [ ]` to `- [x]`. This helps track progress and ensures you don't skip any steps.

Example:
- `- [ ] 1.1 Read file` → `- [x] 1.1 Read file` (after completing)

Update the file after completing each sub-task, not just after completing an entire parent task.

## Tasks

- [x] 0.0 Create feature branch
  - [x] 0.1 Create and checkout a new branch for this feature (e.g., `git checkout -b feature/space-wars`)

- [x] 1.0 Project setup and build configuration
  - [x] 1.1 Create project directory structure (`src/` directory for source files)
  - [x] 1.2 Create `CMakeLists.txt` with C++23 standard configuration
  - [x] 1.3 Configure SFML library dependency in CMakeLists.txt (supports both SFML 2.5 and 3.0)
  - [x] 1.4 Configure ZeroMQ library dependency in CMakeLists.txt
  - [x] 1.5 Set up cross-platform build configuration (macOS and Ubuntu Linux)
  - [x] 1.6 Create `README.md` with project description, dependencies, and build instructions
  - [x] 1.7 Test that the project compiles successfully on macOS (if available)
  - [ ] 1.8 Test that the project compiles successfully on Ubuntu Linux (if available) - Pending Ubuntu system testing

- [x] 2.0 Core game engine and window management
  - [x] 2.1 Create `src/main.cpp` with basic application structure
  - [x] 2.2 Create `src/Game.h` header file with Game class declaration
  - [x] 2.3 Create `src/Game.cpp` with Game class implementation
  - [x] 2.4 Initialize SFML window in Game class (default size: 1024x768 or as specified)
  - [x] 2.5 Implement main game loop structure (input → update → render)
  - [x] 2.6 Handle window close events
  - [x] 2.7 Implement frame rate limiting (target 60 FPS)
  - [x] 2.8 Add delta time calculation for frame-independent updates

- [x] 3.0 Game entities and physics system
  - [x] 3.1 Create `src/Constants.h` with game constants (window size, physics values, etc.)
  - [x] 3.2 Create `src/Spacecraft.h` header file with Spacecraft class declaration
  - [x] 3.3 Create `src/Spacecraft.cpp` with Spacecraft class implementation
  - [x] 3.4 Implement spacecraft position and orientation properties
  - [x] 3.5 Implement spacecraft rotation mechanics (left/right arrow keys)
  - [x] 3.6 Implement spacecraft thrust mechanics (up arrow key) with velocity and acceleration
  - [x] 3.7 Implement screen wrapping for spacecraft (wrap to opposite edge when reaching boundaries)
  - [x] 3.8 Create `src/Projectile.h` header file with Projectile class declaration
  - [x] 3.9 Create `src/Projectile.cpp` with Projectile class implementation
  - [x] 3.10 Implement projectile position and velocity properties
  - [x] 3.11 Implement projectile movement along its direction vector
  - [x] 3.12 Implement projectile edge detection (disappear when reaching screen edge)
  - [x] 3.13 Create `src/GameState.h` header file with GameState class declaration
  - [x] 3.14 Create `src/GameState.cpp` with GameState class implementation
  - [x] 3.15 Implement GameState to hold both spacecraft, projectiles, and scores
  - [x] 3.16 Implement game initialization logic (spawn both spacecraft at center, facing away from each other)

- [x] 4.0 Network communication layer
  - [x] 4.1 Create `src/NetworkManager.h` header file with NetworkManager class declaration
  - [x] 4.2 Create `src/NetworkManager.cpp` with NetworkManager class implementation
  - [x] 4.3 Initialize ZeroMQ context and sockets
  - [x] 4.4 Implement connection establishment function (accept IP address and port input)
  - [x] 4.5 Design message format for game state synchronization (JSON or binary)
  - [x] 4.6 Implement serialization function to convert GameState to message format
  - [x] 4.7 Implement deserialization function to convert message to GameState
  - [x] 4.8 Implement sending game state updates to peer
  - [x] 4.9 Implement receiving game state updates from peer (non-blocking)
  - [x] 4.10 Implement network disconnection detection
  - [x] 4.11 Implement reconnection logic when connection is lost
  - [x] 4.12 Integrate NetworkManager into Game class for state synchronization

- [x] 4.13 Configuration file support for network settings
  - [x] 4.13.1 Create `src/ConfigReader.h` header file with ConfigReader class declaration
  - [x] 4.13.2 Create `src/ConfigReader.cpp` with ConfigReader class implementation
  - [x] 4.13.3 Design configuration file format (INI-style or key-value pairs: host_ip, host_port, client_ip, client_port)
  - [x] 4.13.4 Implement function to read configuration file from disk
  - [x] 4.13.5 Implement parsing logic for configuration file format
  - [x] 4.13.6 Implement validation for IP address and port number formats
  - [x] 4.13.7 Implement error handling for missing configuration file
  - [x] 4.13.8 Implement error handling for invalid or malformed configuration file
  - [x] 4.13.9 Update NetworkManager to accept configuration parameters (host_ip, host_port, client_ip, client_port)
  - [x] 4.13.10 Update main.cpp or Game class to read configuration file at application launch
  - [x] 4.13.11 Pass configuration values to NetworkManager for connection setup
  - [x] 4.13.12 Display error message and exit gracefully if configuration file is missing or invalid
  - [x] 4.13.13 Create example configuration file template (e.g., `config.example.txt`)
  - [x] 4.13.14 Update README.md with configuration file format documentation

- [x] 5.0 Rendering system and visual effects
  - [x] 5.1 Create `src/Renderer.h` header file with rendering function declarations
  - [x] 5.2 Create `src/Renderer.cpp` with rendering function implementations
  - [x] 5.3 Implement function to draw spacecraft as line drawing (vector graphics style)
  - [x] 5.4 Design and implement two distinct spacecraft shapes (visually different)
  - [x] 5.5 Implement spacecraft rotation rendering based on orientation - Fixed: Corrected 90-degree offset (0° now points right)
  - [x] 5.6 Implement flame animation rendering at rear of spacecraft when thrust is active - Fixed: Thrust flag persistence issue resolved
  - [x] 5.7 Implement projectile rendering as a dot
  - [x] 5.8 Implement explosion animation rendering (expanding circle or burst pattern)
  - [x] 5.9 Implement score display rendering (show "Player 1: X" and "Player 2: Y")
  - [x] 5.10 Implement connection status indicator rendering
  - [x] 5.11 Implement game over/win message rendering
  - [x] 5.12 Integrate Renderer into Game class main loop

- [x] 6.0 Input handling, collision detection, and game rules
  - [x] 6.1 Create `src/InputHandler.h` header file with InputHandler class declaration
  - [x] 6.2 Create `src/InputHandler.cpp` with InputHandler class implementation
  - [x] 6.3 Implement arrow key input detection (Left, Right, Up) - Fixed: Switched from polling to event-based input for SFML 3.0 compatibility
  - [x] 6.4 Implement spacebar input detection for firing - Fixed: Event-based input handling
  - [x] 6.5 Map input to local player's spacecraft controls
  - [x] 6.6 Implement projectile firing logic (create projectile at spacecraft front, traveling in direction of orientation)
  - [x] 6.7 Implement collision detection between projectiles and spacecraft (bounding box or geometric collision)
  - [x] 6.8 Implement hit detection logic (determine which player's projectile hit which spacecraft)
  - [x] 6.9 Implement scoring system (increment score when projectile hits opponent)
  - [x] 6.10 Implement explosion trigger when collision detected
  - [x] 6.11 Implement respawn logic (destroyed spacecraft reappears at random location with neutral orientation)
  - [x] 6.12 Implement win condition check (game ends when either player reaches score of 5)
  - [x] 6.13 Implement game state management (playing, paused on disconnect, game over)
  - [x] 6.14 Implement pause logic when network disconnection is detected
  - [x] 6.15 Implement resume logic when network reconnection is successful
  - [x] 6.16 Ensure only one projectile per player exists at a time (new projectile replaces old one if still active)
  - [x] 6.17 Test complete gameplay flow (connect, play, score, win condition)

## Implementation Notes

### Issues Fixed During Development

1. **SFML 3.0 Compatibility:**
   - Updated code to work with SFML 3.0 API changes (event handling, Text rendering, Angle types)
   - Fixed keyboard input by switching from polling (`isKeyPressed()`) to event-based input handling
   - Resolved "Detected a keyboard without any keys" warning on macOS

2. **Spacecraft Orientation:**
   - Fixed 90-degree offset issue where spacecraft visual direction didn't match thrust/fire direction
   - Updated spacecraft shapes so 0° points right (positive X direction) to match physics calculations

3. **Thrust Flame Animation:**
   - Fixed thrust flag being reset in `Spacecraft::update()`, preventing flame animation from displaying
   - Thrust flag now persists correctly through the frame for proper rendering

4. **Network Architecture:**
   - Implemented bidirectional PUSH/PULL ZeroMQ sockets for peer-to-peer communication
   - Each player binds locally and connects to peer for sending/receiving

### Known Limitations

- Configuration file must be manually created and edited by users (no in-game configuration UI)
- Font rendering requires font file loading (currently uses fallback when no font loaded)
- Single-player testing mode (no AI opponent)

