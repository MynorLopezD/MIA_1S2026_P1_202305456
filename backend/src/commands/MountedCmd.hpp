#ifndef MOUNTEDCMD_HPP
#define MOUNTEDCMD_HPP

#include <string>
#include "../utils/CommandParser.hpp"

class MountedCmd {
public:
    static std::string execute(Command cmd);
};

#endif
