#include "Spacecraft.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//----------------------------------------------------------------------------------------
Spacecraft::Spacecraft() 
    : m_position(0.0f, 0.0f)
    , m_orientation(0.0f)
    , m_velocity(0.0f, 0.0f)
    , m_playerId(1)
    , m_isThrusting(false) 
{
}

//----------------------------------------------------------------------------------------
Spacecraft::Spacecraft(sf::Vector2f position, float orientation, int playerId)
    : m_position(position)
    , m_orientation(orientation)
    , m_velocity(0.0f, 0.0f)
    , m_playerId(playerId)
    , m_isThrusting(false) 
{
}

//----------------------------------------------------------------------------------------
void Spacecraft::update(float deltaTime)
{
    // Apply gravitational force toward center
    applyGravitationalForce(deltaTime);
    
    // Apply friction
    applyFriction(deltaTime);
    
    // Update position based on velocity
    m_position += m_velocity * deltaTime;
    
    // Wrap around screen edges
    wrapAroundScreen();
    
    // Note: m_isThrusting is managed by InputHandler, don't reset it here
}

//----------------------------------------------------------------------------------------
void Spacecraft::rotateLeft(float deltaTime)
{
    m_orientation -= Constants::SPACECRAFT_ROTATION_SPEED * deltaTime;
    // Normalize to 0-360 range
    while (m_orientation < 0.0f) {
        m_orientation += 360.0f;
    }
}

//----------------------------------------------------------------------------------------
void Spacecraft::rotateRight(float deltaTime) 
{
    m_orientation += Constants::SPACECRAFT_ROTATION_SPEED * deltaTime;
    // Normalize to 0-360 range
    while (m_orientation >= 360.0f) {
        m_orientation -= 360.0f;
    }
}

//----------------------------------------------------------------------------------------
void Spacecraft::applyThrust(float deltaTime) 
{
    m_isThrusting = true;
    
    // Convert orientation from degrees to radians
    float angleRad = m_orientation * M_PI / 180.0f;
    
    // Calculate thrust vector (forward direction)
    sf::Vector2f thrustVector(
        std::cos(angleRad) * Constants::SPACECRAFT_THRUST_ACCELERATION,
        std::sin(angleRad) * Constants::SPACECRAFT_THRUST_ACCELERATION
    );
    
    // Apply acceleration to velocity
    m_velocity += thrustVector * deltaTime;
    
    // Limit velocity to maximum
    float speed = std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);
    if (speed > Constants::SPACECRAFT_MAX_VELOCITY) {
        m_velocity = (m_velocity / speed) * Constants::SPACECRAFT_MAX_VELOCITY;
    }
}

//----------------------------------------------------------------------------------------
void Spacecraft::applyGravitationalForce(float deltaTime)
{
    // Calculate center of screen
    sf::Vector2f center(
        Constants::WINDOW_WIDTH / 2.0f,
        Constants::WINDOW_HEIGHT / 2.0f
    );
    
    // Calculate vector from spacecraft to center
    sf::Vector2f toCenter = center - m_position;
    
    // Calculate distance to center
    float distance = std::sqrt(toCenter.x * toCenter.x + toCenter.y * toCenter.y);
    
    // Avoid division by zero and apply minimum distance
    if (distance < Constants::MIN_GRAVITY_DISTANCE) {
        return;  // Too close to center, no gravitational effect
    }
    
    // Normalize direction vector
    sf::Vector2f direction = toCenter / distance;
    
    // Calculate gravitational acceleration (inverse square law: F = G / r^2)
    // The further from center, the weaker the pull
//    float acceleration = Constants::GRAVITATIONAL_STRENGTH / (distance * distance);
    float acceleration = Constants::GRAVITATIONAL_STRENGTH / (distance);
    
    // Apply gravitational acceleration to velocity
    m_velocity += direction * acceleration * deltaTime;
}

//----------------------------------------------------------------------------------------
void Spacecraft::applyFriction(float /*deltaTime*/) 
{
    // Apply friction to gradually slow down
    m_velocity *= Constants::SPACECRAFT_FRICTION;
    
    // Stop very small velocities to prevent jitter
    if (std::abs(m_velocity.x) < 0.1f) m_velocity.x = 0.0f;
    if (std::abs(m_velocity.y) < 0.1f) m_velocity.y = 0.0f;
}

//----------------------------------------------------------------------------------------
void Spacecraft::wrapAroundScreen() 
{
    // Wrap around horizontal edges
    if (m_position.x < 0.0f) {
        m_position.x = Constants::WINDOW_WIDTH;
    } else if (m_position.x > Constants::WINDOW_WIDTH) {
        m_position.x = 0.0f;
    }
    
    // Wrap around vertical edges
    if (m_position.y < 0.0f) {
        m_position.y = Constants::WINDOW_HEIGHT;
    } else if (m_position.y > Constants::WINDOW_HEIGHT) {
        m_position.y = 0.0f;
    }
}

//----------------------------------------------------------------------------------------
void Spacecraft::reset(sf::Vector2f position, float orientation) 
{
    m_position = position;
    m_orientation = orientation;
    m_velocity = sf::Vector2f(0.0f, 0.0f);
    m_isThrusting = false;
}

