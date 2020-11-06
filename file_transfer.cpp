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
#include <thread>
#define PR(s) {\
        perror((s));\
        exit(EXIT_FAILURE); }

int serv_port, client_port1, client_port2;
class p2p{

private:

    server s1;
    int num_foreign_hosts;
    client c1[5]; 
    char filepath1[1000];
    char filepath2[1000];

public:

    p2p(){ ;}

    p2p(int serv_port, vector <pair<char *, int>> foreign_hosts){
        num_foreign_hosts = foreign_hosts.size();
        s1 = server(serv_port);
        for (int i=0; i<foreign_hosts.size(); i++){
            c1[i] = client(foreign_hosts[i].first, foreign_hosts[i].second);
        }
    }

    void file_to_transfer(char *file1, char *file2){
        strcpy(filepath1, "books/");
        strcat(filepath1, file1);
        strcpy(filepath2, "received/");
        strcat(filepath2, file2);
    }

    void start(){
        for (int i=0; i<num_foreign_hosts; i++){
            c1[i].connect2server();
        }
        s1.accept_connection();
        thread t1  = thread(&server::send_file, s1, filepath1);
        //s1.send_file(filepath1);
        thread t2[num_foreign_hosts];
        for (int i=0; i<num_foreign_hosts; i++){
            t2[i] = thread(&client::receive_data, c1[i], filepath2);
            //c1[i].receive_data(filepath2);
        }
        t1.join();
        for (int i=0; i<1000; i++)
        cout<<"Thread 1 finished"<<endl;
        for (int i=0; i<num_foreign_hosts; i++)
            t2[i].join();
        for (int i=0; i<1000; i++)
        cout<<"Thread 2 finished"<<endl;
    } 

};


int main(){

    cin>>serv_port>>client_port1>>client_port2;
    vector <pair <char *, int>> foreign_hosts;
    foreign_hosts.push_back({(char *)"127.0.0.1", client_port1});
    foreign_hosts.push_back({(char *)"127.0.0.1", client_port2});

    char file1 [1000], file2[1000];
    cin>>file1>>file2;

    p2p p1 (serv_port, foreign_hosts);
    p1.file_to_transfer(file1, file2);
    p1.start();

    return 0;
}