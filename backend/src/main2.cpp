#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include "httplib.h"
#include "structures/Structures.hpp"
#include "disk/DiskManager.hpp"
#include "utils/CommandParser.hpp"

#define SERVER_PORT 8080

std::map<std::string, MountedPartition> mountedPartitions;

struct Session {
    std::string user;
    std::string id;
};

Session* currentSession = nullptr;

std::string executeCommand(Command cmd) {
    std::string result;
    
    if (cmd.name == "MKDISK") {
        int size = 0;
        char fit = 'F';
        char unit = 'M';
        std::string path;
        
        if (cmd.params.count("-SIZE")) {
            size = stoi(cmd.params.at("-SIZE"));
        } else if (cmd.params.count("SIZE")) {
            size = stoi(cmd.params.at("SIZE"));
        } else {
            return "Error: Falta el parametro -size";
        }
        
        if (cmd.params.count("-PATH")) {
            path = cmd.params.at("-PATH");
        } else if (cmd.params.count("PATH")) {
            path = cmd.params.at("PATH");
        } else {
            return "Error: Falta el parametro -path";
        }
        
        if (cmd.params.count("-FIT")) {
            fit = cmd.params.at("-FIT")[0];
        } else if (cmd.params.count("FIT")) {
            fit = cmd.params.at("FIT")[0];
        }
        
        if (cmd.params.count("-UNIT")) {
            unit = cmd.params.at("-UNIT")[0];
        } else if (cmd.params.count("UNIT")) {
            unit = cmd.params.at("UNIT")[0];
        }
        
        if (unit == 'K') size *= 1024;
        else if (unit == 'M') size *= 1024 * 1024;
        
        if (size <= 0) {
            return "Error: El tamaño debe ser mayor a cero";
        }
        
        if (DiskManager::createDisk(path, size, fit)) {
            result = "Disco creado exitosamente: " + path;
        } else {
            result = "Error al crear el disco";
        }
    }
    else if (cmd.name == "RMDISK") {
        std::string path;
        
        if (cmd.params.count("-PATH")) {
            path = cmd.params.at("-PATH");
        } else if (cmd.params.count("PATH")) {
            path = cmd.params.at("PATH");
        } else {
            return "Error: Falta el parametro -path";
        }
        
        if (DiskManager::deleteDisk(path)) {
            result = "Disco eliminado exitosamente";
        } else {
            result = "Error al eliminar el disco";
        }
    }
    else if (cmd.name == "FDISK") {
        int size = 0;
        char unit = 'K';
        char type = 'P';
        char fit = 'W';
        std::string path;
        std::string name;
        
        if (cmd.params.count("-SIZE")) {
            size = stoi(cmd.params.at("-SIZE"));
        } else if (cmd.params.count("SIZE")) {
            size = stoi(cmd.params.at("SIZE"));
        } else {
            return "Error: Falta el parametro -size";
        }
        
        if (cmd.params.count("-PATH")) {
            path = cmd.params.at("-PATH");
        } else if (cmd.params.count("PATH")) {
            path = cmd.params.at("PATH");
        } else {
            return "Error: Falta el parametro -path";
        }
        
        if (cmd.params.count("-NAME")) {
            name = cmd.params.at("-NAME");
        } else if (cmd.params.count("NAME")) {
            name = cmd.params.at("NAME");
        } else {
            return "Error: Falta el parametro -name";
        }
        
        if (cmd.params.count("-UNIT")) {
            unit = cmd.params.at("-UNIT")[0];
        } else if (cmd.params.count("UNIT")) {
            unit = cmd.params.at("UNIT")[0];
        }
        
        if (cmd.params.count("-TYPE")) {
            type = cmd.params.at("-TYPE")[0];
        } else if (cmd.params.count("TYPE")) {
            type = cmd.params.at("TYPE")[0];
        }
        
        if (cmd.params.count("-FIT")) {
            fit = cmd.params.at("-FIT")[0];
        } else if (cmd.params.count("FIT")) {
            fit = cmd.params.at("FIT")[0];
        }
        
        if (unit == 'B') {} // bytes
        else if (unit == 'K') size *= 1024;
        else if (unit == 'M') size *= 1024 * 1024;
        
        if (size <= 0) {
            return "Error: El tamano debe ser mayor a cero";
        }
        
        if (DiskManager::createPartition(path, size, type, fit, name)) {
            result = "Particion creada exitosamente: " + name;
        } else {
            result = "Error al crear la particion";
        }
    }
    else if (cmd.name == "MOUNT") {
        std::string path;
        std::string name;
        
        if (cmd.params.count("-PATH")) {
            path = cmd.params.at("-PATH");
        } else if (cmd.params.count("PATH")) {
            path = cmd.params.at("PATH");
        } else {
            return "Error: Falta el parametro -path";
        }
        
        if (cmd.params.count("-NAME")) {
            name = cmd.params.at("-NAME");
        } else if (cmd.params.count("NAME")) {
            name = cmd.params.at("NAME");
        } else {
            return "Error: Falta el parametro -name";
        }
        
        MBR mbr = DiskManager::readMBR(path);
        
        int partitionIndex = -1;
        for (int i = 0; i < PARTITION_COUNT; i++) {
            std::string partName(mbr.mbr_partitions[i].part_name);
            if (partName == name && mbr.mbr_partitions[i].part_s > 0) {
                partitionIndex = i;
                break;
            }
        }
        
        if (partitionIndex == -1) {
            return "Error: Particion no encontrada";
        }
        
        mbr.mbr_partitions[partitionIndex].part_status = '1';
        mbr.mbr_partitions[partitionIndex].part_correlative = 1;
        DiskManager::writeMBR(path, mbr);
        
        MountedPartition mp;
        mp.path = path;
        mp.name = name;
        mp.number = 1;
        
        std::string id = "1" + std::to_string(partitionIndex + 1) + "A";
        mp.id = id;
        
        mountedPartitions[id] = mp;
        
        result = "Particion montada exitosamente con ID: " + id;
    }
    else if (cmd.name == "MOUNTED") {
        if (mountedPartitions.empty()) {
            return "No hay particiones montadas";
        }
        
        result = "Particiones montadas:\n";
        for (auto& mp : mountedPartitions) {
            result += "ID: " + mp.first + " | Path: " + mp.second.path + " | Nombre: " + mp.second.name + "\n";
        }
    }
    else if (cmd.name == "COMMENT") {
        result = cmd.params.at("text");
    }
    else {
        result = "Comando no reconocido: " + cmd.name;
    }
    
    return result;
}

