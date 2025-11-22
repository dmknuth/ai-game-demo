#include "Renderer.h"
#include "Constants.h"
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//----------------------------------------------------------------------------------------
Renderer::Renderer()
    : m_fontLoaded(false)
    , m_explosionRadius(0.0f)
    , m_explosionTime(0.0f)
    , m_explosionActive(false) 
{
    // Try to load a default font (SFML 3.0 may have built-in font support)
    // For now, we'll use SFML's default rendering which should work
    m_fontLoaded = false;  // We'll use SFML's default text rendering

    // Font loading
    for (const auto& path : {
        #ifdef SFML_SYSTEM_WINDOWS
        // Windows-specific code
        #elif defined(SFML_SYSTEM_LINUX)
        "/snap/gnome-42-2204/202/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"
        #elif defined(SFML_SYSTEM_MACOS)
        "/System/Library/Fonts/Monaco.ttf"
        #elif defined(SFML_SYSTEM_IOS)
            // iOS-specific code
        #elif defined(SFML_SYSTEM_ANDROID)
            // Android-specific code
        #elif defined(SFML_SYSTEM_FREEBSD)
            // FreeBSD-specific code
        #endif
            }) {
        if (m_font.openFromFile(path)) {
            m_font.setSmooth(true);
            m_fontLoaded = true;
            break;
        }
    }
/*
    sf::Text statusText(m_font);
    if (m_fontLoaded) {
        std::string message("Game Arena");
        statusText.setString(message.c_str());
        statusText.setCharacterSize(12);
        statusText.setFillColor(sf::Color::Black);
        statusText.setPosition({10.f, 10.f});
        window.draw(statusText);
    } else {
        statusText.setString("No font found");
        statusText.setFillColor(sf::Color::Red);
    }
*/
}

//----------------------------------------------------------------------------------------
void Renderer::render(sf::RenderWindow& window, const GameState& gameState,
                      bool connectionLost, int /*localPlayerId*/) 
{
    // Update explosion animation (will be called with deltaTime from Game class)
    // Note: deltaTime should be passed, but for now we'll update in render
    // This will be fixed when integrated with Game class
    
    // Draw spacecraft
    drawSpacecraft(window, gameState.getSpacecraft(1));
    drawSpacecraft(window, gameState.getSpacecraft(2));
    
    // Draw projectiles
    for (const auto& projectile : gameState.getProjectiles()) {
        if (projectile.isActive()) {
            drawProjectile(window, projectile);
        }
    }
    
    // Draw explosions (if any active)
    if (m_explosionActive) {
        drawExplosion(window, m_explosionPosition, m_explosionRadius);
    }
    
    // Draw UI
    drawScore(window, gameState.getScore(1), gameState.getScore(2));
    drawConnectionStatus(window, true, connectionLost);  // TODO: pass actual connection status
    
    // Draw game over message if game is over
    if (gameState.isGameOver()) {
        drawGameOver(window, gameState.getWinner());
    }
}

//----------------------------------------------------------------------------------------
void Renderer::drawSpacecraft(sf::RenderWindow& window, const Spacecraft& spacecraft) 
{
    int playerId = spacecraft.getPlayerId();
    sf::Vector2f position = spacecraft.getPosition();
    float orientation = spacecraft.getOrientation();
    
    // Draw different shapes for different players
    if (playerId == 1) {
        drawSpacecraftShape1(window, position, orientation);
    } else {
        drawSpacecraftShape2(window, position, orientation);
    }
    
    // Draw thrust flame if thrusting
    if (spacecraft.isThrusting()) {
        drawThrustFlame(window, spacecraft);
    }
}

//----------------------------------------------------------------------------------------
void Renderer::drawSpacecraftShape1(sf::RenderWindow& window, sf::Vector2f position, float orientation) 
{
    // Triangle shape (pointing forward)
    // 0 degrees = pointing right (positive X direction)
    float size = 15.0f;
    sf::Vector2f front(size, 0);           // Front point (pointing right when orientation = 0)
    sf::Vector2f backLeft(-size/2, -size/2);   // Back left
    sf::Vector2f backRight(-size/2, size/2);   // Back right
    
    // Rotate points around origin, then translate to position
    front = rotatePoint(front, sf::Vector2f(0, 0), orientation) + position;
    backLeft = rotatePoint(backLeft, sf::Vector2f(0, 0), orientation) + position;
    backRight = rotatePoint(backRight, sf::Vector2f(0, 0), orientation) + position;
    
    // Draw triangle outline
    drawLine(window, front, backLeft, sf::Color::White);
    drawLine(window, front, backRight, sf::Color::White);
    drawLine(window, backLeft, backRight, sf::Color::White);
}

