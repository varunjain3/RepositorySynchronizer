#include <iostream>
#include <stdio.h>
#include <string.h>
#include <map>
#include <mutex>
#include <thread>

#include "hell.h"
#include "file_transfer.h"

mutex m_lock;

p2p p1;
WatchDog w1;

void server_thread(string folder, vector<pair<char *, int>> foreign_hosts, int serv_port)
{

start:
    // Initializing for initial files to send
    cout << "intializing... watchdog" << endl;
    pair<filelist, filelist> adds = w1.initialize();
    filelist addfilelist = adds.first;
    filelist addfolderlist = adds.second;
    cout << "intialized watchdog" << endl;

    // |||||Call server add list here ||||||||
    p1.server_filelist(w1.rootdir, addfilelist);

    // Update loop
    while (true)
    {
        m_lock.lock();

        filepair adds = w1.checkchanges();
        filelist addfilelist = adds.first;
        filelist addfolderlist = adds.second;
        m_lock.unlock();

        // If connection broken
        if (p1.s1.get_connectedclients() == 0)
        {
            p1.initialise();
            goto start;
        }

        p1.server_filelist(w1.rootdir, addfilelist);
        cout << "back to watchdog after sleep" << endl;
        sleep(2);
    }
}

void client_thread(string destdir)
{

    while (true)
    {
        cout << "getting lock" << endl;
        while (m_lock.try_lock())
            cout
                << "Locked in client thread" << endl;

        cout << "client Lock acquired..." << endl;
        string file = p1.c1[0].receive_data((char *)destdir.c_str());

        if (file == "Log.txt")
        {
            filemap currLog = ReadFile("Log.txt");
            filepair dels = w1.comparelog(w1.Log, currLog);
            filelist del_list = dels.first;
            filelist del_folder = dels.second;

            for (auto itr = del_list.begin(); itr != del_list.end(); itr++)
            {
                cout << endl
                     << "remove file- \t" << *itr;
                w1.delFile(destdir, *itr);
            }

            for (auto itr = del_folder.begin(); itr != del_folder.end(); itr++)
            {
                string command = "rm -r " + destdir + "/" + *itr;
                cout << "\nDeleting folder... " << command;
                system((char *)command.c_str());
            }
        }
        else if (file == "empty")
        {
            m_lock.unlock();
            sleep(1);
        }
    }
};

int main(int argc, char *argv[])
{

    serv_port = atoi(argv[1]);

    client_port1 = atoi(argv[2]);

    // client_port2 = atoi(argv[3]);
    vector<pair<char *, int>> foreign_hosts;

    foreign_hosts.push_back({(char *)"127.0.0.1", client_port1});
    // foreign_hosts.push_back({(char *)"127.0.0.1", client_port2});

    // Initialize Server & Client!!
    p1 = p2p(serv_port, foreign_hosts);
    p1.initialise();

    string root_folder = argv[3];
    w1 = WatchDog(root_folder);
    cout << "Watchdog Ready" << endl;

    // |||||||||Call server add here|||||||||||||
    thread t1 = thread(server_thread, root_folder, foreign_hosts, serv_port);
    thread t2 = thread(client_thread, root_folder);

    t1.join();
    t2.join();

    return 0;
};