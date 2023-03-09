#include "server.h"

socket_data* create_socket_data() {
    socket_data *new_socket = malloc(sizeof (socket_data));

    new_socket -> fd = 0;
    new_socket -> sockaddr = malloc(sizeof (struct sockaddr_in));
    new_socket -> socklen = sizeof (*new_socket->sockaddr);

    return new_socket;
}

void initialize_server_sockaddr_in(socket_data* new_socket) {
    new_socket -> sockaddr -> sin_family = AF_INET;
    new_socket -> sockaddr -> sin_addr.s_addr = htonl(INADDR_ANY);
    new_socket -> sockaddr -> sin_port = htons(PORT_NUMBER);
}

socket_data* run_server() {
    socket_data * server = create_socket_data();
    server -> fd = socket(AF_INET, SOCK_STREAM, 0);
    initialize_server_sockaddr_in(server);

    if (bind(server -> fd, (const struct sockaddr *) server -> sockaddr, server -> socklen) < 0) {
        fprintf(stderr, "Error! Bind has failed\n");
        exit(0);
    }

    if (listen(server -> fd, QUEUE_SIZE) < 0) {
        fprintf(stderr, "Error! Can't listen\n");
        exit(0);
    }

    return server;
}

socket_data* accept_client(socket_data * server) {
    socket_data* client = create_socket_data();

    char buffer[256];

    //inet_ntop(AF_INET, &server->sockaddr->sin_addr, buffer, sizeof (buffer)); 이건 됨.
    //inet_ntoa(client->sockaddr->sin_addr) 오류가 나는 이유는 정확히 분석하지 못했다.
    printf("%s\n", inet_ntoa(client->sockaddr->sin_addr));
    //printf("%s\n", buffer);

    client -> fd = accept(server -> fd, (struct sockaddr *) &client -> sockaddr, &client -> socklen);

    struct sockaddr_in address;
    socklen_t address_size = sizeof (address);

    int res = getpeername(client -> fd, (struct sockaddr *) &address, &address_size); // 직접 접근이 불가능하여 이 함수를 써야한다.
    printf("%s\n", inet_ntoa(address.sin_addr));


    //printf("%d", client->sockaddr->sin_addr);


    //printf("%s", inet_ntoa(client->sockaddr->sin_addr));


    printf("client fd : %d\n", client -> fd);

    return client;
}

char* create_buffer(size_t length) {
    return malloc(length * sizeof (char));
}

char* process_received_message(char *input) {
    size_t length = sizeof (char) * strlen(input);
    char* output = malloc(length);
    memcpy(output, input, length);
    output[length] = '\0';
    return output;
}

void process_data(socket_data* client) {
    char *response = NULL;
    char *buffer = create_buffer(BUFFER_SIZE);

    for (;;) {
        recv(client -> fd, buffer, BUFFER_SIZE, 0);

        if (strcmp(buffer, "close") == 0) {
            exit(0);
        }

        if (strcmp(buffer, "quit") == 0 || strlen(buffer) == 0) {
            printf("Process %d: ", getpid());
            close(client -> fd);
            printf("Closing session with `%d`. Bye!\n", client->fd);
            break;
        }

        printf("Process %d: ", getpid());
        printf("Received `%s`. Processing... ", buffer);

        free(response);
        response = process_received_message(buffer);
        bzero(buffer, sizeof (char) * BUFFER_SIZE);

        send(client -> fd, response, strlen(response), 0);
        printf("Responded with `%s`. Waiting for a new query...\n", response);
    }
}

void fork_process(socket_data* server, socket_data* client) {
    __pid_t pid = -1;
    pid = fork();

    if (pid == 0) {
        close(server->fd);

        if (client->fd == -1) {
            exit(0);
        }

        printf("Connection with `%d` has been established and delegated to the process %d.\nWaiting for a query...\n", client->fd, getpid());
        process_data(client);
        exit(0);
    } else {
        close(client->fd);
    }
}

void start_communication(socket_data * server) {
    for (;;) {
        socket_data* client = accept_client(server);
        fork_process(server, client);
    }
}

int main(int argc, char *argv[]) {
    socket_data * server = run_server();
    start_communication(server);

    return (EXIT_SUCCESS);
}

//특정 ip구간 검사하는 방법 고안해볼 것. 0.0.0.0을 검사했을시 모든 요청을 거부하는 코드 작성해볼 것.