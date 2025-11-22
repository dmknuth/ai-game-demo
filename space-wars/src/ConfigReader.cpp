#include "ConfigReader.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

//----------------------------------------------------------------------------------------
ConfigReader::ConfigReader() 
{
}

//----------------------------------------------------------------------------------------
ConfigReader::~ConfigReader() 
{
}

//----------------------------------------------------------------------------------------
std::string ConfigReader::trim(const std::string& str) 
{
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

//----------------------------------------------------------------------------------------
bool ConfigReader::parseLine(const std::string& line, std::string& key, std::string& value) 
{
    // Skip empty lines and comments (lines starting with #)
    std::string trimmed = trim(line);
    if (trimmed.empty() || trimmed[0] == '#') {
        return false;
    }
    
    // Find the equals sign
    size_t eqPos = trimmed.find('=');
    if (eqPos == std::string::npos) {
        return false;
    }
    
    // Extract key and value
    key = trim(trimmed.substr(0, eqPos));
    value = trim(trimmed.substr(eqPos + 1));
    
    return !key.empty() && !value.empty();
}

//----------------------------------------------------------------------------------------
bool ConfigReader::stringToInt(const std::string& str, int& value) 
{
    try {
        value = std::stoi(str);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

//----------------------------------------------------------------------------------------
bool ConfigReader::isValidIpAddress(const std::string& ip) 
{
    if (ip.empty()) {
        return false;
    }
    
    // Basic IPv4 validation using regex
    // Pattern: 0-255.0-255.0-255.0-255
    std::regex ipPattern(R"(^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$)");
    std::smatch matches;
    
    if (!std::regex_match(ip, matches, ipPattern)) {
        return false;
    }
    
    // Check each octet is 0-255
    for (int i = 1; i <= 4; ++i) {
        int octet = std::stoi(matches[i].str());
        if (octet < 0 || octet > 255) {
            return false;
        }
    }
    
    return true;
}

//----------------------------------------------------------------------------------------
bool ConfigReader::isValidPort(int port) 
{
    return port >= 1 && port <= 65535;
}

//----------------------------------------------------------------------------------------
bool ConfigReader::readConfig(const std::string& filename, NetworkConfig& config) 
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool hasHostIp = false;
    bool hasHostPort = false;
    bool hasClientIp = false;
    bool hasClientPort = false;
    
    while (std::getline(file, line)) {
        std::string key, value;
        if (!parseLine(line, key, value)) {
            continue;  // Skip empty lines and comments
        }
        
        // Convert key to lowercase for case-insensitive matching
        std::string lowerKey = key;
        std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
        
        if (lowerKey == "host_ip" || lowerKey == "hostip") {
            if (!isValidIpAddress(value)) {
                return false;  // Invalid IP address
            }
            config.hostIp = value;
            hasHostIp = true;
        } else if (lowerKey == "host_port" || lowerKey == "hostport") {
            int port;
            if (!stringToInt(value, port) || !isValidPort(port)) {
                return false;  // Invalid port
            }
            config.hostPort = port;
            hasHostPort = true;
        } else if (lowerKey == "client_ip" || lowerKey == "clientip") {
            if (!isValidIpAddress(value)) {
                return false;  // Invalid IP address
            }
            config.clientIp = value;
            hasClientIp = true;
        } else if (lowerKey == "client_port" || lowerKey == "clientport") {
            int port;
            if (!stringToInt(value, port) || !isValidPort(port)) {
                return false;  // Invalid port
            }
            config.clientPort = port;
            hasClientPort = true;
        }
        // Ignore unknown keys (for future extensibility)
    }
    
    file.close();
    
    // Check that all required fields are present
    return hasHostIp && hasHostPort && hasClientIp && hasClientPort;
}

