#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;

    struct hostent *server;

    char buffer[256];
    portno = 5001;

    // create socket and get file descriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server = gethostbyname("127.0.0.1"); //클라이언트는 내가 연결해야할 서버 정보를 알고 있어야 한다.
    // IP주소를 통해 서버 정보 구조체를 생성하는 함수.

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);


    // connect to server with server address which is set above (serv_addr)

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        printf("ERROR while connecting"); //perror
        exit(1);
    }

    // inside this while loop, implement communicating with read/write or send/recv function
    while (1) {
        printf("What do you want to say? ");
        bzero(buffer, 256);
        scanf("%s", buffer);
        
        n = write(sockfd, buffer, strlen(buffer)); //server에서는 socket을 2개 생성하지만 client에서는 1개만 생성한다.

        if (strcmp(buffer, "close") == 0) { // quit을 close로 설정할 시 이 루프는 종료된다. 하지만 서버에서 무한루프 문제가 발생한다. 해결 -> quit도 가능하다.
            break;
        }

        if (n < 0) {
            printf("ERROR while writing to socket");
            exit(1);
        }

        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);

        if (n < 0) {
            printf("ERROR while reading from socket");
            exit(1);
        }
        printf("server replied: %s \n", buffer);

        // escape this loop, if the server sends message "quit" -> 위치가 잘못되었다. 무한 루프 오류

    }
    return 0;
}