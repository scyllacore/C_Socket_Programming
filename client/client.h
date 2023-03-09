/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/file.h to edit this template
 */

/* 
 * File:   client.h
 * Author: yoon
 *
 * Created on 2023년 1월 17일, 오후 12:33
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#ifndef PORT_NUMBER
#define PORT_NUMBER 5001
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 256
#endif

#ifndef IP_ADDRESS
#define IP_ADDRESS "127.0.0.1"
#endif

typedef struct socket_data {
    int fd;
    struct sockaddr_in *sockaddr;
    struct hostent *server_hostent;
    socklen_t socklen;
} socket_data;

#endif /* CLIENT_H */

