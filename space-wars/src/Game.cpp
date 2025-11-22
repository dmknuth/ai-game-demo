#include "Game.h"
#include "Constants.h"
#include <iostream>
#include <optional>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <fstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//----------------------------------------------------------------------------------------
Game::Game() 
    : m_isRunning(true)
    , m_isPaused(false)
    , m_localPlayerId(1)  // Will be set from config file
    , m_networkUpdateTimer(0.0f)
    , m_reconnectTimer(0.0f)
    , m_bothPlayersConnected(false)
{
    // Initialize SFML window (1024x768, windowed mode)
    m_window.create(sf::VideoMode(sf::Vector2u(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT)), "Space Wars");
    m_window.setFramerateLimit(static_cast<unsigned int>(TARGET_FPS));
    m_window.setKeyRepeatEnabled(true);  // Enable key repeat for continuous input
    
    m_lastFrameTime = std::chrono::high_resolution_clock::now();
    
    // Initialize random seed for respawn
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Initialize network connection
    initializeNetwork();
}

//----------------------------------------------------------------------------------------
Game::~Game() 
{
    m_networkManager.disconnect();
}

//----------------------------------------------------------------------------------------
void Game::run() 
{
    while (m_isRunning && m_window.isOpen()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration<float>(currentTime - m_lastFrameTime).count();
        m_lastFrameTime = currentTime;
        
        processInput();
        
        // Always call update - it handles network sync even when paused/waiting
        // and only updates game logic when both players are connected
        update(deltaTime);
        
        render();
    }
}

//----------------------------------------------------------------------------------------
void Game::processInput() 
{
    // Handle window events and pass keyboard events to InputHandler
    while (std::optional<sf::Event> event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            // Disconnect network before closing to prevent hanging
            m_networkManager.disconnect();
            m_window.close();
            m_isRunning = false;
            return;  // Exit immediately to avoid further processing
        } else {
            // Pass all events to InputHandler for keyboard event processing
            m_inputHandler.handleEvent(*event);
        }
    }
    
    // Process game input (only if not paused, both players connected, game not over, and window has focus)
    // Note: Input processing uses fixed timestep for consistency
    if (!m_isPaused && m_bothPlayersConnected && !m_gameState.isGameOver() && m_window.hasFocus()) {
        float fixedDeltaTime = 1.0f / TARGET_FPS;
        m_inputHandler.processInput(m_gameState, m_localPlayerId, fixedDeltaTime);
    }
}

