#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {
    // Window dimensions
    constexpr unsigned int WINDOW_WIDTH = 1024;
    constexpr unsigned int WINDOW_HEIGHT = 768;
    
    // Spacecraft physics
    constexpr float SPACECRAFT_ROTATION_SPEED = 180.0f;  // degrees per second
    constexpr float SPACECRAFT_THRUST_ACCELERATION = 200.0f;  // pixels per second squared
    constexpr float SPACECRAFT_MAX_VELOCITY = 300.0f;  // pixels per second
    constexpr float SPACECRAFT_FRICTION = 0.98f;  // velocity multiplier per frame (simulates friction)
    
    // Gravitational force
    constexpr float GRAVITATIONAL_STRENGTH = 50000.0f;  // gravitational constant (pixels^3 per second^2)
    constexpr float MIN_GRAVITY_DISTANCE = 10.0f;  // minimum distance to prevent division by zero
    
    // Projectile physics
    constexpr float PROJECTILE_SPEED = 400.0f;  // pixels per second
    
    // Game rules
    constexpr int WIN_SCORE = 5;  // First player to reach this score wins
    
    // Spacecraft dimensions (for collision detection)
    constexpr float SPACECRAFT_SIZE = 20.0f;  // approximate radius in pixels
    
    // Projectile dimensions
    constexpr float PROJECTILE_SIZE = 2.0f;  // radius in pixels
}

#endif // CONSTANTS_H

