#ifndef RMDISK_HPP
#define RMDISK_HPP

#include <string>
#include "../utils/CommandParser.hpp"

class Rmdisk {
public:
    static std::string execute(Command cmd);
};

#endif
