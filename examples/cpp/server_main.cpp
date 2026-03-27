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
    return std::string(target_.size(), '_'); // hardcoded for the moment
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

    // TODO: Bug hiding in wait, is user types INVALID GUESS, client will crash.
    // Parse client input, return on error
    std::string guess_ = parse_guess_(msg);
    if (guess_ != msg) {
        q.push(guess_);
        return q;
    }

    // Detect correct placement and char
    std::string result(target_.size(), '_');        
    for (int i=0; i < target_.size(); ++i)
        if (target_[i] == toupper(guess_[i])) {
            result[i] = target_[i];
            freq_table[target_[i]]--;
        }

    // TODO: I already have converted to upper case prior to this, check and remove
    for (int i=0; i < target_.size(); ++i) {
        // Already matched
        if (result[i] != '_') continue;
        // Lowercase match, in word, wrong spot and letter left in target
        if (freq_table[toupper(guess_[i])] > 0) {
            result[i] = tolower(guess_[i]);
            freq_table[toupper(guess_[i])]--;
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
    for (auto i = 0; i != guess.size(); ++i) {
        // Cap it while we are here to avoid case mismatch checks in guess()
        guess[i] = toupper(guess[i]);
    }

    // Client doesnt drop server, tries again
    auto it = std::find(valid_guess_.begin(), valid_guess_.end(), guess);
    if (it == valid_guess_.end())
        return "INVALID GUESS";
    return guess;
}

// server_main.cpp 
#include <iostream>
#include <string>
#include <map>

#include "network.hpp"
#include "server.hpp"

std::queue<std::string> handle_client(Message, std::unordered_map<int, Game> &);

int main(void) {
    std::string host = "127.0.0.1";
    std::string port = "1991";
    std::string protocol = "newline";
    std::string transport = "tcp";
    Network network;
    Server server = network.create_server(host, port, protocol, transport);
    std::unordered_map<int, Game> games;
    std::cout << "Wordle server ready" << '\n';
    
    while (1) {
        server.tick();

        if (!server.has_message()) continue;

        auto msg = server.next();

        switch(msg.event) {
            case NetEvent::CLIENT_DISCONNECT:
                games.erase(msg.fd);
                std::cout << "Client " << msg.fd << " Has disconnected.\n";
                break;
            case NetEvent::DATA:{
                std::cout << "Client " << msg.fd << ": " << msg.payload << '\n';
                std::queue<std::string> response = handle_client(msg, games);
                while (!response.empty()) {
                    // Bugs galore here? remove fd, pop messages etc?
                    // Explore why im returning ""?
                    if (response.front() == "") {
                        std::cout << "Client " << msg.fd << " was kicked.\n";
                        server.kick(msg.fd);
                        break;
                    }
                    std::cout << "Server: " << response.front() << '\n';
                    server.send(msg.fd, response.front());
                    response.pop();
                }
                break;
                                }
            default: break;
        }
    }
return 0;
}

/* */
std::queue<std::string> handle_client(Message m, std::unordered_map<int, Game> &g) {
    std::queue<std::string> response;
    if (g.contains(m.fd))
        // process next step of game;
        response = g.at(m.fd).guess(m.payload);
    else {
        // TODO: implllment an enum for all these string vals 
        if (m.payload != "START GAME") {
            response.push(""); 
            return response; 
        }

        Game game;
        response.push(game.start());
        g[m.fd] = std::move(game);
    }

    // Make enum for GAME_OVER, GAME_START, etc
    if (response.back() == "GAME OVER") { 
        g.erase(m.fd);
    }
    return response;
}

