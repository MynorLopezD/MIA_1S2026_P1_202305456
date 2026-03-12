#include "crow.h"
#include <nlohmann/json.hpp>
#include <iostream>

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/ping")([]() {
        nlohmann::json res;
        res["status"] = "ok";
        res["message"] = "EXT2 Backend running on Windows";
        return res.dump();
    });

    std::cout << "Server en http://localhost:8080" << std::endl;
    app.port(8080).multithreaded().run();
    return 0;
}