#include "Game.h"
#include "Constants.h"
#include <iostream>
#include <optional>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Game::Game() 
    : m_isRunning(true)
    , m_isPaused(false)
    , m_localPlayerId(1)  // Default to player 1, could be made configurable
    , m_networkUpdateTimer(0.0f) {
    // Initialize SFML window (1024x768, windowed mode)
    m_window.create(sf::VideoMode(sf::Vector2u(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT)), "Space Wars");
    m_window.setFramerateLimit(static_cast<unsigned int>(TARGET_FPS));
    m_window.setKeyRepeatEnabled(true);  // Enable key repeat for continuous input
    
    m_lastFrameTime = std::chrono::high_resolution_clock::now();
    
    // Initialize random seed for respawn
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Initialize network (for now, network setup will be manual)
    // TODO: Add UI for network connection setup
    std::cout << "Space Wars Game Started" << std::endl;
    std::cout << "Note: Network connection setup not yet implemented in UI" << std::endl;
}

Game::~Game() {
    m_networkManager.disconnect();
}

void Game::run() {
    while (m_isRunning && m_window.isOpen()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration<float>(currentTime - m_lastFrameTime).count();
        m_lastFrameTime = currentTime;
        
        processInput();
        
        if (!m_isPaused && !m_gameState.isGameOver()) {
            update(deltaTime);
        }
        
        render();
    }
}

void Game::processInput() {
    // Handle window events and pass keyboard events to InputHandler
    while (std::optional<sf::Event> event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            m_isRunning = false;
        } else {
            // Pass all events to InputHandler for keyboard event processing
            m_inputHandler.handleEvent(*event);
        }
    }
    
    // Process game input (only if not paused and game not over and window has focus)
    // Note: Input processing uses fixed timestep for consistency
    if (!m_isPaused && !m_gameState.isGameOver() && m_window.hasFocus()) {
        float fixedDeltaTime = 1.0f / TARGET_FPS;
        m_inputHandler.processInput(m_gameState, m_localPlayerId, fixedDeltaTime);
    }
}

void Game::update(float deltaTime) {
    // Update game state
    m_gameState.updateProjectiles(deltaTime);
    m_gameState.removeInactiveProjectiles();
    
    // Update remote player's spacecraft (local player's is updated in InputHandler)
    int remotePlayerId = (m_localPlayerId == 1) ? 2 : 1;
    m_gameState.getSpacecraft(remotePlayerId).update(deltaTime);
    
    // Check collisions
    checkCollisions();
    
    // Check win condition
    checkWinCondition();
    
    // Update explosion animation
    m_renderer.updateExplosion(deltaTime);
    
    // Network synchronization
    m_networkUpdateTimer += deltaTime;
    if (m_networkUpdateTimer >= NETWORK_UPDATE_INTERVAL) {
        syncNetworkState();
        m_networkUpdateTimer = 0.0f;
    }
    
    // Check network connection
    if (m_networkManager.isConnected()) {
        if (m_networkManager.isConnectionLost()) {
            m_isPaused = true;
            std::cout << "Connection lost - Game paused" << std::endl;
        }
    } else {
        // Try to reconnect if connection was lost
        if (m_networkManager.isConnectionLost()) {
            // Reconnection logic would go here
            // For now, just reset the connection status
            m_networkManager.resetConnectionStatus();
        }
    }
}

void Game::render() {
    m_window.clear(sf::Color::Black);
    
    // Render game state
    bool connectionLost = m_networkManager.isConnectionLost();
    m_renderer.render(m_window, m_gameState, connectionLost, m_localPlayerId);
    
    m_window.display();
}

void Game::checkCollisions() {
    const auto& projectiles = m_gameState.getProjectiles();
    
    for (const auto& projectile : projectiles) {
        if (!projectile.isActive()) continue;
        
        sf::Vector2f projPos = projectile.getPosition();
        int projOwnerId = projectile.getOwnerPlayerId();
        
        // Check collision with spacecraft 1
        if (projOwnerId != 1) {
            Spacecraft& sc1 = m_gameState.getSpacecraft(1);
            sf::Vector2f sc1Pos = sc1.getPosition();
            float distance = std::sqrt(
                (projPos.x - sc1Pos.x) * (projPos.x - sc1Pos.x) +
                (projPos.y - sc1Pos.y) * (projPos.y - sc1Pos.y)
            );
            
            if (distance < Constants::SPACECRAFT_SIZE + Constants::PROJECTILE_SIZE) {
                handleHit(projOwnerId, 1);
                return;  // Only one hit per frame
            }
        }
        
        // Check collision with spacecraft 2
        if (projOwnerId != 2) {
            Spacecraft& sc2 = m_gameState.getSpacecraft(2);
            sf::Vector2f sc2Pos = sc2.getPosition();
            float distance = std::sqrt(
                (projPos.x - sc2Pos.x) * (projPos.x - sc2Pos.x) +
                (projPos.y - sc2Pos.y) * (projPos.y - sc2Pos.y)
            );
            
            if (distance < Constants::SPACECRAFT_SIZE + Constants::PROJECTILE_SIZE) {
                handleHit(projOwnerId, 2);
                return;  // Only one hit per frame
            }
        }
    }
}

