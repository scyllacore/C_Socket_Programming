#include "client.h"

socket_data* create_socket_data() {
    socket_data *new_socket = malloc(sizeof (socket_data));

    new_socket -> fd = socket(AF_INET, SOCK_STREAM, 0);
    new_socket -> sockaddr = malloc(sizeof (struct sockaddr_in));
    new_socket -> server_hostent = gethostbyname(IP_ADDRESS);
    new_socket -> socklen = sizeof (*new_socket->sockaddr);

    if (new_socket -> server_hostent == NULL) {
        return NULL;
    }

    return new_socket;
}

void initialize_connection_socket(socket_data * socket) {
    bzero(socket ->sockaddr, sizeof (*socket -> sockaddr));
    socket -> sockaddr -> sin_family = AF_INET;
    bcopy((char *) socket -> server_hostent -> h_addr
            , (char *) &socket->sockaddr -> sin_addr.s_addr
            , socket -> server_hostent -> h_length);
    socket -> sockaddr -> sin_port = htons(PORT_NUMBER);
}

int connect_server(socket_data * socket) {
    int connection_result = connect(socket -> fd, (struct sockaddr *) socket->sockaddr, sizeof (*socket->sockaddr));

    if (connection_result < 0) {
        return EXIT_FAILURE;
    }
}

int start_client(socket_data * connection_socket) {
    int received_flag;
    char *buffer = malloc(sizeof (char) * BUFFER_SIZE);

    while (1) {
        printf("What do you want to say? ");
        bzero(buffer, BUFFER_SIZE);
        scanf("%s", buffer);

        received_flag = write(connection_socket -> fd, buffer, strlen(buffer));

        if (received_flag < 0) {
            fprintf(stderr, "ERROR while writing to socket");
            return EXIT_FAILURE;
        }

        if (strcmp(buffer, "quit") == 0) {
            printf("Closing session\n");
            break;
        }


        bzero(buffer, BUFFER_SIZE);
        received_flag = read(connection_socket -> fd, buffer, 255);

        if (received_flag < 0) {
            fprintf(stderr, "ERROR while reading from socket");
            return EXIT_FAILURE;
        }
        printf("server replied: %s \n", buffer);
    }

    return EXIT_SUCCESS;
}

void run_system() {
    socket_data * connection_socket = create_socket_data();
    if (connection_socket == NULL) {
        fprintf(stderr, "ERROR, no such host");
        return;
    }

    initialize_connection_socket(connection_socket);

    if (connect_server(connection_socket) == EXIT_FAILURE) {
        fprintf(stderr, "ERROR while connecting");
        return;
    }

    if (start_client(connection_socket) == EXIT_FAILURE) {
        fprintf(stderr, "ERROR while writing to socket");
        return;
    }

    printf("client finish success");

}

int main(int argc, char *argv[]) {
    run_system();
    return 0;
}