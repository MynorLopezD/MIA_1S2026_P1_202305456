#include "crow.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include "utils/CommandParser.hpp"
#include "commands/Mkdisk.hpp"
#include "commands/Rmdisk.hpp"
#include "commands/Fdisk.hpp"
#include "commands/Mount.hpp"
#include "commands/MountedCmd.hpp"

std::string executeCommand(Command cmd) {
    std::string name = cmd.name;
    
    if (name == "MKDISK") {
        return Mkdisk::execute(cmd);
    } else if (name == "RMDISK") {
        return Rmdisk::execute(cmd);
    } else if (name == "FDISK") {
        return Fdisk::execute(cmd);
    } else if (name == "MOUNT") {
        return Mount::execute(cmd);
    } else if (name == "MOUNTED") {
        return MountedCmd::execute(cmd);
    } else {
        return "Error: Comando no reconocido: " + name;
    }
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/ping")([]() {
        nlohmann::json res;
        res["status"] = "ok";
        res["message"] = "EXT2 Backend running on Windows";
        return res.dump();
    });

    CROW_ROUTE(app, "/execute").methods("POST"_method)([](const crow::request& req) {
        nlohmann::json response;
        
        try {
            auto body = nlohmann::json::parse(req.body);
            
            if (body.find("command") == body.end()) {
                response["output"] = "Error: No se proporciono el comando";
                return response.dump();
            }
            
            std::string commandStr = body["command"];
            auto commands = CommandParser::parseScript(commandStr);
            
            std::string output;
            for (const auto& cmd : commands) {
                if (cmd.name == "COMMENT" || cmd.name.empty()) {
                    continue;
                }
                output += executeCommand(cmd) + "\n";
            }
            
            response["output"] = output;
            
        } catch (const std::exception& e) {
            response["output"] = std::string("Error: ") + e.what();
        }
        
        return response.dump();
    });

    std::cout << "Server en http://localhost:8080" << std::endl;
    app.port(8080).multithreaded().run();
    return 0;
}
