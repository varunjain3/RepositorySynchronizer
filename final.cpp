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
int ticket = 0;
int turn_global = 0;

void server_thread(string folder, vector<pair<char *, int>> foreign_hosts, int serv_port, int turn)
{

start:
    // Initializing for initial files to send
    cout << "intializing... watchdog" << endl;
    m_lock.lock();
    pair<filelist, filelist> adds = w1.initialize();
    filelist addfilelist = adds.first;
    filelist addfolderlist = adds.second;
    m_lock.unlock();
    cout << "intialized watchdog" << endl;

    // |||||Call server add list here ||||||||
    p1.server_filelist(w1.rootdir, addfilelist);

    // Update loop
    while (true)
    {
        cout<<ticket<<" "<<turn<<endl;
        while (ticket != turn)
            ;
        m_lock.lock();
        cout << "watchdog calculating changed..." << endl;
        filepair adds = w1.checkchanges();
        filelist addfilelist = adds.first;
        filelist addfolderlist = adds.second;
        cout << "Watchdog Changes calculated..." << endl;

        // If connection broken
        if (p1.s1.get_connectedclients() == 0)
        {
            p1.initialise();
            goto start;
        }

        p1.server_filelist(w1.rootdir, addfilelist);
        cout << "back to watchdog after sleep" << endl;
        ticket = (ticket + 1) % 3;
        m_lock.unlock();
        sleep(2);
    }
}

void client_thread(string destdir, int clientno)
{

    while (true)
    {
        cout << "getting lock" << endl;
        while (m_lock.try_lock())
            ;

        // cout << "Locked in client thread" << endl;

        // cout << "client Lock acquired..." << endl;
        string file = p1.c1[clientno].receive_data((char *)destdir.c_str());

        if (file == w1.rootdir + "Log.txt")
        {
            filemap currLog = ReadFile(w1.rootdir + "Log.txt");
            filepair dels = w1.comparelog(w1.Log, currLog);
            filelist del_list = dels.first;
            filelist del_folder = dels.second;

            pair<filelist, filelist> adds = w1.comparelog(currLog, w1.Log);
            filelist add_list = adds.first;
            filelist add_folder = adds.second;

            for (auto itr = del_list.begin(); itr != del_list.end(); itr++)
            {
                bool to_delete = true;
                for (auto itr1 = add_list.begin(); itr1 != add_list.end(); itr1++){
                    if (*itr == *itr1){
                        to_delete = false;
                    }
                }
                if (to_delete){
                    cout << endl
                        << "remove file- \t" << *itr;
                    w1.delFile(destdir, *itr);
                }
            }

            for (auto itr = del_folder.begin(); itr != del_folder.end(); itr++)
            {
                string command = "rm -r " + destdir + "/" + *itr;
                cout << "\nDeleting folder... " << command;
                system((char *)command.c_str());
            }

            for (auto itr = add_folder.begin(); itr != add_folder.end(); itr++)
            {
                    string command = "mkdir -p " + destdir + "/" + *itr;
                    cout << "Adding folder- " << *itr << endl;
                    system((char *)command.c_str());
            }

            w1.updatelog(currLog);
            WriteFile(w1.rootdir + "Log.txt", &currLog);
            ticket = (ticket + 1) % 3;
            cout<<ticket<<" "<<turn_global<<endl;
            m_lock.unlock();
            cout << "client lock unlocked" << endl;
        }
        else if (file == "empty")
        {
            m_lock.unlock();
            cout << "client lock unlocked" << endl;
            sleep(2);
        }
    }
};

int main(int argc, char *argv[])
{

    serv_port = atoi(argv[2]);

    client_port1 = atoi(argv[3]);
    client_port2 = atoi(argv[4]);
    int turn = atoi(argv[5]); turn_global = turn;

    vector<pair<char *, int>> foreign_hosts;

    foreign_hosts.push_back({(char *)"127.0.0.1", client_port1});
    foreign_hosts.push_back({(char *)"127.0.0.1", client_port2});

    // Initialize Server & Client!!
    p1 = p2p(serv_port, foreign_hosts);
    p1.initialise();

    string root_folder = argv[1];
    w1 = WatchDog(root_folder);
    cout << "Watchdog Ready" << endl;

    // |||||||||Call server add here|||||||||||||
    thread t1 = thread(server_thread, root_folder, foreign_hosts, serv_port, turn);
    thread t2 = thread(client_thread, root_folder, 0);
    thread t3 = thread(client_thread, root_folder, 1);

    t1.join();
    t2.join();
    t3.join();

    return 0;
};