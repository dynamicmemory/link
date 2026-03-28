#include <iostream>
#include <string>
#include <array>
#include "network.hpp"

struct Args {
    std::string host;
    std::string port;
    std::string protocol;
    std::string transport;
};

Args parse_args(int, char **);
std::unique_ptr<Client> connect_to_server(const Args &);
void handle_response(Client *, const std::string);
std::string get_guess();
bool is_valid_hint(const std::string &);
bool check_hint(const std::string &, const std::string &);


int main(int argc, char **argv) {
    Args args = parse_args(argc, argv); 
    auto client = connect_to_server(args);
    if (!client) return 1;
    std::string last_guess(5,'_');

    while (!client->is_ready()) continue; // Only needed for tls not tcp only
    client->send("START GAME");

    // Game loop
    while (1) {
        client->tick();
        if(!client->has_message()) continue;
        Message response = client->next();

        // Exit on server disconnect
        if (response.event == NetEvent::SERVER_DISCONNECT) {
            std::cout << "Server has disconnected, exiting... " << std::endl;
            break;
        }

        std::string msg = response.payload;
        // user sent invalid word 
        if (msg == "INVALID GUESS") {
            std::cout << "Invalid input, 5 letter words only" << '\n';
            client->send(get_guess());
        }
        // only send response and set last_guess if server hint was valid 
        else if (is_valid_hint(msg)) {
            if (!check_hint(msg, last_guess)) {
                std::cout << "Invalid server command, exiting...\n";
                return 1;
            }

            std::cout << "Current hint: " << response.payload << '\n';
            last_guess = get_guess();
            client->send(last_guess);
        }
        // Score
        else if (!msg.empty() && std::isdigit(msg[0])) {
            std::cout << "You're score was: " << msg << '\n';
        }
        // End game
        else if (msg == "GAME OVER") {
            return 0;
        }
        // Server sent unrecognizable command
        else {
            std::cout << "Invalid server command, exiting...\n";
            return 1;
        }
    }
    return 0;
}

/* Parses cmd line args for client, allowing user to set host and port number
 * @params argc - number of args from the cmd line 
 * @params argc - cmd line arguments 
 * @return args - struct containing server settings information */
Args parse_args(int argc, char *argv[]) {
    Args args {"127.0.0.1", "1991", "newline", "tcp"};
    if (argc == 2) args.host = argv[1];
    if (argc == 3) args.port = argv[2];

    return args;
}

/* */
std::unique_ptr<Client> connect_to_server(const Args &args) {
    Network n;
    // Connect to default or provided host and port  
    try {
        return std::make_unique<Client>(n.create_client(args.host, args.port, 
                                           args.protocol, args.transport));
    } catch (std::runtime_error &e) {
        std::cerr << "Connection failed: " << e.what() << std::endl;
        return nullptr;
    }
}

/**/
std::string get_guess() {
    std::string word;
    std::cout << "Enter a 5 letter word: ";
    std::cin >> word;
    return word;
}

/* */
bool is_valid_hint(const std::string &msg) {
    if (msg.size() != 5) return false;
    for (char c : msg)
        if (!(c == '_' || std::isalpha(c)))
            return false;
    return true;
}

/**/
bool check_hint(const std::string &hint, const std::string &prev) {
    if (hint == "_____") return true;
    std::array<int, 26> hint_freq{};
    std::array<int, 26> prev_freq{};

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
