#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include "network.hpp"
#include "client.hpp"

std::string get_valid_guess_(std::vector<std::string> &);
std::string get_guess_();
bool check_hint_(std::string &, std::string &);

int main(void) {
    std::string host = "127.0.0.1";
    std::string port = "1991";
    std::string protocol = "newline";
    std::string transport = "tcp";
    std::string last_guess_(5,'_');
    // std::string guess_path_ = "../examples/cpp/word_lists/guess.txt";
    // std::vector<std::string> valid_word_list_;
    Network n;
    // set up 
    Client client = n.create_client(host, port, protocol, transport);

    // TODO: COULD CAUSE DEATH, NOT TESTED YET
    while (!client.is_ready())
        continue;

    client.send("START GAME");
    // std::ifstream in(guess_path_);
    // std::string word;
    // while (in >> word) {
    //     valid_word_list_.push_back(word);
    // }

    // Game loop
    while (1) {
        // if (!client.is_ready()) continue;
        client.tick();

        if(!client.has_message()) continue;

        auto msg = client.next();
        std::string res = msg.payload;
        // TODO: Return enum or something better on server crash, also warn user
        // Also this would fall into the else below, so may not need this.
        if (res == "Server disconnected") {
            std::cout << "Server has disconnected... " << std::endl;
            break;
        }

        // Client sent invalid word 
        if (res == "INVALID GUESS") {
            std::cout << "Invalid input, 5 letter words only" << '\n';
            client.send(get_guess_());
        }
        // Normal guess 
        else if (res.size() == 5 && check_hint_(res, last_guess_)) {
            std::cout << "Current hint: " << msg.payload << '\n';
            std::string guess = get_guess_();
            last_guess_ = guess;
            client.send(guess);
        }
        // Score
        else if (isdigit(res[0])) {
            std::cout << "You're score was: " << res << '\n';
        }
        // End game
        else if (res == "GAME OVER") {
            break;
        }
        // Server sent unrecognizable command
        else {
            std::cout << "Fatal error, disconnecting\n";
            break;
        }
    }
    return 0;
}

/**/
std::string get_guess_() {
    std::string word;
    std::cout << "Enter a 5 letter word: ";
    std::cin >> word;
    return word;
}

// TODO: This needs a modern facelift
bool check_hint_(std::string &hint, std::string &prev) {
    if (hint == "_____") return true;
    for (int i=0; i < hint.size(); ++i) { 
        if (hint[i] == '_') continue;
        for (int j=0; j < prev.size(); ++j)
            if (toupper(hint[i]) == toupper(prev[j]))
                return true;
    }
    return false;
}

/*For client input validation, not needed yet*/
std::string get_valid_guess_(std::vector<std::string> &list) {
    std::string word;
    while (1) {
        std::cout << "Enter guess, 5 letters, balh:";
        std::cin >> word;
        // TODO: Use iterators
        for (int i=0; i<word.size(); ++i) word[i] = toupper(word[i]);
        // word.pop_back();
        auto search = std::find(list.begin(), list.end(), word);
        if (search != list.end()) {
            return word;
        }
        else 
            std::cout << "Not a valid word\n";
    }
}
