#include "NetworkManager.h"
#include <sstream>
#include <iostream>
#include <chrono>

//----------------------------------------------------------------------------------------
NetworkManager::NetworkManager()
    : m_connected(false)
    , m_connectionLost(false)
    , m_localPort(0) 
{
    try {
        m_context = std::make_unique<zmq::context_t>(1);
    } catch (const std::exception& e) {
        std::cerr << "Failed to create ZeroMQ context: " << e.what() << std::endl;
    }
}

//----------------------------------------------------------------------------------------
NetworkManager::~NetworkManager() 
{
    disconnect();
    // Context will be automatically destroyed by unique_ptr
    // ZeroMQ contexts should be destroyed after all sockets are closed
}

//----------------------------------------------------------------------------------------
std::string NetworkManager::createAddress(const std::string& ip, int port) 
{
    return "tcp://" + ip + ":" + std::to_string(port);
}

//----------------------------------------------------------------------------------------
std::string NetworkManager::createLocalAddress(int port) 
{
    return "tcp://*:" + std::to_string(port);
}

//----------------------------------------------------------------------------------------
bool NetworkManager::connect(const std::string& peerIp, int peerPort, int localPort) 
{
    try {
        disconnect();
        
        // Create PULL socket for receiving (bind locally)
        m_receiveSocket = std::make_unique<zmq::socket_t>(*m_context, ZMQ_PULL);
        std::string receiveAddress = createLocalAddress(localPort);
        m_receiveSocket->bind(receiveAddress);
        
        // Create PUSH socket for sending (connect to peer)
        m_sendSocket = std::make_unique<zmq::socket_t>(*m_context, ZMQ_PUSH);
        std::string sendAddress = createAddress(peerIp, peerPort);
        m_sendSocket->connect(sendAddress);
        
        // Set socket options for non-blocking receive
        int timeout = 100;  // 100ms timeout
        m_receiveSocket->set(zmq::sockopt::rcvtimeo, timeout);
        
        // Set linger to 0 so sockets close immediately (prevents hanging on shutdown)
        int linger = 0;
        m_receiveSocket->set(zmq::sockopt::linger, linger);
        m_sendSocket->set(zmq::sockopt::linger, linger);
        
        m_connected = true;
        m_connectionLost = false;
        m_localPort = localPort;
        m_peerAddress = sendAddress;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to connect: " << e.what() << std::endl;
        m_connected = false;
        return false;
    }
}

//----------------------------------------------------------------------------------------
void NetworkManager::disconnect() 
{
    // Set linger to 0 on sockets before destroying them to prevent hanging on shutdown
    // This ensures sockets close immediately rather than waiting for pending messages
    try {
        if (m_receiveSocket) {
            // Set linger to 0 to close immediately (no waiting for pending messages)
            m_receiveSocket->set(zmq::sockopt::linger, 0);
        }
        if (m_sendSocket) {
            // Set linger to 0 to close immediately (no waiting for pending messages)
            m_sendSocket->set(zmq::sockopt::linger, 0);
        }
    } catch (const std::exception& e) {
        // Ignore errors during cleanup - we're shutting down anyway
        // Socket might already be closed or in an invalid state
    }
    
    // Reset the unique pointers - sockets will be automatically closed/destroyed
    // Setting linger=0 ensures they close immediately without blocking
    m_sendSocket.reset();
    m_receiveSocket.reset();
    m_connected = false;
    m_peerAddress.clear();
}

//----------------------------------------------------------------------------------------
std::string NetworkManager::serializeGameState(const GameState& gameState) 
{
    std::ostringstream oss;
    
    // Serialize spacecraft 1
    const Spacecraft& sc1 = gameState.getSpacecraft(1);
    sf::Vector2f pos1 = sc1.getPosition();
    oss << "SC1:" << pos1.x << "," << pos1.y << "," 
        << sc1.getOrientation() << "," 
        << sc1.getVelocity().x << "," << sc1.getVelocity().y << ";";
    
    // Serialize spacecraft 2
    const Spacecraft& sc2 = gameState.getSpacecraft(2);
    sf::Vector2f pos2 = sc2.getPosition();
    oss << "SC2:" << pos2.x << "," << pos2.y << "," 
        << sc2.getOrientation() << "," 
        << sc2.getVelocity().x << "," << sc2.getVelocity().y << ";";
    
    // Serialize projectiles
    oss << "PROJ:";
    const auto& projectiles = gameState.getProjectiles();
    for (const auto& proj : projectiles) {
        if (proj.isActive()) {
            sf::Vector2f ppos = proj.getPosition();
            oss << ppos.x << "," << ppos.y << "," 
                << proj.getVelocity().x << "," << proj.getVelocity().y << "," 
                << proj.getOwnerPlayerId() << "|";
        }
    }
    oss << ";";
    
    // Serialize scores
    oss << "SCORE:" << gameState.getScore(1) << "," << gameState.getScore(2) << ";";
    
    // Serialize game over status
    oss << "GAMEOVER:" << (gameState.isGameOver() ? "1" : "0") << ";";
    
    return oss.str();
}

