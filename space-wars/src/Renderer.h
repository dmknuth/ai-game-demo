#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include "Spacecraft.h"
#include "Projectile.h"
#include "GameState.h"
#include <vector>

class Renderer {
public:
    Renderer();
    
    // Main rendering function
    void render(sf::RenderWindow& window, const GameState& gameState, 
                bool connectionLost, int localPlayerId, 
                bool connected, bool bothPlayersConnected);
    
    // Explosion management
    void triggerExplosion(sf::Vector2f position);
    void updateExplosion(float deltaTime);
    
private:
    // Spacecraft rendering
    void drawSpacecraft(sf::RenderWindow& window, const Spacecraft& spacecraft);
    void drawSpacecraftShape1(sf::RenderWindow& window, sf::Vector2f position, float orientation);
    void drawSpacecraftShape2(sf::RenderWindow& window, sf::Vector2f position, float orientation);
    void drawThrustFlame(sf::RenderWindow& window, const Spacecraft& spacecraft);
    
    // Projectile rendering
    void drawProjectile(sf::RenderWindow& window, const Projectile& projectile);
    
    // Explosion rendering
    void drawExplosion(sf::RenderWindow& window, sf::Vector2f position, float radius);
    
    // UI rendering
    void drawScore(sf::RenderWindow& window, int score1, int score2);
    void drawConnectionStatus(sf::RenderWindow& window, bool connected, bool connectionLost, 
                               bool bothPlayersConnected, int localPlayerId);
    void drawGameOver(sf::RenderWindow& window, int winner);
    
    // Helper functions
    sf::Vector2f rotatePoint(sf::Vector2f point, sf::Vector2f center, float angleDegrees);
    void drawLine(sf::RenderWindow& window, sf::Vector2f p1, sf::Vector2f p2, sf::Color color = sf::Color::White);
    
    // Font for text rendering
    sf::Font m_font;
    bool m_fontLoaded;
    
    // Explosion animation state (could be expanded for multiple explosions)
    float m_explosionRadius;
    float m_explosionTime;
    bool m_explosionActive;
    sf::Vector2f m_explosionPosition;
};

#endif // RENDERER_H

