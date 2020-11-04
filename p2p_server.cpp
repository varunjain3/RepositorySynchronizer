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
#define PR(s) {\
        perror((s));\
        exit(EXIT_FAILURE); }
using namespace std;


struct client_details{
    int client_sock;
    sockaddr_in client_addr;
    socklen_t client_addr_len;
};


class server{

private:
    in_port_t serv_port;
    int serv_socket;
    sockaddr_in serv_addr;
    int MAXPENDING;
    vector <client_details> connected_clients;

public:

    void send_file(char *);

    server(serv_port){
        serv_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (serv_socket<0){
            PR("socket error");
        }
        memset(&serv_addr, 0, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(serv_port);
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(serv_socket, (sockaddr *) &serv_addr, sizeof(serv_addr))==-1){ //bind to the port
            PR("bind error");
        }

        MAXPENDING = 5; //maximum number of client connections handled
        if (listen(serv_socket, MAXPENDING)==-1){
            PR("listen error");
        }
    }
    void accept_connection(){

        client_details t1;
        t1.client_addr_len = sizeof(t1.client_addr);
        t1.client_sock = accept(serv_socket, (sockaddr *) &t1.client_addr, &t1.client_addr_len);
        if (t1.client_sock<0){
            PR("accept error");
        }
        connected_clients.push_back(t1);

    }

    ~server(){
        close (serv_socket);
    }
    
};

void server::send_file(char *filepath){
    int BUFSIZE = 1024;
    char buffer [BUFSIZE+1];
    for (int i=0; i<connected_clients.size(); i++){

        FILE *fp = fopen(filepath, "r");
        memset(filepath, 0, sizeof(filepath));
        while (fgets(buffer, BUFSIZE+1, fp)){
            ssize_t num_bytes_sent = send(connected_clients[i].client_sock, buffer, strlen(buffer), 0);
            if (num_bytes_sent<0){
                PR("send error");
            }
            else if (num_bytes_sent!=strlen(buffer));{
                cerr<<"send() sent incorrect number of bytes"<<endl;
                exit(1);
            }
            memset(filepath, 0, sizeof(filepath));
        }

    }

}
