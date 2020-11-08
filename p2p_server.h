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
#include <fcntl.h>
#include "utils.h"
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


    server (){  ;}

    server(int port){

        serv_port = port;
        serv_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (serv_socket<0){
            PR("socket error");
        }
        int enable = 1;
        if (setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
            PR("setsockopt(SO_REUSEADDR) failed");
        }
        memset(&serv_addr, 0, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(serv_port);
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(serv_socket, (sockaddr *) &serv_addr, sizeof(serv_addr))==-1){ //bind to the port
            PR("bind error");
        }

        MAXPENDING = 2; //maximum number of client connections handled
        if (listen(serv_socket, MAXPENDING)==-1){
            PR("listen error");
        }

    }

    void accept_connection();

    void send_file(char *);

    int check_correctsend(int , int , int);

    ~server(){
        //close (serv_socket);
    }
    
};

void server::accept_connection(){

    for (int i=0; i<2; i++){
        client_details t1;
        t1.client_addr_len = sizeof(t1.client_addr);

        //int flags = fcntl(serv_socket, F_GETFL, 0);
        //fcntl(serv_socket, F_SETFL, flags | O_NONBLOCK);

        t1.client_sock = accept(serv_socket, (sockaddr *) &t1.client_addr, &t1.client_addr_len);
        if (t1.client_sock<0){
            perror("accept error");
            return;
        }
        //int enable = 1;
        //if (setsockopt(t1.client_sock, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(int)) < 0){
            //PR("setsockopt(TCP_NODELAY) failed");
        //}

        connected_clients.push_back(t1);

        char client_name[INET_ADDRSTRLEN]; //String to store client's name
        if (inet_ntop(AF_INET, &t1.client_addr.sin_addr.s_addr, client_name, t1.client_addr_len)!=NULL){
            cout<<"Handling Client "<<client_name<<" "<<ntohs(t1.client_addr.sin_port)<<endl;
        }
        else{
            cerr<<"Unable to get client address"<<endl;
        }
    }
}

void server::send_file(char *filepath){
    int BUFSIZE = 1024;
    char buffer [BUFSIZE+1];
    int file_size = get_filesize(filepath);

    char file_desc [1024] = "";
    strcpy(file_desc, filepath);
    strcat(file_desc, "|");
    strcat(file_desc, to_string(file_size).c_str());


    for (int i=0; i<connected_clients.size(); i++){


        ssize_t num_bytes_sent = 0;
        num_bytes_sent = send(connected_clients[i].client_sock, file_desc, 1024, 0); 
        if (check_correctsend(num_bytes_sent, 1024, i) == 0) continue;

        FILE *fp = fopen(filepath, "r");
        memset(buffer, 0, sizeof(buffer));
        while (fgets(buffer, BUFSIZE+1, fp)){

            num_bytes_sent = send(connected_clients[i].client_sock, buffer, strlen(buffer), 0);
            cout<<"Sending in progress..."<<endl;
            if (check_correctsend(num_bytes_sent, strlen(buffer), i) == 0) break;

            memset(buffer, 0, sizeof(buffer));
        }
        //close(connected_clients[i].client_sock);
        //for (int j=0; j<1000; j++){
            //memset(buffer, 0, sizeof(buffer));
            //strcpy(buffer, "exit");
            //ssize_t num_bytes_sent = send(connected_clients[i].client_sock, buffer, strlen(buffer), 0);
        //}

    }

    for (int i=0; i<1000; i++) //DEBUGGING PURPOSES
    cout<<"Send finished"<<endl;

}

int server::check_correctsend(int num_bytes_sent, int buffer_len, int index){
    
    if (num_bytes_sent<0){
        perror("send error");
        connected_clients.erase(connected_clients.begin() + index);
        return 0;
    }
    else if (num_bytes_sent != buffer_len){
        cerr<<"send() sent incorrect number of bytes"<<endl;
        exit(1);
    }

    return 1;
}
//int main(){

    //server s1 (12345);

    //while (1) {
        //s1.accept_connection();
        //char filepath [] = "books/50.txt";
        //s1.send_file(filepath);
    //} 
    //return 0;
//}