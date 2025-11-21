#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Spacecraft.h"
#include "Projectile.h"
#include <vector>

class GameState {
public:
    GameState();
    
    // Spacecraft access
    Spacecraft& getSpacecraft(int playerId);  // playerId is 1 or 2
    const Spacecraft& getSpacecraft(int playerId) const;
    
    // Projectile management
    void addProjectile(const Projectile& projectile);
    void updateProjectiles(float deltaTime);
    void removeInactiveProjectiles();
    const std::vector<Projectile>& getProjectiles() const { return m_projectiles; }
    std::vector<Projectile>& getProjectiles() { return m_projectiles; }
    
    // Score management
    int getScore(int playerId) const;  // playerId is 1 or 2
    void setScore(int playerId, int score);  // For synchronization
    void incrementScore(int playerId);
    void resetScores();
    bool hasWinner() const;
    int getWinner() const;  // Returns 1 or 2, or 0 if no winner
    
    // Game state
    void reset();  // Reset to initial state
    bool isGameOver() const { return m_gameOver; }
    void setGameOver(bool gameOver) { m_gameOver = gameOver; }
    
private:
    Spacecraft m_spacecraft1;
    Spacecraft m_spacecraft2;
    std::vector<Projectile> m_projectiles;
    int m_score1;
    int m_score2;
    bool m_gameOver;
    
    void initializeSpacecraft();
};

#endif // GAMESTATE_H

