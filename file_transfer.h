/*
We use a Peer-2-Peer architecture for handling the file
transfers involved during synchronization.
Here each node acts both as a server,
and a client.
This is the main P2P class that further uses 
Server and Client classes for each Host --->
*/

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
#define PR(s)               \
    {                       \
        perror((s));        \
        exit(EXIT_FAILURE); \
    }

int serv_port, client_port1, client_port2, client_port3;

// P2P class Handles File Transfers (Exists for each host)

class p2p
{

private:
    int num_foreign_hosts;
    char filepath1[1024];
    char save_folder[1024];

public:
    server s1;
    client c1[5];

    // Constructor, will initialize the P2P

    p2p() { ; }

    p2p(int serv_port, vector<pair<char *, int>> foreign_hosts)
    {
        num_foreign_hosts = foreign_hosts.size(); // All the other Hosts
        // Server Initialized
        s1 = server(serv_port);
        for (int i = 0; i < foreign_hosts.size(); i++)
        {
            c1[i] = client(foreign_hosts[i].first, foreign_hosts[i].second);
        }
    }

    /////////////////////////////////////////////////////////////
    // [Only for Debugging] Trash function: Used to test the working of P2P without Watchdog
    void file_to_transfer(char *file1, char *file2)
    {
        strcpy(filepath1, "books/");
        strcat(filepath1, file1);
        strcpy(save_folder, file2);
    }

    /////////////////////////////////////////////////////////////
    // To initialize the P2P instance
    int initialise(int index)
    {
        if (c1[index].connect2server()==0) {
            return 0;
        }
        cout << "client initialized...";
        s1.accept_connection();
        cout << "server accepted";

        return 1; //if everything worked smoothly
    }

    /////////////////////////////////////////////////////////////
    // To send the files in addlist to all the other hosts
    void server_filelist(string rootdir, filelist addlist)
    {
        cout << "starting file transfer..." << endl;
        for (auto itr = addlist.begin(); itr != addlist.end(); itr++)
        {
            s1.send_file((char *)rootdir.c_str(), (char *)(*itr).c_str());
            cout << "server sent file" << endl;
        }
        cout << "returning from server file list..." << endl;
    }

    void checkonline(){
        s1.checkonline();
    }

    ~p2p() { ; }
};
// }
// }
// ;

// int main(int argc, char * argv[]){

//     serv_port = atoi(argv[1]); client_port1 = atoi(argv[2]); client_port2 = atoi(argv[3]);
//     vector <pair <char *, int>> foreign_hosts;
//     foreign_hosts.push_back({(char *)"127.0.0.1", client_port1});
//     foreign_hosts.push_back({(char *)"127.0.0.1", client_port2});

//     char *file1 = argv[4];
//     char *save_folder = argv[5];

//     p2p p1 (serv_port, foreign_hosts);
//     p1.file_to_transfer(file1, save_folder);
//     p1.start();

//     return 0;
// }
