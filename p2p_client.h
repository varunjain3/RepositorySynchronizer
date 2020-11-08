#ifndef p2p_client 
#define p2p_client

#include <fstream>
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
#include <chrono>
#define PR(s) {\
        perror((s));\
        exit(EXIT_FAILURE); }
using namespace std;

class client{

private:

    char *serv_ip;
    in_port_t serv_port;
    sockaddr_in serv_addr;
    int sock;

public:

    client (){  ;}

    client(char *ip, int port){
        serv_port = port;
        serv_ip = ip;
        sock = socket(AF_INET, SOCK_STREAM, 0); // 0 defaults to TCP using SOCK_STREAM
        if (sock<0)
            PR("socket failed");

        memset(&serv_addr, 0, sizeof(serv_addr)); // Initialising the sockaddr_in with 0

        serv_addr.sin_family = AF_INET; //IPv4
        serv_addr.sin_port = htons(serv_port);

        int rv = inet_pton(AF_INET, serv_ip, &serv_addr.sin_addr);
        if (rv==0){
            cerr<<"Invalid sin_addr"<<endl; 
            exit(1);
        }
        else if (rv<0) PR("pton failed");

    }

    void connect2server();

    void receive_data(char *);

    ~client(){ ; }

};

void client::connect2server(){
    while (1){
        int t1 = connect(sock, (sockaddr *) &serv_addr, sizeof(serv_addr));
        if (t1<0){
            perror("connect error");
            sleep(1);
        }
        else break;
    }
}

void client::receive_data(char * save_folder){
    //connect2server();

    int BUFSIZE = 1024, num_bytes = 0, FileSize = 0;
    char *filepath, file_desc[1024]; 

    num_bytes = recv(sock, file_desc, 1024, 0); 
    if (num_bytes<0){   perror ("recv error");  return; }
    
    cout<<file_desc<<endl;
    filepath = strtok(file_desc, "|");
    FileSize = atoi (strtok(NULL, "|"));
    
    char filepath_temp [1024];
    strcpy(filepath_temp, save_folder);
    strcat(filepath_temp, filepath);
    filepath = filepath_temp;

    unsigned int rec_len = 0; 
    FILE *fp = fopen(filepath, "w");
    int ct = 0; 
    do{
        cout<<rec_len<<endl;
        char buffer[BUFSIZE+1];
        num_bytes = recv(sock, buffer, BUFSIZE, 0); //receives data from the transport layer
        if (num_bytes<0){   perror ("recv error");  return; }

        buffer[num_bytes] = '\0';

        rec_len += num_bytes;
        fputs (buffer, fp);
        ct++;
        //if (ct>100) break;

    }   while (rec_len<FileSize);

    for (int i=0; i<1000; i++)
    cout<<"received_finished"<<endl;
}


//int main(){

    //client c1("127.0.0.1", 12345);
    //c1.connect2server();
    //char filepath[] = "received/50.txt";
    //c1.receive_data(filepath);

    //return 0;
//}

#endif