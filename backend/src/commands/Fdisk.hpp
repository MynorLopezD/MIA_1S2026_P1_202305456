#ifndef FDISK_HPP
#define FDISK_HPP

#include <string>
#include "../utils/CommandParser.hpp"

class Fdisk {
public:
    static std::string execute(Command cmd);
};

#endif
