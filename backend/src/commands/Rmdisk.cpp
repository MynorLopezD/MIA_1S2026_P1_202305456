#include "Rmdisk.hpp"
#include "../disk/DiskManager.hpp"
#include <iostream>

std::string Rmdisk::execute(Command cmd) {
    if (cmd.params.find("PATH") == cmd.params.end()) {
        return "Error: Parametro -path requerido para rmdisk";
    }

    std::string path = cmd.params["PATH"];

    if (DiskManager::deleteDisk(path)) {
        return "Disk deleted successfully: " + path;
    } else {
        return "Error: No se pudo eliminar el disco";
    }
}
