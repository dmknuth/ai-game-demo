# Testing on Ubuntu Linux - Step by Step Guide

This guide will walk you through testing the Space Wars build on an Ubuntu system.

## Prerequisites

- Access to an Ubuntu Linux system (20.04 or later recommended)
- Internet connection for installing dependencies
- Terminal/command line access

## Step 1: Transfer the Project to Ubuntu

You have a few options:

### Option A: Using Git (Recommended)
If the project is in a git repository:
```bash
# On Ubuntu, clone or pull the repository
git clone <repository-url>
# or if already cloned:
git pull
```

### Option B: Using SCP/SFTP
From your macOS system, copy the `space-wars` directory:
```bash
# From macOS terminal
scp -r "space-wars" user@ubuntu-host:/path/to/destination/
```

### Option C: Using a USB drive or cloud storage
Copy the entire `space-wars` directory to your Ubuntu system.

## Step 2: Install Dependencies

Open a terminal on your Ubuntu system and run:

```bash
# Update package list
sudo apt update

# Install build tools and CMake
sudo apt install -y build-essential cmake pkg-config

# Install SFML
sudo apt install -y libsfml-dev

# Install ZeroMQ
sudo apt install -y libzmq3-dev

# Verify installations
cmake --version
g++ --version
pkg-config --modversion libzmq
pkg-config --modversion sfml-system 2>/dev/null || pkg-config --modversion sfml-all 2>/dev/null || echo "SFML version check"
```

## Step 3: Navigate to Project Directory

```bash
cd /path/to/space-wars
# Replace /path/to/space-wars with the actual path where you placed the project
```

## Step 4: Run the Automated Test Script

If you transferred the test script:

```bash
# Make sure the script is executable
chmod +x test-build-ubuntu.sh

# Run the test
./test-build-ubuntu.sh
```

The script will:
- Check for all required tools
- Verify dependencies are installed
- Clean any previous builds
- Configure with CMake
- Build the project
- Verify the executable was created

## Step 5: Manual Testing (Alternative)

If you prefer to test manually or the script doesn't work:

```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Check if executable exists
ls -la bin/SpaceWars

# If successful, you should see the executable
```

## Step 6: Verify the Build

After a successful build, verify:

```bash
# Check executable exists
file build/bin/SpaceWars

# Check it's a Linux executable
ldd build/bin/SpaceWars  # Should show linked libraries

# Try running it (it will just print a message and exit since it's a minimal test)
./build/bin/SpaceWars
```

Expected output:
```
Space Wars Game - Build Test
```

## Troubleshooting

### Issue: "cmake: command not found"
**Solution:**
```bash
sudo apt install cmake
```

### Issue: "g++: command not found"
**Solution:**
```bash
sudo apt install build-essential
```

### Issue: "SFML not found"
**Solution:**
```bash
sudo apt install libsfml-dev
# Verify installation
pkg-config --modversion sfml-system
```

### Issue: "ZeroMQ not found"
**Solution:**
```bash
sudo apt install libzmq3-dev
# Verify installation
pkg-config --modversion libzmq
```

### Issue: "C++23 not supported"
**Solution:**
- Check GCC version: `g++ --version`
- GCC 13+ is recommended for full C++23 support
- If using older GCC, you may need to update:
  ```bash
  sudo add-apt-repository ppa:ubuntu-toolchain-r/test
  sudo apt update
  sudo apt install gcc-13 g++-13
  # Use specific version: cmake -DCMAKE_CXX_COMPILER=g++-13 ..
  ```

### Issue: Build fails with linker errors
**Solution:**
- Ensure all dependencies are installed
- Try cleaning and rebuilding:
  ```bash
  rm -rf build
  mkdir build && cd build
  cmake ..
  cmake --build .
  ```

## Success Criteria

The test is successful if:
1. ✅ CMake configuration completes without errors
2. ✅ Build completes without errors
3. ✅ Executable `build/bin/SpaceWars` is created
4. ✅ Executable runs and prints "Space Wars Game - Build Test"

## Next Steps

Once the build test passes on Ubuntu, you can:
- Mark task 1.8 as complete
- Proceed with task 2.0 (Core game engine and window management)
- Continue development on either macOS or Ubuntu

## Reporting Results

After testing, note:
- Ubuntu version: `lsb_release -a`
- GCC version: `g++ --version`
- SFML version: `pkg-config --modversion sfml-system` (or similar)
- ZeroMQ version: `pkg-config --modversion libzmq`
- Build result: Success or any errors encountered

