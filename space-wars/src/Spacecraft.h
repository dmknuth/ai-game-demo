#ifndef SPACECRAFT_H
#define SPACECRAFT_H

#include <SFML/Graphics.hpp>
#include "Constants.h"

class Spacecraft {
public:
    Spacecraft();
    Spacecraft(sf::Vector2f position, float orientation, int playerId);
    
    void update(float deltaTime);
    void rotateLeft(float deltaTime);
    void rotateRight(float deltaTime);
    void applyThrust(float deltaTime);
    void wrapAroundScreen();
    
    // Getters
    sf::Vector2f getPosition() const { return m_position; }
    float getOrientation() const { return m_orientation; }
    sf::Vector2f getVelocity() const { return m_velocity; }
    int getPlayerId() const { return m_playerId; }
    bool isThrusting() const { return m_isThrusting; }
    
    // Setters
    void setPosition(sf::Vector2f position) { m_position = position; }
    void setOrientation(float orientation) { m_orientation = orientation; }
    void setVelocity(sf::Vector2f velocity) { m_velocity = velocity; }
    void setThrusting(bool thrusting) { m_isThrusting = thrusting; }
    
    // Reset spacecraft to initial state
    void reset(sf::Vector2f position, float orientation);
    
private:
    sf::Vector2f m_position;
    float m_orientation;  // in degrees, 0 = pointing right, 90 = pointing down
    sf::Vector2f m_velocity;
    int m_playerId;  // 1 or 2, for visual distinction
    bool m_isThrusting;
    
    void applyFriction(float deltaTime);
};

#endif // SPACECRAFT_H