//----------------------------------------------------------------------------------------
bool NetworkManager::deserializeGameState(const std::string& data, GameState& gameState) 
{
    try {
        std::istringstream iss(data);
        std::string token;
        
        std::cout << "data: " << data << std::endl;
        // Parse spacecraft 1
        if (std::getline(iss, token, ';') && token.substr(0, 4) == "SC1:") {
            std::string scData = token.substr(4);
            std::istringstream scStream(scData);
            std::string val;
            float values[5];
            int i = 0;
            while (std::getline(scStream, val, ',') && i < 5) {
                values[i++] = std::stof(val);
            }
            if (i == 5) {
                Spacecraft& sc1 = gameState.getSpacecraft(1);
                sc1.setPosition(sf::Vector2f(values[0], values[1]));
                sc1.setOrientation(values[2]);
                sc1.setVelocity(sf::Vector2f(values[3], values[4]));
            }
        }
        
        // Parse spacecraft 2
        if (std::getline(iss, token, ';') && token.substr(0, 4) == "SC2:") {
            std::string scData = token.substr(4);
            std::istringstream scStream(scData);
            std::string val;
            float values[5];
            int i = 0;
            while (std::getline(scStream, val, ',') && i < 5) {
                values[i++] = std::stof(val);
            }
            if (i == 5) {
                Spacecraft& sc2 = gameState.getSpacecraft(2);
                sc2.setPosition(sf::Vector2f(values[0], values[1]));
                sc2.setOrientation(values[2]);
                sc2.setVelocity(sf::Vector2f(values[3], values[4]));
            }
        }
        
        // Parse projectiles
        if (std::getline(iss, token, ';') && token.substr(0, 5) == "PROJ:") {
            std::string projData = token.substr(5);
            if (!projData.empty()) {
                std::istringstream projStream(projData);
                std::string projToken;
                while (std::getline(projStream, projToken, '|')) {
                    if (!projToken.empty()) {
                        std::istringstream pStream(projToken);
                        std::vector<std::string> parts;
                        std::string part;
                        while (std::getline(pStream, part, ',')) {
                            parts.push_back(part);
                        }
                        if (parts.size() == 5) {
                            float posX = std::stof(parts[0]);
                            float posY = std::stof(parts[1]);
                            float velX = std::stof(parts[2]);
                            float velY = std::stof(parts[3]);
                            int ownerId = std::stoi(parts[4]);
                            sf::Vector2f pos(posX, posY);
                            sf::Vector2f vel(velX, velY);
                            Projectile proj(pos, vel, ownerId);
                            gameState.addProjectile(proj);
                        }
                    }
                }
            }
        }
        
        // Parse scores
        if (std::getline(iss, token, ';') && token.substr(0, 6) == "SCORE:") {
            std::string scoreData = token.substr(6);
            std::istringstream scoreStream(scoreData);
            std::string val;
            if (std::getline(scoreStream, val, ',')) {
                int score1 = std::stoi(val);
                if (std::getline(scoreStream, val, ',')) {
                    int score2 = std::stoi(val);
                    gameState.setScore(1, score1);
                    gameState.setScore(2, score2);
                }
            }
        }
        
        // Parse game over
        if (std::getline(iss, token, ';') && token.substr(0, 9) == "GAMEOVER:") {
            std::string gameOverStr = token.substr(9);
            bool gameOver = (gameOverStr == "1");
            gameState.setGameOver(gameOver);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to deserialize game state: " << e.what() << std::endl;
        return false;
    }
}

//----------------------------------------------------------------------------------------
bool NetworkManager::sendGameState(const GameState& gameState) 
{
    if (!m_connected || !m_sendSocket) {
        std::cout << "No game state sent" << std::endl;
        return false;
    }
    
    try {
        std::string data = serializeGameState(gameState);
        zmq::message_t message(data.size());
        memcpy(message.data(), data.c_str(), data.size());
        
        zmq::send_result_t result = m_sendSocket->send(message, zmq::send_flags::dontwait);
        if (!result.has_value()) {
            m_connectionLost = true;
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to send game state: " << e.what() << std::endl;
        m_connectionLost = true;
        return false;
    }
}

//----------------------------------------------------------------------------------------
bool NetworkManager::receiveGameState(GameState& gameState) 
{
    if (!m_connected || !m_receiveSocket) {
        std::cout << "No game state received" << std::endl;
        return false;
    }
    
    try {
        zmq::message_t message;
        zmq::recv_result_t result = m_receiveSocket->recv(message, zmq::recv_flags::dontwait);
        
        if (!result.has_value()) {
            // No message available (non-blocking)
            return false;
        }
        
        std::string data(static_cast<char*>(message.data()), message.size());
        bool success = deserializeGameState(data, gameState);
        
        if (!success) {
            m_connectionLost = true;
        }
        
        return success;
    } catch (const std::exception& e) {
        std::cerr << "Failed to receive game state: " << e.what() << std::endl;
        m_connectionLost = true;
        return false;
    }
}

//----------------------------------------------------------------------------------------
bool NetworkManager::checkConnection() 
{
    if (!m_connected) {
        return false;
    }
    
    // Try to send a ping or check socket state
    // For now, we'll rely on send/receive failures to detect disconnection
    return !m_connectionLost;
}

