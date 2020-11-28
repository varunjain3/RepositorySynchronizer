/*
We use a Peer-2-Peer architecture for handling the file
transfers involved during synchronization.
Here each node acts both as a server,
and a client.

This is the Client-Side of the P2P --->
*/

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
#define PR(s)               \
    {                       \
        perror((s));        \
        exit(EXIT_FAILURE); \
    }
using namespace std;

// Server class (Exists for each host)

class client
{

private:
    char *serv_ip;
    in_port_t serv_port;
    sockaddr_in serv_addr;
    int sock;

public:
    // Constructor, will initialize the client-side of the P2P

    client() { ; }

    client(char *ip, int port)
    {
        serv_port = port;
        serv_ip = ip;
        // Creating Socket
        sock = socket(AF_INET, SOCK_STREAM, 0); // 0 defaults to TCP using SOCK_STREAM
        if (sock < 0)
            PR("socket failed");

        memset(&serv_addr, 0, sizeof(serv_addr)); // Initialising the sockaddr_in with 0

        serv_addr.sin_family = AF_INET; //IPv4
        serv_addr.sin_port = htons(serv_port);

        int rv = inet_pton(AF_INET, serv_ip, &serv_addr.sin_addr);
        if (rv == 0)
        {
            cerr << "Invalid sin_addr" << endl;
            exit(1);
        }
        else if (rv < 0)
            PR("pton failed");
    }

    int connect2server();

    string receive_data(char *);

    ~client() { ; }
};

/////////////////////////////////////////////////////////////
    // Client instance sends the connection request to other host Servers
    
int client::connect2server()
{
    while (1)
    {
        int t1 = connect(sock, (sockaddr *)&serv_addr, sizeof(serv_addr));
        if (t1 < 0)
        {
            perror("connect error");
            //return 0;
            sleep(1);
        }
        else
            break;
    }
    return 1;
}

/////////////////////////////////////////////////////////////
    // To receive file from other hosts

string client::receive_data(char *save_folder)
{
    //connect2server();

    int BUFSIZE = 1024, num_bytes = 0, FileSize = 0;
    char *filepath, file_desc[1024];

    memset(file_desc, '\0', sizeof(file_desc));
    do{
        memset(file_desc, '\0', sizeof(file_desc));
        num_bytes = recv(sock, file_desc, 1024, MSG_DONTWAIT);
    }   while (strcmp(file_desc, "AYT")==0);

    if (num_bytes < 0)
    {
        // perror("recv error");
        return "empty";
    }
    else if (num_bytes == 0)
    {
        cout << "Nothing Received from Server" << endl;
        return "broken";
    }
    else
    {
        cout << "Client Recieved bytes = " << num_bytes << endl;
    }

    //filepath | filesize
    cout << file_desc << endl;
    filepath = strtok(file_desc, "|");
    FileSize = atoi(strtok(NULL, "|"));
    cout << "filepath - " << filepath << " " << FileSize << endl;

    // Adding Root folder details to Filepath
    char filepath_temp[1024];
    strcpy(filepath_temp, save_folder);
    strcat(filepath_temp, filepath);
    filepath = filepath_temp;
    cout << "filepath2 - " << filepath << " " << FileSize << endl;

    string DEST = filepath;
    cout << "filepath3 - " << DEST << " " << FileSize << endl;
    string parent = DEST.substr(0, DEST.find_last_of("/"));
    string command = "mkdir -p " + parent;
    system((char *)command.c_str());

    unsigned int rec_len = 0;
    FILE *fp = fopen(filepath, "w");

    // Receiving data in the form of packets
    while (rec_len < FileSize)
    {
        char buffer[BUFSIZE + 1];
        num_bytes = recv(sock, buffer, min(BUFSIZE, (int)(FileSize - rec_len)), 0); //receives data from the transport layer
        if (num_bytes < 0)
        {
            perror("recv error");
            return "empty";
        }

        buffer[num_bytes] = '\0';

        rec_len += num_bytes;
        cout << "client recieved " << rec_len << " of file..." << endl;
        int rec_bytes = fwrite (buffer, 1, num_bytes, fp);
    }

    fclose(fp);

    for (int i = 0; i < 10; i++)
        cout << "received_finished" << endl;

    return filepath;
}

//int main(){

//client c1("127.0.0.1", 12345);
//c1.connect2server();
//char filepath[] = "received/50.txt";
//c1.receive_data(filepath);

//return 0;
//}

#endif