//----------------------------------------------------------------------------------------
void Renderer::drawSpacecraftShape2(sf::RenderWindow& window, sf::Vector2f position, float orientation) 
{
    // Diamond/rhombus shape (pointing forward)
    // 0 degrees = pointing right (positive X direction)
    float size = 15.0f;
    sf::Vector2f front(size, 0);           // Front point (pointing right when orientation = 0)
    sf::Vector2f right(0, -size/2);        // Right point
    sf::Vector2f back(-size/2, 0);         // Back point
    sf::Vector2f left(0, size/2);          // Left point
    
    // Rotate points around origin, then translate to position
    front = rotatePoint(front, sf::Vector2f(0, 0), orientation) + position;
    right = rotatePoint(right, sf::Vector2f(0, 0), orientation) + position;
    back = rotatePoint(back, sf::Vector2f(0, 0), orientation) + position;
    left = rotatePoint(left, sf::Vector2f(0, 0), orientation) + position;
    
    // Draw diamond outline
    drawLine(window, front, right, sf::Color::Cyan);
    drawLine(window, right, back, sf::Color::Cyan);
    drawLine(window, back, left, sf::Color::Cyan);
    drawLine(window, left, front, sf::Color::Cyan);
}

//----------------------------------------------------------------------------------------
void Renderer::drawThrustFlame(sf::RenderWindow& window, const Spacecraft& spacecraft) 
{
    sf::Vector2f position = spacecraft.getPosition();
    float orientation = spacecraft.getOrientation();
    
    // Calculate rear position (behind the spacecraft)
    float size = 15.0f;
    float angleRad = (orientation + 180.0f) * M_PI / 180.0f;  // Opposite direction
    sf::Vector2f rearOffset(
        std::cos(angleRad) * size * 0.7f,
        std::sin(angleRad) * size * 0.7f
    );
    sf::Vector2f rearPosition = position + rearOffset;
    
    // Draw flame as small lines/particles
    float flameLength = 8.0f;
    float spreadAngle = 20.0f;  // degrees
    
    // Draw 3 flame lines
    for (int i = -1; i <= 1; i++) {
        float flameAngle = angleRad + (i * spreadAngle * M_PI / 180.0f);
        sf::Vector2f flameEnd(
            rearPosition.x + std::cos(flameAngle) * flameLength,
            rearPosition.y + std::sin(flameAngle) * flameLength
        );
        drawLine(window, rearPosition, flameEnd, sf::Color::Yellow);
    }
}

//----------------------------------------------------------------------------------------
void Renderer::drawProjectile(sf::RenderWindow& window, const Projectile& projectile) 
{
    sf::Vector2f position = projectile.getPosition();
    
    // Draw as a small circle/dot
    float radius = Constants::PROJECTILE_SIZE;
    sf::CircleShape dot(radius);  // SFML 3.0: constructor takes radius
    dot.setPosition(sf::Vector2f(position.x - radius, position.y - radius));
    dot.setFillColor(sf::Color::White);
    window.draw(dot);
}

//----------------------------------------------------------------------------------------
void Renderer::drawExplosion(sf::RenderWindow& window, sf::Vector2f position, float radius) 
{
    // Draw explosion as expanding circles
    int numCircles = 3;
    for (int i = 0; i < numCircles; i++) {
        float circleRadius = radius * (1.0f - i * 0.3f);
        if (circleRadius > 0) {
            sf::CircleShape explosion(circleRadius);
            explosion.setPosition(sf::Vector2f(position.x - circleRadius, position.y - circleRadius));
            explosion.setFillColor(sf::Color::Transparent);
            explosion.setOutlineColor(sf::Color::Red);
            explosion.setOutlineThickness(2.0f);
            window.draw(explosion);
        }
    }
    
    // Draw explosion lines (burst pattern)
    int numLines = 8;
    for (int i = 0; i < numLines; i++) {
        float angle = (360.0f / numLines) * i * M_PI / 180.0f;
        sf::Vector2f end(
            position.x + std::cos(angle) * radius,
            position.y + std::sin(angle) * radius
        );
        // Use a custom orange color (255, 165, 0)
        drawLine(window, position, end, sf::Color(255, 165, 0));
    }
}

