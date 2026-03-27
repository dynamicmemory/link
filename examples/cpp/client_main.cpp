#include <iostream>
#include <string>
#include <array>
#include "network.hpp"

struct Args {
    std::string host;
    std::string port;
};

Args parse_args(int, char **);
std::string get_guess();
bool check_hint(const std::string &, const std::string &);

// TODO: Check networking code for throw or err return when host or port is wrong
int main(int argc, char **argv) {
    std::string protocol = "newline";
    std::string transport = "tcp";
    std::string last_guess(5,'_');
    Args args = parse_args(argc, argv); 
    Network n;

    // set up 
    Client client = n.create_client(args.host, args.port, protocol, transport);
    while (!client.is_ready()) continue;
    client.send("START GAME");

    // Game loop
    while (1) {
        client.tick();
        if(!client.has_message()) continue;
        Message response = client.next();

        // Exit on server disconnect
        if (response.event == NetEvent::SERVER_DISCONNECT) {
            std::cout << "Server has disconnected, exiting... " << std::endl;
            break;
        }
        std::string msg = response.payload;

        // user sent invalid word 
        if (msg == "INVALID GUESS") {
            std::cout << "Invalid input, 5 letter words only" << '\n';
            client.send(get_guess());
        }
        // Normal guess 
        else if (msg.size() == 5 && check_hint(msg, last_guess)) {
            std::cout << "Current hint: " << response.payload << '\n';
            last_guess = get_guess();
            client.send(last_guess);
        }
        // Score
        else if (!msg.empty() && std::isdigit(msg[0])) {
            std::cout << "You're score was: " << msg << '\n';
        }
        // End game
        else if (msg == "GAME OVER") {
            break;
        }
        // Server sent unrecognizable command
        else {
            std::cout << "Ivalid server command, exiting...\n";
            break;
        }
    }
    return 0;
}

/**/
Args parse_args(int argc, char *argv[]) {
    Args args {"127.0.0.1", "1991"};
    if (argc == 1) args.host = argv[1];
    if (argc == 2) args.port = argv[2];

    return args;
}

/**/
std::string get_guess() {
    std::string word;
    std::cout << "Enter a 5 letter word: ";
    std::cin >> word;
    return word;
}

/**/
bool check_hint(const std::string &hint, const std::string &prev) {
    if (hint == "_____") return true;
    std::array<int, 26> hint_freq;
    std::array<int, 26> prev_freq;

    // Populate both frequency tables 
    for (auto c : hint) {
        if (c == '_') continue;
        hint_freq[std::toupper(c) - 'A']++;
    }
    for (auto c : prev) 
        prev_freq[std::toupper(c) - 'A']++;
    // compare that hint is contained in prev
    for (int i=0; i < 26; ++i)
        if (hint_freq[i] > prev_freq[i])
            return false;

    return true;
}
