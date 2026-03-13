#include "MountedPartitionManager.hpp"
#include "../structures/Structures.hpp"
#include <algorithm>

static std::vector<MountedPartition> mountedPartitions;

std::vector<MountedPartition>& MountedPartitionManager::getMountedPartitions() {
    return mountedPartitions;
}

MountedPartition* MountedPartitionManager::findById(std::string id) {
    for (auto& mp : mountedPartitions) {
        if (mp.id == id) {
            return &mp;
        }
    }
    return nullptr;
}

MountedPartition* MountedPartitionManager::findByPathAndName(std::string path, std::string name) {
    for (auto& mp : mountedPartitions) {
        if (mp.path == path && mp.name == name) {
            return &mp;
        }
    }
    return nullptr;
}

std::string MountedPartitionManager::generateId(std::string path) {
    std::string baseId = "56";
    
    int sameDiskCount = 0;
    char maxLetter = 'A';
    int maxNumber = 0;
    
    for (const auto& mp : mountedPartitions) {
        if (mp.path == path) {
            sameDiskCount++;
            if (mp.id.length() >= 4) {
                char letter = mp.id[3];
                if (letter > maxLetter) {
                    maxLetter = letter;
                }
                if (mp.id.length() >= 3) {
                    int num = mp.id[2] - '0';
                    if (num > maxNumber) {
                        maxNumber = num;
                    }
                }
            }
        }
    }
    
    if (sameDiskCount == 0) {
        return baseId + "1A";
    } else {
        char letter = 'A';
        int num = sameDiskCount + 1;
        return baseId + std::to_string(num) + letter;
    }
}

void MountedPartitionManager::add(MountedPartition mp) {
    mountedPartitions.push_back(mp);
}

bool MountedPartitionManager::remove(std::string id) {
    for (auto it = mountedPartitions.begin(); it != mountedPartitions.end(); ++it) {
        if (it->id == id) {
            mountedPartitions.erase(it);
            return true;
        }
    }
    return false;
}