//----------------------------------------------------------------------------------------
void Renderer::drawScore(sf::RenderWindow& window, int score1, int score2) 
{
    // Draw scores in top corners
    std::string score1Text = "Player 1: " + std::to_string(score1);
    std::string score2Text = "Player 2: " + std::to_string(score2);
    
    // For SFML 3.0, Text requires a Font
    // Use default font if available, otherwise draw as simple shapes
    if (m_fontLoaded) {
        sf::Text text1(m_font, score1Text);
        text1.setCharacterSize(24);
        text1.setFillColor(sf::Color::White);
        text1.setPosition(sf::Vector2f(10, 10));
        
        sf::Text text2(m_font, score2Text);
        text2.setCharacterSize(24);
        text2.setFillColor(sf::Color::Cyan);
        text2.setPosition(sf::Vector2f(Constants::WINDOW_WIDTH - 200, 10));
        
        window.draw(text1);
        window.draw(text2);
    } else {
        // Fallback: Draw simple rectangles as placeholders for text
        // In a real implementation, you'd load a font file
        // For now, we'll skip text rendering if no font is loaded
    }
}

//----------------------------------------------------------------------------------------
void Renderer::drawConnectionStatus(sf::RenderWindow& window, bool connected, bool connectionLost) 
{
    if (!m_fontLoaded) {
        // Skip if no font loaded
        return;
    }
    
    std::string statusText;
    sf::Color statusColor;
    
    if (connectionLost) {
        statusText = "Connection Lost - Waiting for reconnection...";
        statusColor = sf::Color::Red;
    } else if (connected) {
        statusText = "Connected";
        statusColor = sf::Color::Green;
    } else {
        statusText = "Not Connected";
        statusColor = sf::Color::Yellow;
    }
    
    sf::Text text(m_font, statusText);
    text.setCharacterSize(18);
    text.setFillColor(statusColor);
    text.setPosition(sf::Vector2f(10, Constants::WINDOW_HEIGHT - 30));
    
    window.draw(text);
}

//----------------------------------------------------------------------------------------
void Renderer::drawGameOver(sf::RenderWindow& window, int winner) 
{
    if (!m_fontLoaded) {
        // Skip if no font loaded
        return;
    }
    
    std::string message;
    if (winner == 1) {
        message = "Player 1 Wins!";
    } else if (winner == 2) {
        message = "Player 2 Wins!";
    } else {
        message = "Game Over";
    }
    
    sf::Text text(m_font, message);
    text.setCharacterSize(48);
    text.setFillColor(sf::Color::Yellow);
    
    // Center the text (approximate - SFML 3.0 Rect API may differ)
    // For now, use a simple approximation based on character count
    float estimatedWidth = message.length() * 20.0f;  // Rough estimate
    float estimatedHeight = 48.0f;
    text.setPosition(sf::Vector2f(
        (Constants::WINDOW_WIDTH - estimatedWidth) / 2.0f,
        (Constants::WINDOW_HEIGHT - estimatedHeight) / 2.0f
    ));
    
    window.draw(text);
}

//----------------------------------------------------------------------------------------
sf::Vector2f Renderer::rotatePoint(sf::Vector2f point, sf::Vector2f center, float angleDegrees) 
{
    float angleRad = angleDegrees * M_PI / 180.0f;
    float cosA = std::cos(angleRad);
    float sinA = std::sin(angleRad);
    
    // Translate to origin
    point -= center;
    
    // Rotate
    float newX = point.x * cosA - point.y * sinA;
    float newY = point.x * sinA + point.y * cosA;
    
    // Translate back
    return sf::Vector2f(newX, newY) + center;
}

//----------------------------------------------------------------------------------------
void Renderer::drawLine(sf::RenderWindow& window, sf::Vector2f p1, sf::Vector2f p2, sf::Color color) 
{
    // Draw line using a rectangle (SFML doesn't have a direct line drawing function)
    sf::Vector2f diff = p2 - p1;
    float length = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    float angleRad = std::atan2(diff.y, diff.x);
    
    sf::RectangleShape line(sf::Vector2f(length, 1.0f));
    line.setPosition(p1);
    // SFML 3.0: use sf::degrees() to create Angle from degrees
    line.setRotation(sf::degrees(angleRad * 180.0f / M_PI));
    line.setFillColor(color);
    
    window.draw(line);
}

//----------------------------------------------------------------------------------------
void Renderer::triggerExplosion(sf::Vector2f position) 
{
    m_explosionPosition = position;
    m_explosionRadius = 0.0f;
    m_explosionTime = 0.0f;
    m_explosionActive = true;
}

//----------------------------------------------------------------------------------------
void Renderer::updateExplosion(float deltaTime) 
{
    if (m_explosionActive) {
        m_explosionTime += deltaTime;
        m_explosionRadius = (m_explosionTime / 0.5f) * 30.0f;  // 0.5s duration, 30px max radius
        
        if (m_explosionTime >= 0.5f) {
            m_explosionActive = false;
            m_explosionRadius = 0.0f;
        }
    }
}

