#include "Fdisk.hpp"
#include "../disk/DiskManager.hpp"
#include "../structures/Structures.hpp"
#include <iostream>
#include <sstream>

std::string Fdisk::execute(Command cmd) {
    if (cmd.params.find("PATH") == cmd.params.end() || cmd.params.find("NAME") == cmd.params.end()) {
        return "Error: Parametros insuficientes para fdisk. Use -path=ruta -name=nombre";
    }

    std::string path = cmd.params["PATH"];
    std::string name = cmd.params["NAME"];

    if (cmd.params.find("DELETE") != cmd.params.end()) {
        if (DiskManager::deletePartition(path, name)) {
            return "Partition deleted successfully: " + name;
        } else {
            return "Error: No se pudo eliminar la particion";
        }
    }

    int size = 0;
    if (cmd.params.find("SIZE") != cmd.params.end()) {
        size = std::stoi(cmd.params["SIZE"]);
    }

    char type = 'P';
    if (cmd.params.find("TYPE") != cmd.params.end()) {
        std::string typeStr = cmd.params["TYPE"];
        if (typeStr == "E") type = 'E';
        else if (typeStr == "L") type = 'L';
        else type = 'P';
    }

    char fit = 'W';
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
    if (unit == 'B') {
        sizeInBytes = size;
    } else if (unit == 'K') {
        sizeInBytes = size * 1024;
    } else if (unit == 'M') {
        sizeInBytes = size * 1024 * 1024;
    }

    if (DiskManager::createPartition(path, sizeInBytes, type, fit, name)) {
        return "Partition created successfully: " + name;
    } else {
        return "Error: No se pudo crear la particion";
    }
}
