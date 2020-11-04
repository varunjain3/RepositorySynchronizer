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
    server s1 = server(serv_port);
    client c1 = client("127.0.0.1", client_port);
    server s2;
public:
    void start(){
        c1.connect2server();
        s1.accept_connection();
        char filepath1[] = "books/50.txt";
        char filepath2[] = "received/50.txt";
        s1.send_file(filepath1);
        c1.receive_data(filepath2);
    } 
};


int main(){
    cin>>serv_port>>client_port;
    p2p p1(1000);
    p1.start();
    return 0;
}