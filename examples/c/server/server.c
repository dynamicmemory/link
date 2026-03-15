#include "link_c_api.h"
#include <stdio.h>

int main(void) {
    NetworkServer *server = server_create("0.0.0.0", 
            "1991", "default", "tls", "select"); 

    printf("Server Created. Running...");
    while (1) {
        server_tick(server, 0);

        if (server_has_message(server)) {
            int fd;
            const char *message = server_next(server, &fd);
            printf("%s\n", message);
            server_send(server, fd, "I received your message, im in C\n");
        }
    }
    return 0;
}

