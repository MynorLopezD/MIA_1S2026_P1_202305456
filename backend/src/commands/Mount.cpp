#include "Mount.hpp"
#include "../disk/DiskManager.hpp"
#include "../disk/MountedPartitionManager.hpp"
#include "../structures/Structures.hpp"
#include <iostream>
#include <fstream>

std::string Mount::execute(Command cmd) {
    if (cmd.params.find("PATH") == cmd.params.end() || cmd.params.find("NAME") == cmd.params.end()) {
        return "Error: Parametros insuficientes para mount. Use -path=ruta -name=nombre";
    }

    std::string path = cmd.params["PATH"];
    std::string name = cmd.params["NAME"];

    std::ifstream disk(path, std::ios::binary | std::ios::in);
    if (!disk.is_open()) {
        return "Error: No se pudo abrir el disco en " + path;
    }
    disk.close();

    MBR mbr = DiskManager::readMBR(path);

    int partitionIndex = -1;
    for (int i = 0; i < PARTITION_COUNT; i++) {
        std::string partName(mbr.mbr_partitions[i].part_name);
        if (partName == name && mbr.mbr_partitions[i].part_type == 'P') {
            partitionIndex = i;
            break;
        }
    }

    if (partitionIndex == -1) {
        return "Error: Particion primaria no encontrada: " + name;
    }

    if (MountedPartitionManager::findByPathAndName(path, name) != nullptr) {
        return "Error: La particion ya esta montada";
    }

    MountedPartition mp;
    mp.id = MountedPartitionManager::generateId(path);
    mp.path = path;
    mp.name = name;
    mp.number = mbr.mbr_partitions[partitionIndex].part_start;
    mp.fit = mbr.mbr_partitions[partitionIndex].part_fit;

    mbr.mbr_partitions[partitionIndex].part_status = '1';
    strncpy(mbr.mbr_partitions[partitionIndex].part_id, mp.id.c_str(), 4);
    mbr.mbr_partitions[partitionIndex].part_correlative = 1;

    DiskManager::writeMBR(path, mbr);

    MountedPartitionManager::add(mp);

    return "Partition mounted successfully: " + mp.id;
}
