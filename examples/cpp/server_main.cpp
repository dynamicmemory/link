// game.hpp
#include <string>
#include <queue>
#include <unordered_map>
#include <cctype>
#include <fstream>
#include <algorithm>
#include <random>

class Game {
private:
    std::string target_path_ = "../examples/cpp/word_lists/target.txt";
    std::string guess_path_ = "../examples/cpp/word_lists/guess.txt";
    std::vector<std::string> valid_guess_;

    int score_;
    std::string target_;
public: 
    std::string start();
    std::string generate_word_();
    void store_valid_guesses_();
    std::string parse_guess_(std::string &);
    std::queue<std::string> guess(std::string &);
};

// game.cpp
/* */
std::string Game::start() { 
    target_ = generate_word_();
    score_ = 0;
    store_valid_guesses_();
    return std::string(target_.size(), '_'); 
}

/* */
// Could maybe do this better
std::string Game::generate_word_() { 
    std::fstream in(target_path_);
    std::string word;
    std::vector<std::string> words;

    while (in >> word)
        words.push_back(word);
    if (words.empty()) throw std::runtime_error("Words database is missing");

    // Generate a random int to pick a target word
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<size_t> r(0, words.size()-1);
    
    return words.at(r(gen));
}

/* */
void Game::store_valid_guesses_() {
    std::fstream in(guess_path_);
    std::string word;
    while (in >> word)
        valid_guess_.push_back(word);
}

/* */
std::queue<std::string> Game::guess(std::string &msg) {
    // Stores message for returning to client 
    std::queue<std::string> q;

    // To ensure no double adding to hint when comparing guess to target
    std::unordered_map<char, int> freq_table;
    for (char c : target_)
        freq_table[c]++;

    // Parse client input, return on error
    std::string guess_ = parse_guess_(msg);
    if (guess_ != msg || guess_ == "INVALID GUESS") {
        q.push(guess_);
        return q;
    }

    // Detect correct placement and char
    std::string result(target_.size(), '_');        
    for (int i=0; i < target_.size(); ++i)
        if (target_[i] == guess_[i]) {
            result[i] = target_[i];
            freq_table[target_[i]]--;
        }

    for (int i=0; i < target_.size(); ++i) {
        // Already matched
        if (result[i] != '_') continue;
        // Lowercase match, in word, wrong spot and letter left in target
        if (freq_table[guess_[i]] > 0) {
            result[i] = std::tolower(guess_[i]);
            freq_table[guess_[i]]--;
        }
    }

    // Game over check
    if (result == target_) {
        q.push(std::to_string(++score_));
        q.push("GAME OVER");
        return q;
    }

    score_++;
    q.push(result);
    return q;
}

/* */
std::string Game::parse_guess_(std::string &guess) { 
    for (auto i = 0; i != guess.size(); ++i) 
        guess[i] = toupper(guess[i]);            // cap guess 

    // returns error if word not found in accepted guesses list
    auto it = std::find(valid_guess_.begin(), valid_guess_.end(), guess);
    if (it == valid_guess_.end())
        return "INVALID GUESS";
    return guess;
}

// server_main.cpp 
#include <iostream>
#include <string>
#include <memory>

#include "network.hpp"
#include "server.hpp"

struct Args {
    std::string host;
    std::string port;
    std::string protocol;
    std::string transport;
};

Args parse_args(int, char**);
std::unique_ptr<Server> create_server(const Args &);
std::queue<std::string> handle_client(Message, std::unordered_map<int, Game> &);

int main(int argc, char **argv) {
    Args args = parse_args(argc, argv);
    auto server = create_server(args);
    if (!server) return 1;
    std::unordered_map<int, Game> games;

    std::cout << "Wordle server ready" << '\n';
    while (1) {
        server->tick();
        if (!server->has_message()) continue;
        auto res = server->next();

        // removes client from the games map if they disconnect
        if (res.event == NetEvent::CLIENT_DISCONNECT) {
                games.erase(res.fd);
                std::cout << "Client " << res.fd << " has disconnected.\n";
                continue;
        }

        std::cout << "Client " << res.fd << ": " << res.payload << '\n';
        std::queue<std::string> msg = handle_client(res, games);
        while (!msg.empty()) {
            // empty messge == START GAME failure, kick client
            if (msg.front() == "") {
                std::cout << "Client " << res.fd << " was kicked.\n";
                server->kick(res.fd);
                games.erase(res.fd);
                break;
            }
            // Send server responses back to client
            std::cout << "Server: " << msg.front() << '\n';
            server->send(res.fd, msg.front());
            msg.pop();
        }
    }
return 0;
}

/* Parses cmd line args for the server, allowing user to set port number
 * @params argc - number of args from the cmd line 
 * @params argc - cmd line arguments 
 * @return args - struct containing server settings information */
Args parse_args(int argc, char *argv[]) {
    Args args {"0.0.0.0", "1991", "newline", "tcp"};
    if (argc == 2) args.port = argv[1];

    return args;
}

/* Returns a pointer to a new server on success or a nullptr if host or port 
 * number is busy or incorrect */
std::unique_ptr<Server> create_server(const Args &args) {
    Network n;
    // create a server with the passed in port number and default settings 
    try {
        return std::make_unique<Server>(n.create_server(args.host, args.port, 
                                          args.protocol, args.transport));
    } catch (std::runtime_error &e) {
        std::cerr << "Server failed: " << e.what() << std::endl;
        return nullptr;
    }
}

/* */
std::queue<std::string> handle_client(Message m, std::unordered_map<int, Game> &g) {
    std::queue<std::string> response;
    // game already in progress, process next step;
    if (g.contains(m.fd))
        response = g.at(m.fd).guess(m.payload);
    // New game 
    else {
        if (m.payload != "START GAME") {
            response.push(""); 
            return response; 
        }

        Game game;
        response.push(game.start());
        g[m.fd] = std::move(game);
    }

    if (response.back() == "GAME OVER") { 
        g.erase(m.fd);
    }
    return response;
}
