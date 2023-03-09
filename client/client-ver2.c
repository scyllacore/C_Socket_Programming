#include "client.h"

socket_data* create_socket_data() {
    socket_data *new_socket = malloc(sizeof (socket_data));

    new_socket -> fd = socket(AF_INET, SOCK_STREAM, 0);
    new_socket -> sockaddr = malloc(sizeof (struct sockaddr_in));
    new_socket -> server_hostent = gethostbyname(IP_ADDRESS);
    new_socket -> socklen = sizeof (*new_socket->sockaddr);

    if (new_socket -> server_hostent == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
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

void connect_server(socket_data * socket) {
    int connection_result = connect(socket -> fd, (struct sockaddr *) socket->sockaddr, sizeof (*socket->sockaddr));

    if (connection_result < 0) {
        fprintf(stderr, "ERROR while connecting");
        exit(1);
    }
}

void start_client(socket_data * connection_socket) {
    int flag;
    char *buffer = malloc(sizeof (char) * BUFFER_SIZE);

    while (1) {
        printf("What do you want to say? ");
        bzero(buffer, BUFFER_SIZE);
        scanf("%s", buffer);

        flag = write(connection_socket -> fd, buffer, strlen(buffer));

        if (strcmp(buffer, "quit") == 0) {
            printf("Closing session\n");
            break;
        }

        if (flag < 0) {
            fprintf(stderr, "ERROR while writing to socket");
            exit(1);
        }

        bzero(buffer, BUFFER_SIZE);
        flag = read(connection_socket -> fd, buffer, 255);

        if (flag < 0) {
            fprintf(stderr, "ERROR while reading from socket");
            exit(1);
        }
        printf("server replied: %s \n", buffer);
        
        for(int i=0; i<strlen(buffer); i++){
            printf("%d ",buffer[i]);
        }
        printf("\n");
        
    }
}

int main(int argc, char *argv[]) {
    socket_data * connection_socket = create_socket_data();
    initialize_connection_socket(connection_socket);
    connect_server(connection_socket);
    start_client(connection_socket);

    return (EXIT_SUCCESS);
}