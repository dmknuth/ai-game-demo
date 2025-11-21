#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <SFML/Graphics.hpp>
#include "Constants.h"

class Projectile {
public:
    Projectile();
    Projectile(sf::Vector2f position, sf::Vector2f direction, int ownerPlayerId);
    
    void update(float deltaTime);
    bool isOffScreen() const;
    
    // Getters
    sf::Vector2f getPosition() const { return m_position; }
    sf::Vector2f getVelocity() const { return m_velocity; }
    int getOwnerPlayerId() const { return m_ownerPlayerId; }
    bool isActive() const { return m_active; }
    
    // Setters
    void setPosition(sf::Vector2f position) { m_position = position; }
    void setActive(bool active) { m_active = active; }
    
private:
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    int m_ownerPlayerId;  // Which player fired this projectile (1 or 2)
    bool m_active;
};

#endif // PROJECTILE_H

