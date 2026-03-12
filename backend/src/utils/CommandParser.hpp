#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <cctype>

struct Command {
    std::string name;
    std::map<std::string, std::string> params;
};

class CommandParser {
public:
    static std::vector<Command> parseScript(std::string script);
    static Command parseLine(std::string line);
    static std::string trim(std::string str);
    static std::string toUpper(std::string str);
    
private:
    static std::vector<std::string> split(const std::string& str, char delimiter);
};

std::vector<Command> CommandParser::parseScript(std::string script) {
    std::vector<Command> commands;
    std::stringstream ss(script);
    std::string line;
    
    while (std::getline(ss, line)) {
        line = trim(line);
        
        if (line.empty()) {
            continue;
        }
        
        if (line[0] == '#') {
            Command cmd;
            cmd.name = "COMMENT";
            cmd.params["text"] = line;
            commands.push_back(cmd);
            continue;
        }
        
        Command cmd = parseLine(line);
        if (!cmd.name.empty()) {
            commands.push_back(cmd);
        }
    }
    
    return commands;
}

Command CommandParser::parseLine(std::string line) {
    Command cmd;
    line = trim(line);
    
    if (line.empty()) {
        return cmd;
    }
    
    std::vector<std::string> parts = split(line, ' ');
    
    if (parts.empty()) {
        return cmd;
    }
    
    cmd.name = toUpper(parts[0]);
    
    for (size_t i = 1; i < parts.size(); i++) {
        std::string param = parts[i];
        size_t equalPos = param.find('=');
        
        if (equalPos != std::string::npos) {
            std::string key = toUpper(trim(param.substr(0, equalPos)));
            std::string value = trim(param.substr(equalPos + 1));
            
            if (value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.length() - 2);
            }
            
            cmd.params[key] = value;
        }
    }
    
    return cmd;
}

std::string CommandParser::trim(std::string str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    
    if (start == std::string::npos) {
        return "";
    }
    
    return str.substr(start, end - start + 1);
}

std::string CommandParser::toUpper(std::string str) {
    for (char& c : str) {
        c = toupper(c);
    }
    return str;
}

std::vector<std::string> CommandParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(str);
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

#endif
