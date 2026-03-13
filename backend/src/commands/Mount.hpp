#ifndef MOUNT_HPP
#define MOUNT_HPP

#include <string>
#include "../utils/CommandParser.hpp"

class Mount {
public:
    static std::string execute(Command cmd);
};

#endif
