#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <string>
#include <memory>
#include <zmq.hpp>
#include "GameState.h"

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();
    
    // Connection management
    // For bidirectional communication, each player needs:
    // - localPort: port to bind for receiving (PULL socket)
    // - peerIp, peerPort: where to connect for sending (PUSH socket)
    bool connect(const std::string& peerIp, int peerPort, int localPort);
    void disconnect();
    bool isConnected() const { return m_connected; }
    
    // Message sending/receiving
    bool sendGameState(const GameState& gameState);
    bool receiveGameState(GameState& gameState);  // Non-blocking, returns false if no message
    
    // Connection status
    bool checkConnection();  // Check if connection is still alive
    bool isConnectionLost() const { return m_connectionLost; }
    void resetConnectionStatus() { m_connectionLost = false; }
    
private:
    std::unique_ptr<zmq::context_t> m_context;
    std::unique_ptr<zmq::socket_t> m_sendSocket;
    std::unique_ptr<zmq::socket_t> m_receiveSocket;
    
    bool m_connected;
    bool m_connectionLost;
    int m_localPort;
    std::string m_peerAddress;
    
    // Serialization
    std::string serializeGameState(const GameState& gameState);
    bool deserializeGameState(const std::string& data, GameState& gameState);
    
    // Helper to create socket address
    std::string createAddress(const std::string& ip, int port);
    std::string createLocalAddress(int port);
};

#endif // NETWORKMANAGER_H

