#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <chrono>
#include "GameState.h"
#include "InputHandler.h"
#include "Renderer.h"
#include "NetworkManager.h"
#include "ConfigReader.h"

class Projectile;  // Forward declaration

class Game {
public:
    Game();
    ~Game();
    
    void run();
    
private:
    void processInput();
    void update(float deltaTime);
    void render();
    
    // Game logic
    void checkCollisions();
    void handleHit(const Projectile& projectile, int hitSpacecraftId);
    void respawnSpacecraft(int playerId, sf::Vector2f avoidPosition);
    void checkWinCondition();
    
    // Network
    void initializeNetwork();
    void syncNetworkState();
    std::string findConfigFile();  // Helper to locate config.txt
    
    // Game components
    sf::RenderWindow m_window;
    GameState m_gameState;
    InputHandler m_inputHandler;
    Renderer m_renderer;
    NetworkManager m_networkManager;
    
    // Game state
    bool m_isRunning;
    bool m_isPaused;
    int m_localPlayerId;  // 1 or 2
    float m_networkUpdateTimer;
    static constexpr float NETWORK_UPDATE_INTERVAL = 1.0f / 60.0f;  // 60 updates per second (matches frame rate for lower latency)
    
    // Network reconnection
    NetworkConfig m_networkConfig;  // Store config for reconnection attempts
    float m_reconnectTimer;  // Timer for periodic reconnection attempts
    static constexpr float RECONNECT_INTERVAL = 2.0f;  // Try to reconnect every 2 seconds
    
    // Player connection state
    bool m_bothPlayersConnected;  // True when we've received at least one message from the other player
    
    // Respawn system
    float m_respawnTimer1;  // Timer for player 1 respawn
    float m_respawnTimer2;  // Timer for player 2 respawn
    sf::Vector2f m_pendingRespawnPos1;  // Position to respawn player 1
    sf::Vector2f m_pendingRespawnPos2;  // Position to respawn player 2
    static constexpr float RESPAWN_DELAY = 1.5f;  // Delay before respawning in seconds
    
    // Frame rate limiting
    static constexpr float TARGET_FPS = 60.0f;
    static constexpr float FRAME_TIME = 1.0f / TARGET_FPS;
    
    std::chrono::high_resolution_clock::time_point m_lastFrameTime;
};

#endif // GAME_H

