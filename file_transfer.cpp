#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "p2p_client.h"
#include "p2p_server.h"
#define PR(s) {\
        perror((s));\
        exit(EXIT_FAILURE); }

int serv_port, client_port;
class p2p{

    server s1;
    client c1; 
    char filepath1[1000];
    char filepath2[1000];

public:

    p2p(){ ; };

    p2p(int serv_port, int client_port){
        s1 = server(serv_port);
        c1 = client((char *)"127.0.0.1", client_port);
    }

    void file_to_transfer(char *file1, char *file2){
        strcpy(filepath1, "books/");
        strcat(filepath1, file1);
        strcpy(filepath2, "received/");
        strcat(filepath2, file2);
    }

    void start(){
        c1.connect2server();
        s1.accept_connection();
        s1.send_file(filepath1);
        c1.receive_data(filepath2);
    } 

};


int main(){
    cin>>serv_port>>client_port;
    p2p p1 (serv_port, client_port);
    char file1 [1000], file2[1000];
    cin>>file1>>file2;
    p1.file_to_transfer(file1, file2);
    p1.start();
    return 0;
}