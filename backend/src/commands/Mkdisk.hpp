#ifndef MKDISK_HPP
#define MKDISK_HPP

#include <string>
#include "../utils/CommandParser.hpp"

class Mkdisk {
public:
    static std::string execute(Command cmd);
};

#endif
