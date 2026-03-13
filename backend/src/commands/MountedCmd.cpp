#include "MountedCmd.hpp"
#include "../disk/MountedPartitionManager.hpp"
#include "../structures/Structures.hpp"
#include <sstream>

std::string MountedCmd::execute(Command cmd) {
    auto& mounted = MountedPartitionManager::getMountedPartitions();
    
    if (mounted.empty()) {
        return "No hay particiones montadas";
    }

    std::ostringstream oss;
    oss << "Particiones montadas:\n";
    oss << "ID\t\tPath\t\tName\n";
    
    for (const auto& mp : mounted) {
        oss << mp.id << "\t" << mp.path << "\t" << mp.name << "\n";
    }
    
    return oss.str();
}
