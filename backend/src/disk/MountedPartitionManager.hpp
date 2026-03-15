#ifndef MOUNTED_MANAGER_HPP
#define MOUNTED_MANAGER_HPP

#include <vector>
#include <string>
#include "../structures/Structures.hpp"

class MountedPartitionManager {
public:
    static std::vector<MountedPartition>& getMountedPartitions();
    static MountedPartition* findById(std::string id);
    static MountedPartition* getPartitionById(std::string id);
    static MountedPartition* findByPathAndName(std::string path, std::string name);
    static std::string generateId(std::string path);
    static void add(MountedPartition mp);
    static bool remove(std::string id);
};

#endif
