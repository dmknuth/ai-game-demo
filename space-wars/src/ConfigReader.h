#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <string>

struct NetworkConfig {
    std::string hostIp;
    int hostPort;
    std::string clientIp;
    int clientPort;
    int hostPlayerId;      // Which player this configuration is for (1 or 2)
    int clientPlayerId;    // Which player we're connecting to (1 or 2)
    
    NetworkConfig()
        : hostIp("127.0.0.1")
        , hostPort(5555)
        , clientIp("127.0.0.1")
        , clientPort(5556)
        , hostPlayerId(1)
        , clientPlayerId(2)
    {}
};

class ConfigReader {
public:
    ConfigReader();
    ~ConfigReader();
    
    // Read configuration from file
    // Returns true if successful, false otherwise
    bool readConfig(const std::string& filename, NetworkConfig& config);
    
    // Validate IP address format (basic validation)
    static bool isValidIpAddress(const std::string& ip);
    
    // Validate port number (1-65535)
    static bool isValidPort(int port);
    
private:
    // Parse a line from the configuration file
    // Format: key=value or key = value (whitespace is trimmed)
    bool parseLine(const std::string& line, std::string& key, std::string& value);
    
    // Trim whitespace from string
    static std::string trim(const std::string& str);
    
    // Convert string to integer, returns false if invalid
    static bool stringToInt(const std::string& str, int& value);
};

#endif // CONFIGREADER_H

