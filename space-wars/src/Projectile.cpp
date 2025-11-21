#include "Projectile.h"
#include <cmath>

Projectile::Projectile()
    : m_position(0.0f, 0.0f)
    , m_velocity(0.0f, 0.0f)
    , m_ownerPlayerId(1)
    , m_active(false) {
}

Projectile::Projectile(sf::Vector2f position, sf::Vector2f direction, int ownerPlayerId)
    : m_position(position)
    , m_ownerPlayerId(ownerPlayerId)
    , m_active(true) {
    // Normalize direction and multiply by projectile speed
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0.0f) {
        m_velocity = (direction / length) * Constants::PROJECTILE_SPEED;
    } else {
        m_velocity = sf::Vector2f(0.0f, 0.0f);
        m_active = false;
    }
}

void Projectile::update(float deltaTime) {
    if (!m_active) return;
    
    // Update position based on velocity
    m_position += m_velocity * deltaTime;
    
    // Check if projectile is off screen
    if (isOffScreen()) {
        m_active = false;
    }
}

bool Projectile::isOffScreen() const {
    // Check if projectile has left the screen boundaries
    return (m_position.x < 0.0f || 
            m_position.x > Constants::WINDOW_WIDTH ||
            m_position.y < 0.0f || 
            m_position.y > Constants::WINDOW_HEIGHT);
}

