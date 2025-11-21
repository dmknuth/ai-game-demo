cmake_minimum_required(VERSION 3.20)

project(SpaceWars VERSION 0.1.0 LANGUAGES CXX)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type to Release by default if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Spacecraft.cpp
    src/Projectile.cpp
    src/GameState.cpp
    src/NetworkManager.cpp
    src/Renderer.cpp
    src/InputHandler.cpp
)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Find SFML (try 3.0 first, then fall back to 2.5)
set(_sfml_components Graphics Window System)
find_package(SFML 3.0 COMPONENTS ${_sfml_components} QUIET)
if(SFML_FOUND AND SFML_VERSION_MAJOR VERSION_GREATER_EQUAL 3)
    set(SFML_3_FOUND TRUE)
else()
    find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)
    set(SFML_3_FOUND FALSE)
endif()

# Find ZeroMQ using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link SFML libraries (handle both 2.5 and 3.0)
set(_sfml_targets)
foreach(_component IN LISTS _sfml_components)
    string(TOLOWER "${_component}" _component_lower)
    set(_candidate_targets
        "SFML::${_component}"
        "sfml-${_component_lower}"
    )
    set(_component_target "")
    foreach(_candidate IN LISTS _candidate_targets)
        if(TARGET "${_candidate}" OR NOT "${_candidate}" MATCHES "^SFML::")
            set(_component_target "${_candidate}")
            break()
        endif()
    endforeach()
    if(_component_target MATCHES "^SFML::" AND NOT TARGET "${_component_target}")
        set(_component_target "")
    endif()
    if(_component_target STREQUAL "")
        message(FATAL_ERROR "SFML target for component ${_component} not found")
    endif()
    list(APPEND _sfml_targets "${_component_target}")
endforeach()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${_sfml_targets}
)

# Link ZeroMQ libraries
target_include_directories(${PROJECT_NAME} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${ZMQ_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${ZMQ_LIBRARIES})
target_compile_options(${PROJECT_NAME} PRIVATE ${ZMQ_CFLAGS_OTHER})

# Compiler-specific settings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Enable common warnings
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# Platform-specific settings
if(APPLE)
    # macOS specific settings
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
    
    # macOS compiler flags
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${PROJECT_NAME} PRIVATE
            -stdlib=libc++
        )
    endif()
    
    # Handle SFML framework paths on macOS
    if(EXISTS "/opt/homebrew/lib")
        # Homebrew on Apple Silicon
        set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};/opt/homebrew")
    elseif(EXISTS "/usr/local/lib")
        # Homebrew on Intel Mac
        set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};/usr/local")
    endif()
endif()

if(UNIX AND NOT APPLE)
    # Linux (Ubuntu) specific settings
    # Ensure we link against pthread for ZeroMQ
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
    
    # Linux-specific compiler flags
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        # GCC-specific flags
        target_compile_options(${PROJECT_NAME} PRIVATE
            -pthread
        )
    endif()
    
    # Common Linux library paths
    if(EXISTS "/usr/lib/x86_64-linux-gnu")
        # Ubuntu/Debian library path
        link_directories(/usr/lib/x86_64-linux-gnu)
    endif()
endif()