//----------------------------------------------------------------------------------------
void Game::update(float deltaTime) 
{
    // Network synchronization - always send/receive when connected, even when paused
    // This allows both players to detect each other and start the game
    if (m_networkManager.isConnected()) {
        m_networkUpdateTimer += deltaTime;
        if (m_networkUpdateTimer >= NETWORK_UPDATE_INTERVAL) {
            syncNetworkState();
            m_networkUpdateTimer = 0.0f;
        }
    }
    
    // Only update game logic if not paused and both players are connected
    if (m_isPaused || !m_bothPlayersConnected) {
        return;
    }
    
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
    
    // Check network connection and handle reconnection
    if (m_networkManager.isConnected()) {
        if (m_networkManager.isConnectionLost()) {
            // Connection was lost - pause the game
            if (!m_isPaused) {
                m_isPaused = true;
                m_bothPlayersConnected = false;  // Reset - need to receive message again
                std::cout << "Connection lost - Game paused. Attempting to reconnect..." << std::endl;
            }
            // Disconnect to allow reconnection attempt
            m_networkManager.disconnect();
        }
    } else {
        // Not connected - try to reconnect periodically
        if (m_networkManager.isConnectionLost()) {
            m_reconnectTimer += deltaTime;
            
            // Attempt reconnection every RECONNECT_INTERVAL seconds
            if (m_reconnectTimer >= RECONNECT_INTERVAL) {
                m_reconnectTimer = 0.0f;
                
                std::cout << "Attempting to reconnect..." << std::endl;
                if (m_networkManager.connect(m_networkConfig.clientIp, 
                                             m_networkConfig.clientPort, 
                                             m_networkConfig.hostPort)) {
                    // Reconnection successful!
                    m_isPaused = false;
                    m_networkManager.resetConnectionStatus();
                    m_bothPlayersConnected = false;  // Reset - need to receive message again to confirm both players
                    std::cout << "Reconnected! Waiting for other player..." << std::endl;
                } else {
                    // Reconnection failed - will try again in RECONNECT_INTERVAL seconds
                    std::cout << "Reconnection failed. Will retry in " << RECONNECT_INTERVAL << " seconds..." << std::endl;
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------
void Game::render() 
{
    m_window.clear(sf::Color::Black);
    
    // Render game state
    bool connectionLost = m_networkManager.isConnectionLost();
    bool connected = m_networkManager.isConnected();
    m_renderer.render(m_window, m_gameState, connectionLost, m_localPlayerId, connected, m_bothPlayersConnected);
    
    m_window.display();
}

//----------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------
void Game::checkWinCondition() {
    if (m_gameState.hasWinner()) {
        int winner = m_gameState.getWinner();
        std::cout << "Player " << winner << " wins!" << std::endl;
        // Game over state is already set in GameState
    }
}

//----------------------------------------------------------------------------------------
void Game::syncNetworkState() {
    if (!m_networkManager.isConnected()) {
        return;
    }
    
    // Always send local game state first (this breaks the deadlock)
    // Both players send continuously, so they will eventually receive each other's messages
    // Even if send fails initially (peer not ready), we keep trying - ZeroMQ will queue messages
    // once the peer's PULL socket is bound and ready
    m_networkManager.sendGameState(m_gameState);
    
    // Receive remote game state (non-blocking)
    GameState remoteState;
    if (m_networkManager.receiveGameState(remoteState)) {
        // Mark that both players are now connected (we've received a message from the other player)
        if (!m_bothPlayersConnected) {
            m_bothPlayersConnected = true;
            std::cout << "Player " << ((m_localPlayerId == 1) ? 2 : 1) << " has joined! Game starting..." << std::endl;
        }
        
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

//----------------------------------------------------------------------------------------
std::string Game::findConfigFile() {
    // Configuration file location priority:
    // 1. Current working directory (where the user runs the command)
    // 2. Same directory as the executable (if we can determine it)
    
    std::string configFile = "config.txt";
    
    // Check current working directory first
    if (std::ifstream(configFile).good()) {
        return configFile;
    }
    
    // Try executable directory (for when running from different directory)
    // On Unix-like systems, we can try to get the executable path
    // For now, we'll rely on current working directory
    // Users should either:
    // - Place config.txt in the same directory as the executable and run from there
    // - Or place config.txt in the current working directory
    
    return configFile;  // Return the filename to try
}

//----------------------------------------------------------------------------------------
void Game::initializeNetwork() {
    ConfigReader configReader;
    NetworkConfig config;
    
    // Find configuration file (tries multiple locations)
    std::string configFile = findConfigFile();
    
    if (!configReader.readConfig(configFile, config)) {
        std::cerr << "\n=== ERROR: Failed to read configuration file ===" << std::endl;
        std::cerr << "Configuration file '" << configFile << "' is missing or invalid." << std::endl;
        std::cerr << "\nLocation: The config.txt file should be in:" << std::endl;
        std::cerr << "  - The current working directory (where you run the command)" << std::endl;
        std::cerr << "  - Or the same directory as the SpaceWars executable" << std::endl;
        std::cerr << "\nPlease create a configuration file with the following format:" << std::endl;
        std::cerr << "  host_ip=127.0.0.1" << std::endl;
        std::cerr << "  host_port=5555" << std::endl;
        std::cerr << "  client_ip=127.0.0.1" << std::endl;
        std::cerr << "  client_port=5556" << std::endl;
        std::cerr << "  host=1" << std::endl;
        std::cerr << "  client=2" << std::endl;
        std::cerr << "\nSee config.example.txt for an example configuration file." << std::endl;
        std::cerr << "Exiting..." << std::endl;
        m_isRunning = false;
        return;
    }
    
    std::cout << "\n=== Space Wars Network Configuration ===" << std::endl;
    std::cout << "Configuration loaded from: " << configFile << std::endl;
    std::cout << "Host IP: " << config.hostIp << std::endl;
    std::cout << "Host Port: " << config.hostPort << std::endl;
    std::cout << "Client IP: " << config.clientIp << std::endl;
    std::cout << "Client Port: " << config.clientPort << std::endl;
    std::cout << "You are Player: " << config.hostPlayerId << std::endl;
    std::cout << "Connecting to Player: " << config.clientPlayerId << std::endl;
    std::cout << "Connecting..." << std::endl;
    
    // Store configuration for reconnection attempts
    m_networkConfig = config;
    
    // Set local player ID from configuration
    m_localPlayerId = config.hostPlayerId;
    
    // Connect using configuration
    // host_ip/host_port: where this player binds (receives)
    // client_ip/client_port: where this player connects (sends)
    if (m_networkManager.connect(config.clientIp, config.clientPort, config.hostPort)) {
        std::cout << "Connected! Waiting for other player..." << std::endl;
        // Reset network timer to send first message immediately on next update
        m_networkUpdateTimer = NETWORK_UPDATE_INTERVAL;
        // Note: We don't send immediately here because the peer's PULL socket might not be bound yet
        // The first send will happen in the next update() call, by which time both players should be ready
    } else {
        std::cerr << "Failed to connect. Please check:" << std::endl;
        std::cerr << "  1. Network configuration in " << configFile << std::endl;
        std::cerr << "  2. Firewall settings" << std::endl;
        std::cerr << "  3. Other player is running and ready to connect" << std::endl;
        std::cerr << "Continuing in single-player mode. Will attempt to reconnect automatically." << std::endl;
        // Mark as connection lost so reconnection attempts will begin
        m_networkManager.resetConnectionStatus();  // This will be set to lost on first send attempt
    }
    
    std::cout << std::endl;
}
