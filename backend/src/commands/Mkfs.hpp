#ifndef MKFS_HPP
#define MKFS_HPP

#include <string>
#include "../utils/CommandParser.hpp"

class Mkfs {
public:
    static std::string execute(Command cmd);
};

#endif