std::string executeScript(std::string script) {
    std::vector<Command> commands = CommandParser::parseScript(script);
    std::string result;
    
    for (Command& cmd : commands) {
        if (cmd.name == "COMMENT") {
            result += executeCommand(cmd) + "\n";
        } else {
            result += ">> " + cmd.name + "\n";
            result += executeCommand(cmd) + "\n\n";
        }
    }
    
    return result;
}

int main() {
    httplib::Server svr;
    
    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content("C++Disk Backend - Sistema de Archivos EXT2", "text/plain");
    });
    
    svr.Post("/execute", [](const httplib::Request& req, httplib::Response& res) {
        std::string script = req.body;
        std::string result = executeScript(script);
        res.set_content(result, "text/plain");
    });
    
    svr.Post("/upload-script", [](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_file("file")) {
            res.set_content("Error: No se proporciono archivo", "text/plain");
            return;
        }
        
        const auto& file = req.get_file_value("file");
        std::string content = file.content;
        res.set_content(content, "text/plain");
    });
    
    std::cout << "Servidor iniciado en http://localhost:" << SERVER_PORT << std::endl;
    std::cout << "Endpoints disponibles:" << std::endl;
    std::cout << "  GET  /           - Verificar que el servidor esta funcionando" << std::endl;
    std::cout << "  POST /execute   - Ejecutar comandos" << std::endl;
    std::cout << "  POST /upload-script - Cargar archivo de script" << std::endl;
    
    svr.listen("0.0.0.0", SERVER_PORT);
    
    return 0;
}