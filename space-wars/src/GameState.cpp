#include "GameState.h"
#include "Constants.h"
#include <algorithm>

GameState::GameState()
    : m_score1(0)
    , m_score2(0)
    , m_gameOver(false) {
    initializeSpacecraft();
}

void GameState::initializeSpacecraft() {
    // Spawn both spacecraft at center of screen
    sf::Vector2f center1(
        Constants::WINDOW_WIDTH / 2.0f - 40,
        Constants::WINDOW_HEIGHT / 2.0f
    );
    sf::Vector2f center2(
        Constants::WINDOW_WIDTH / 2.0f + 40,
        Constants::WINDOW_HEIGHT / 2.0f
    );
    
    // Spacecraft 1 faces right (0 degrees), Spacecraft 2 faces left (180 degrees)
    // They start facing away from each other
    m_spacecraft1 = Spacecraft(center1, 180.0f, 1);
    m_spacecraft2 = Spacecraft(center2, 0.0f, 2);
}

Spacecraft& GameState::getSpacecraft(int playerId) {
    if (playerId == 1) {
        return m_spacecraft1;
    } else {
        return m_spacecraft2;
    }
}

const Spacecraft& GameState::getSpacecraft(int playerId) const {
    if (playerId == 1) {
        return m_spacecraft1;
    } else {
        return m_spacecraft2;
    }
}

void GameState::addProjectile(const Projectile& projectile) {
    m_projectiles.push_back(projectile);
}

void GameState::updateProjectiles(float deltaTime) {
    for (auto& projectile : m_projectiles) {
        projectile.update(deltaTime);
    }
}

void GameState::removeInactiveProjectiles() {
    m_projectiles.erase(
        std::remove_if(
            m_projectiles.begin(),
            m_projectiles.end(),
            [](const Projectile& p) { return !p.isActive(); }
        ),
        m_projectiles.end()
    );
}

int GameState::getScore(int playerId) const {
    if (playerId == 1) {
        return m_score1;
    } else {
        return m_score2;
    }
}

void GameState::setScore(int playerId, int score) {
    if (playerId == 1) {
        m_score1 = score;
    } else {
        m_score2 = score;
    }
    
    // Check for winner after setting score
    if (m_score1 >= Constants::WIN_SCORE || m_score2 >= Constants::WIN_SCORE) {
        m_gameOver = true;
    }
}

void GameState::incrementScore(int playerId) {
    if (playerId == 1) {
        m_score1++;
    } else {
        m_score2++;
    }
    
    // Check for winner
    if (m_score1 >= Constants::WIN_SCORE || m_score2 >= Constants::WIN_SCORE) {
        m_gameOver = true;
    }
}

void GameState::resetScores() {
    m_score1 = 0;
    m_score2 = 0;
}

bool GameState::hasWinner() const {
    return (m_score1 >= Constants::WIN_SCORE || m_score2 >= Constants::WIN_SCORE);
}

int GameState::getWinner() const {
    if (m_score1 >= Constants::WIN_SCORE) {
        return 1;
    } else if (m_score2 >= Constants::WIN_SCORE) {
        return 2;
    } else {
        return 0;
    }
}

void GameState::reset() {
    resetScores();
    m_gameOver = false;
    m_projectiles.clear();
    initializeSpacecraft();
}

