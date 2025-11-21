#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include "GameState.h"

class InputHandler {
public:
    InputHandler();
    
    // Handle SFML events
    void handleEvent(const sf::Event& event);
    
    // Process input and update game state (called every frame)
    void processInput(GameState& gameState, int localPlayerId, float deltaTime);
    
    // Reset input state (for preventing repeated firing)
    void reset();
    
private:
    // Track key states (updated via events)
    bool m_leftPressed;
    bool m_rightPressed;
    bool m_upPressed;
    bool m_spacePressed;
    bool m_spaceWasPressed;  // To detect spacebar press (not hold)
    
    // Handle spacecraft controls
    void handleSpacecraftInput(Spacecraft& spacecraft, float deltaTime);
    
    // Handle projectile firing
    void handleFiring(GameState& gameState, int localPlayerId);
};

#endif // INPUTHANDLER_H

