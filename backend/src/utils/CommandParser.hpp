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

#endif
