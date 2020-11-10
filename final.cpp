#include <iostream>
#include <stdio.h>
#include <string.h>
#include <map>
#include "hell.h"
#include "file_transfer.h"

void serverplsadd(filestat addlist)
{
    for (auto itr = addlist.begin(); itr != addlist.end(); itr++)
    {
    }
}

int main(int argc, char *argv[])
{

    serv_port = atoi(argv[1]);

    client_port1 = atoi(argv[2]);
    // client_port2 = atoi(argv[3]);
    vector<pair<char *, int>> foreign_hosts;

    foreign_hosts.push_back({(char *)"127.0.0.1", client_port1});
    // foreign_hosts.push_back({(char *)"127.0.0.1", client_port2});

    char *file1 = argv[4];
    char *save_folder = argv[5];

    p2p p1(serv_port, foreign_hosts);
    p1.file_to_transfer(file1, save_folder);
    p1.start();

    return 0;
}
