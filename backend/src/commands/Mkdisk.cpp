#include "Mkdisk.hpp"
#include "../disk/DiskManager.hpp"
#include <iostream>

std::string Mkdisk::execute(Command cmd) {
    if (cmd.params.find("SIZE") == cmd.params.end() || cmd.params.find("PATH") == cmd.params.end()) {
        return "Error: Parametros insuficientes para mkdisk. Use -size=X -path=ruta";
    }

    int size = std::stoi(cmd.params["SIZE"]);
    std::string path = cmd.params["PATH"];
    
    char fit = 'F';
    if (cmd.params.find("FIT") != cmd.params.end()) {
        std::string fitStr = cmd.params["FIT"];
        if (fitStr == "BF") fit = 'B';
        else if (fitStr == "FF") fit = 'F';
        else if (fitStr == "WF") fit = 'W';
    }
    
    char unit = 'M';
    if (cmd.params.find("UNIT") != cmd.params.end()) {
        unit = cmd.params["UNIT"][0];
    }
    
    int sizeInBytes = size;
    if (unit == 'K') {
        sizeInBytes = size * 1024;
    } else if (unit == 'M') {
        sizeInBytes = size * 1024 * 1024;
    }
    
    if (sizeInBytes < 0) {
        return "Error: El tamano debe ser positivo";
    }

    if (DiskManager::createDisk(path, sizeInBytes, fit)) {
        return "Disk created successfully: " + path;
    } else {
        return "Error: No se pudo crear el disco";
    }
}
