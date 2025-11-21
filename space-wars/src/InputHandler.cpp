#include "InputHandler.h"
#include "Constants.h"
#include "Projectile.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

InputHandler::InputHandler()
    : m_leftPressed(false)
    , m_rightPressed(false)
    , m_upPressed(false)
    , m_spacePressed(false)
    , m_spaceWasPressed(false) {
}

void InputHandler::handleEvent(const sf::Event& event) {
    // Handle keyboard events - SFML 3.0 uses variant-based events
    if (auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Left) {
            m_leftPressed = true;
        } else if (keyPressed->code == sf::Keyboard::Key::Right) {
            m_rightPressed = true;
        } else if (keyPressed->code == sf::Keyboard::Key::Up) {
            m_upPressed = true;
        } else if (keyPressed->code == sf::Keyboard::Key::Space) {
            if (!m_spaceWasPressed) {
                m_spacePressed = true;
            }
        }
    } else if (auto* keyReleased = event.getIf<sf::Event::KeyReleased>()) {
        if (keyReleased->code == sf::Keyboard::Key::Left) {
            m_leftPressed = false;
        } else if (keyReleased->code == sf::Keyboard::Key::Right) {
            m_rightPressed = false;
        } else if (keyReleased->code == sf::Keyboard::Key::Up) {
            m_upPressed = false;
        } else if (keyReleased->code == sf::Keyboard::Key::Space) {
            m_spacePressed = false;
            m_spaceWasPressed = false;
        }
    }
}

void InputHandler::processInput(GameState& gameState, int localPlayerId, float deltaTime) {
    // Get local player's spacecraft
    Spacecraft& spacecraft = gameState.getSpacecraft(localPlayerId);
    
    // Handle spacecraft movement
    handleSpacecraftInput(spacecraft, deltaTime);
    
    // Handle firing (on spacebar press, not hold)
    if (m_spacePressed && !m_spaceWasPressed) {
        handleFiring(gameState, localPlayerId);
        m_spaceWasPressed = true;
    }
}

void InputHandler::handleSpacecraftInput(Spacecraft& spacecraft, float deltaTime) {
    // Rotate left
    if (m_leftPressed) {
        spacecraft.rotateLeft(deltaTime);
    }
    
    // Rotate right
    if (m_rightPressed) {
        spacecraft.rotateRight(deltaTime);
    }
    
    // Apply thrust
    if (m_upPressed) {
        spacecraft.applyThrust(deltaTime);
        spacecraft.setThrusting(true);
    } else {
        spacecraft.setThrusting(false);
    }
    
    // Update spacecraft
    spacecraft.update(deltaTime);
}

void InputHandler::handleFiring(GameState& gameState, int localPlayerId) {
    const Spacecraft& spacecraft = gameState.getSpacecraft(localPlayerId);
    
    // Remove existing projectile for this player (only one projectile per player)
    auto& projectiles = gameState.getProjectiles();
    projectiles.erase(
        std::remove_if(
            projectiles.begin(),
            projectiles.end(),
            [localPlayerId](const Projectile& p) {
                return p.getOwnerPlayerId() == localPlayerId;
            }
        ),
        projectiles.end()
    );
    
    // Calculate projectile direction from spacecraft orientation
    float angleRad = spacecraft.getOrientation() * M_PI / 180.0f;
    sf::Vector2f direction(
        std::cos(angleRad),
        std::sin(angleRad)
    );
    
    // Calculate projectile starting position (front of spacecraft)
    float spacecraftSize = 15.0f;
    sf::Vector2f startPosition = spacecraft.getPosition() + direction * spacecraftSize;
    
    // Create and add projectile
    Projectile projectile(startPosition, direction, localPlayerId);
    gameState.addProjectile(projectile);
}

void InputHandler::reset() {
    m_leftPressed = false;
    m_rightPressed = false;
    m_upPressed = false;
    m_spacePressed = false;
    m_spaceWasPressed = false;
}

