#include <iostream>
#include <string>
#include <map>
#include "network.hpp"
#include "server.hpp"

class Game {
private: 
    bool state_;
    std::string answer_;
    int score_;

public:
    std::queue<std::string> init_place_holder();
    std::queue<std::string> place_holder(std::string &s) { 
        std::queue<std::string> res;
        res.push(s);
        return res; 
    }
};

std::queue<std::string> handle_client(Message, std::unordered_map<int, Game> &);

int main(void) {
    std::string host = "127.0.0.1";
    std::string port = "1991";
    std::string protocol = "newline";
    std::string transport = "tcp";
    Network network;
    Server server = network.create_server(host, port, protocol, transport);
    std::unordered_map<int, Game> games;
    
    
    while (1) {
        server.tick();

        if (server.has_message()) {
            auto message = server.next();
            std::queue<std::string> response = handle_client(message, games);
            while (!response.empty()) {
                server.send(message.fd, response.front());
                response.pop();
            }
        }
    }

    return 0;
}

std::queue<std::string> handle_client(Message m, std::unordered_map<int, Game> &g) {
    std::queue<std::string> response;
    if (g.contains(m.fd))
        // process next step of game;
        response = g.at(m.fd).place_holder(m.payload);
    else {
        // Start new game;
        Game game;
        response = game.init_place_holder();
    }
    
    // Make enum for GAME_OVER, GAME_START, etc
    if (response.back() == "GAME OVER") { 
            g.erase(m.fd);
        }
    return response;
}

