/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/file.h to edit this template
 */

/* 
 * File:   server.h
 * Author: yoon
 *
 * Created on 2023년 1월 15일, 오후 3:49
 */

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include <unistd.h>

#include <string.h>
#include <time.h>

#ifndef PORT_NUMBER
#define PORT_NUMBER 5001
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 256
#endif

#ifndef QUEUE_SIZE
#define QUEUE_SIZE 3
#endif

typedef struct socket_data {
    int fd;
    struct sockaddr_in *sockaddr;
    socklen_t socklen;
} socket_data;

#endif /* SERVER_H */

