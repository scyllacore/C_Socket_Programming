#include "server.h"

#include <malloc.h>

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
    socket_data * client = create_socket_data();
    client -> fd = accept(server -> fd, (struct sockaddr *) &client -> sockaddr, &client -> socklen);

    return client;
}

char* create_buffer(size_t length) {
    return malloc(length * sizeof (char));
}

char* process_received_message(char *input) {
    size_t length = sizeof (char) * strlen(input);
    printf("%d len \n",length);
    char* output = malloc(length);
    printf("%d size \n",malloc_usable_size(output)); // sizeof(*output) malloc이 정확히 설정한 사이즈만큼의 공간을 할당하지 않는 듯하다. 여유분을 추가해서 할당하는 것 같다.

    memcpy(output, input, length);
    output[length] = '\0'; // 이것을 넣어서 해결
    
    for(int i=0; i<strlen(output); i++){
            printf("%d ",output[i]);
        }
        printf("\n");
    
    return output;
}

void process_data(socket_data* client) {
    char *response = NULL;
    char *buffer = create_buffer(BUFFER_SIZE);

    for (;;) {
        recv(client -> fd, buffer, BUFFER_SIZE, 0); //  close를 받을시 클라이언트만 종료되고 서버는 계속 동작한다.

        if (strcmp(buffer, "quit") == 0 || strlen(buffer) == 0) { //버퍼에 아무 데이터가 없을 때도 recv를 받아 처리하여 오류가 났다. strlen이 0일 때 멈추도록 조건을 추가하여 해결. 
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