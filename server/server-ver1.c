#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <sys/socket.h>

/*void bzero(void *a, size_t n) {
    memset(a, 0, n);
}

void bcopy(const void *src, void *dest, size_t n) {
    memmove(dest, src, n);
}*/

struct sockaddr_in* init_sockaddr_in(uint16_t port_number) {
    struct sockaddr_in *socket_address = malloc(sizeof (struct sockaddr_in));
    memset(socket_address, 0, sizeof (*socket_address)); // 초기화의 이유알아 낼 것,
    // 데이터가 포인터 형태로 선언됐다면 해당 데이터에 직접 접근할 때는 *(참조 연산자)를 사용해야한다.
    socket_address -> sin_family = AF_INET; // Ipv4
    socket_address -> sin_addr.s_addr = htonl(INADDR_ANY); // host to network long, https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=lovinghc&logNo=30031089847
    socket_address -> sin_port = htons(port_number);
    return socket_address;
}

char* process_operation(char *input) {
    size_t n = strlen(input) * sizeof (char);
    char *output = malloc(n);
    memcpy(output, input, n);
    return output;
}

int main(int argc, char *argv[]) {


    printf("%d\n", sizeof (struct sockaddr_in));

    const uint16_t port_number = 5001; //포트 번호 지정
    int server_fd = socket(AF_INET, SOCK_STREAM, 0); // 소켓을 생성하고 file discription(구분자)을 반환.
    //AF_INET은 ipv4,즉 ip 체계를. SOCK_STREAM은 TCP 방식의 통신을 함을 의미, 0은 앞 두개의 인자값을 통해 TCP통신을 하는 것을 알 수 있음으로 따로 설정을 안해도 되어 0.

    struct sockaddr_in *server_sockaddr = init_sockaddr_in(port_number); // sockaddr_in은 socket에 필요한 정보(ip유형,ip주소,포트 번호,)를 담는 크기가 16byte인 구조체이다.
    struct sockaddr_in *client_sockaddr = malloc(sizeof (struct sockaddr_in)); // client 정보를 담을 소켓정보 구조체도 생성한다.



    socklen_t server_socklen = sizeof (*server_sockaddr);
    printf("%u\n", server_socklen);
    socklen_t client_socklen = sizeof (*client_sockaddr);


    if (bind(server_fd, (const struct sockaddr *) server_sockaddr, server_socklen) < 0)// 타입 변환이 일어나는 이유 알아 낼 것. 두번째 인자에는 해당 데이터가 있는 주소가 들어가야된다.
        // bind는 묶다라는 뜻으로 이 함수는 위에서 만든 서버 소켓에 방금 설정한 server_sockaddr을 할당하여 소켓의 주소를 정해주는 역할을 함.
    {
        printf("Error! Bind has failed\n");
        exit(0);
    }
    if (listen(server_fd, 3) < 0) // 이 함수가 호출되었을 때 client가 connect를 호출하여 대기 상태를 가질 수 있다. 두번 째 인자는 최대 클라이언트 대기 수이고 Queue의 방식으로 처리된다.
    {
        printf("Error! Can't listen\n");
        exit(0);
    }


    const size_t buffer_len = 256; // unsigned int
    char *buffer = malloc(buffer_len * sizeof (char));
    char *response = NULL;
    time_t last_operation; // long int (정수형 인데 8byte사용)
    __pid_t pid = -1; // int

    while (1) {
        int client_fd = accept(server_fd, (struct sockaddr *) &client_sockaddr, &client_socklen); // 두번째 소켓이 생성된다. 직접적인 송수신을 담당한다.
        // clnt_sock에 새로운 소켓 디스크립터를 반환할 때에는 대기 큐에서 첫번째로 대기중인 연결요청을 참조한다. 이 때, 대기 큐가 비어있는 상황이라면 새로운 요청이 올 때까지 accept값은 반환되지 않고 대기(blocking)한다.

        pid = fork(); //새로운 프로세서 생성

        if (pid == 0) { // 자식 프로세스라면
            close(server_fd); //listen, 즉 두번째 소켓은 송수신만 담당함으로 fork후 생성된 새로운 프로세스에서는 연결을 위해 썻던 소켓은 제거하고 송수신 소켓만 남김.

            if (client_fd == -1) { // client를 정상적으로 연결하지 못했다면 -1
                exit(0);
            }

            printf("Connection with `%d` has been established and delegated to the process %d.\nWaiting for a query...\n", client_fd, getpid()); // pid id

            last_operation = clock();



            while (1) {
                //read(client_fd, buffer, buffer_len);
                recv(client_fd, buffer, buffer_len,0);


                if (strcmp(buffer, "close") == 0) { // buffer == "close"
                    printf("Process %d: ", getpid());
                    close(client_fd);
                    printf("Closing session with `%d`. Bye!\n", client_fd);
                    break;
                }

                if (strlen(buffer) == 0) { // 무슨 역할인지 알아낼 것.
                    clock_t d = clock() - last_operation; // clock() 대신  time쓸 것 생각할 것.
                    double dif = 1.0 * d / CLOCKS_PER_SEC;

                    if (dif > 5.0) {
                        printf("Process %d: ", getpid());
                        close(client_fd);
                        printf("Connection timed out after %.3lf seconds. ", dif);
                        printf("Closing session with `%d`. Bye!\n", client_fd);
                        break;
                    }

                    continue;
                }

                printf("Process %d: ", getpid());
                printf("Received `%s`. Processing... ", buffer);

                free(response);
                response = process_operation(buffer);
                bzero(buffer, buffer_len * sizeof (char));

                send(client_fd, response, strlen(response), 0);
                printf("Responded with `%s`. Waiting for a new query...\n", response);

                last_operation = clock();
            }
            exit(0);
        } else {
            close(client_fd); // connection, 서버 부모 프로세스의 역할은 데이터 송수신이 아닌 connection임. 따라서 클라이언트와의 연결을 담당해주는 첫번째 소켓만 필요함으로 두번째 소켓 없앰.
        }
    }
}