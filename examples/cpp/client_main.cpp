#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include "network.hpp"
#include "client.hpp"

std::string get_guess_(std::vector<std::string> &);

int main(void) {
    std::string host = "127.0.0.1";
    std::string port = "1991";
    std::string protocol = "newline";
    std::string transport = "tcp";
    std::string guess_path_ = "../examples/cpp/word_lists/guess.txt";
    std::vector<std::string> valid_word_list_;
    Network n;
    // set up 
    Client client = n.create_client(host, port, protocol, transport);

    // while (!client.is_ready())
    //     continue;

    client.send("START GAME");
    std::ifstream in(guess_path_);
    std::string word;
    while (in >> word) {
        valid_word_list_.push_back(word);
    }

    // Game loop
    while (1) {
        // if (!client.is_ready()) continue;
        client.tick();

        if(client.has_message()) {
            auto message = client.next();
            // TODO: Return enum or something better on server crash, also warn user
            if (message.payload == "Server disconnected") {
                std::cout << "Server has dropped the connection, try again later" << std::endl;
                break;
            }

            // Try again 
            if (message.payload == "INVALID GUESS") {
                std::cout << "Invalud input, 5 letter words only" << '\n';
                std::string guess = get_guess_(valid_word_list_);
                client.send(guess);
            }
            // Error, disconnect
            else if (message.payload == "INVALID WORD") {
                std::cout << "Fatal error, disconnecting";
                break;
            }
                // Inform of error, break, shutdown
            else if (message.payload == "GAME OVER") {
                std::cout << message.payload << std::endl;
                break;
            }
            else if (isdigit(message.payload[0])) {
                std::cout << message.payload << '\n';
            }
            else {
                std::cout << message.payload << "\n";
                std::string guess = get_guess_(valid_word_list_);
                client.send(guess);
            }
        }
    }

    return 0;
}

std::string get_guess_(std::vector<std::string> &list) {
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
