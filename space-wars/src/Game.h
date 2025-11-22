#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <chrono>
#include "GameState.h"
#include "InputHandler.h"
#include "Renderer.h"
#include "NetworkManager.h"
#include "ConfigReader.h"

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
    void handleHit(int projectileOwnerId, int hitSpacecraftId);
    void respawnSpacecraft(int playerId);
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
    static constexpr float NETWORK_UPDATE_INTERVAL = 1.0f / 30.0f;  // 30 updates per second
    
    // Network reconnection
    NetworkConfig m_networkConfig;  // Store config for reconnection attempts
    float m_reconnectTimer;  // Timer for periodic reconnection attempts
    static constexpr float RECONNECT_INTERVAL = 2.0f;  // Try to reconnect every 2 seconds
    
    // Frame rate limiting
    static constexpr float TARGET_FPS = 60.0f;
    static constexpr float FRAME_TIME = 1.0f / TARGET_FPS;
    
    std::chrono::high_resolution_clock::time_point m_lastFrameTime;
};

#endif // GAME_H