void Game::handleHit(int projectileOwnerId, int hitSpacecraftId) {
    // Remove the projectile
    auto& projectiles = m_gameState.getProjectiles();
    projectiles.erase(
        std::remove_if(
            projectiles.begin(),
            projectiles.end(),
            [projectileOwnerId](const Projectile& p) {
                return p.getOwnerPlayerId() == projectileOwnerId && p.isActive();
            }
        ),
        projectiles.end()
    );
    
    // Increment score for the player who fired
    m_gameState.incrementScore(projectileOwnerId);
    
    // Trigger explosion at hit location
    Spacecraft& hitSpacecraft = m_gameState.getSpacecraft(hitSpacecraftId);
    m_renderer.triggerExplosion(hitSpacecraft.getPosition());
    
    // Respawn the hit spacecraft
    respawnSpacecraft(hitSpacecraftId);
    
    std::cout << "Player " << projectileOwnerId << " hit Player " << hitSpacecraftId 
              << "! Score: " << m_gameState.getScore(projectileOwnerId) << std::endl;
}

void Game::respawnSpacecraft(int playerId) {
    Spacecraft& spacecraft = m_gameState.getSpacecraft(playerId);
    
    // Generate random position (avoid center where both start)
    float x = static_cast<float>(std::rand() % Constants::WINDOW_WIDTH);
    float y = static_cast<float>(std::rand() % Constants::WINDOW_HEIGHT);
    
    // Ensure not too close to center
    sf::Vector2f center(Constants::WINDOW_WIDTH / 2.0f, Constants::WINDOW_HEIGHT / 2.0f);
    sf::Vector2f pos(x, y);
    float distToCenter = std::sqrt(
        (pos.x - center.x) * (pos.x - center.x) +
        (pos.y - center.y) * (pos.y - center.y)
    );
    
    // If too close to center, move to edge
    if (distToCenter < 100.0f) {
        if (std::rand() % 2 == 0) {
            x = (std::rand() % 2 == 0) ? 50.0f : Constants::WINDOW_WIDTH - 50.0f;
        } else {
            y = (std::rand() % 2 == 0) ? 50.0f : Constants::WINDOW_HEIGHT - 50.0f;
        }
    }
    
    // Reset spacecraft at new position with neutral orientation
    float orientation = static_cast<float>(std::rand() % 360);
    spacecraft.reset(sf::Vector2f(x, y), orientation);
}

void Game::checkWinCondition() {
    if (m_gameState.hasWinner()) {
        int winner = m_gameState.getWinner();
        std::cout << "Player " << winner << " wins!" << std::endl;
        // Game over state is already set in GameState
    }
}

void Game::syncNetworkState() {
    if (!m_networkManager.isConnected()) {
        return;
    }
    
    // Send local game state
    m_networkManager.sendGameState(m_gameState);
    
    // Receive remote game state (non-blocking)
    GameState remoteState;
    if (m_networkManager.receiveGameState(remoteState)) {
        // Merge remote state (for now, just sync the other player's spacecraft)
        // In a full implementation, you'd merge more carefully
        
        // Sync other player's spacecraft
        int otherPlayerId = (m_localPlayerId == 1) ? 2 : 1;
        Spacecraft& otherSc = m_gameState.getSpacecraft(otherPlayerId);
        const Spacecraft& remoteOtherSc = remoteState.getSpacecraft(otherPlayerId);
        
        // Update other player's spacecraft from remote state
        otherSc.setPosition(remoteOtherSc.getPosition());
        otherSc.setOrientation(remoteOtherSc.getOrientation());
        otherSc.setVelocity(remoteOtherSc.getVelocity());
        otherSc.setThrusting(remoteOtherSc.isThrusting());
        
        // Sync projectiles (clear and add remote projectiles)
        m_gameState.getProjectiles().clear();
        for (const auto& proj : remoteState.getProjectiles()) {
            if (proj.isActive()) {
                m_gameState.addProjectile(proj);
            }
        }
        
        // Sync scores
        m_gameState.setScore(1, remoteState.getScore(1));
        m_gameState.setScore(2, remoteState.getScore(2));
    }
}

void Game::initializeNetwork() {
    // Network initialization would be done here
    // For now, this is a placeholder
    // In a full implementation, you'd prompt for IP/port or have a menu
}
