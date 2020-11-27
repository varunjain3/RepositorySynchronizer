#include <iostream>
#include <stdio.h>
#include <string.h>
#include <map>
#include <mutex>
#include <thread>
#include <vector>
#include "utils.h"

#include "hell.h"
#include "file_transfer.h"

mutex m_lock;

p2p p1;
WatchDog w1;
int ticket = 0;
int turn_global = 0;
vector<pair<char *, int>> foreign_hosts;
string root_folder;
bool send_to_all = false;

void server_thread(string folder, vector<pair<char *, int>> foreign_hosts, int serv_port, int turn)
{

start:
    // Initializing for initial files to send
    cout << "intializing... watchdog" << endl;

    m_lock.lock();
    cout << "server lock acquired" << endl;

    // |||||Call server add list here ||||||||
    if (p1.s1.get_connectedclients()!=0){
        if (ticket == turn){
            pair<filelist, filelist> adds = w1.checkchanges();
            filelist addfilelist = adds.first;
            filelist addfolderlist = adds.second;
            cout << "intialized watchdog" << endl;
            p1.server_filelist(w1.rootdir, addfilelist);
            ticket = (ticket + 1) % (p1.s1.get_connectedclients() + 2);
            send_to_all = false;
            cout << "server lock released" << endl;
            m_lock.unlock();
        }
        else{
            cout << "server lock released" << endl;
            m_lock.unlock();
            sleep (1);
            goto start;
        }
    }
    else {
        ticket = p1.s1.get_connectedclients() + 1;
        cout << "server lock released" << endl;
        m_lock.unlock(); 
        sleep (2);
        goto start;
    }
    // Update loop
    while (true)
    {
        //for (int i=0; i<1000; i++)
        m_lock.lock();
        cout << "server lock acquired" << endl;
        cout<<ticket<<" "<<turn<<endl;
        if (ticket != turn){
            m_lock.unlock();
            sleep (1);
            continue;
        }

        cout << "watchdog calculating changed..." << endl;
        filepair adds = w1.checkchanges();
        filelist addfilelist = adds.first;
        filelist addfolderlist = adds.second;
        cout << "Watchdog Changes calculated..." << endl;

        // If connection broken
        //if (p1.s1.get_connectedclients() == 0)
        //{
            //m_lock.unlock();
            //sleep(2);
            ////p1.initialise();
            //goto start;
        //}
        if (send_to_all && turn == 0)
        {
            filelist addfilelist_temp;
            for (auto itr = w1.Log.begin(); itr!=w1.Log.end(); itr++)
            {
                if (itr->second.folder == false && itr->first!="Log.txt")
                {
                    addfilelist_temp.push_back((*itr).first);
                }
            }
            addfilelist_temp.push_back("Log.txt");
            p1.server_filelist(w1.rootdir, addfilelist_temp);
            send_to_all = false;
        }
        else
        {
            send_to_all = false;
            p1.server_filelist(w1.rootdir, addfilelist);
        }
        cout << "back to watchdog after sleep" << endl;
        ticket = (ticket + 1) % (p1.s1.get_connectedclients() + 2);
        cout << "server lock released" << endl;
        sleep(2);
        m_lock.unlock();
    }
}

void client_thread(string destdir, int clientno)
{

    while (true)
    {

        m_lock.lock();
        cout << "client lock acquired" << endl;
                
        if (ticket == turn_global || ticket == p1.s1.get_connectedclients() + 1){
            cout<<ticket<<" "<<turn_global<<endl;
            cout<< "client lock unlocked" << endl;
            m_lock.unlock();
            sleep (1);
            continue;
        }
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
            ticket = (ticket + 1) % (p1.s1.get_connectedclients() + 2);
            cout<<ticket<<" "<<turn_global<<endl;
            cout << "client lock unlocked" << endl;
            m_lock.unlock();
        }
        else if (file == "empty")
        {
            cout << "client lock unlocked" << endl;
            m_lock.unlock();
            cout<<ticket<<" "<<turn_global<<endl;
            sleep(2);
        }
        else if (file == "broken"){
            //cerr<<"broken"<<endl;
            //cerr<<"ticket "<<ticket<<" "<<"turn "<<turn_global<<endl;
            //ticket = (ticket + 1) % (p1.s1.get_connectedclients() + 2);
            p1.checkonline();
            cout << "client lock unlocked" << endl;
            m_lock.unlock();
            cout<<"connection broken" <<endl;
            break;
        }
        else {
            cout << "client lock unlocked" << endl;
            m_lock.unlock();
        }
    }
};

void initiate_connection(){
    vector <thread> client_threads;
    int ctr = 0;
    thread t1[10];
    while (1){
        m_lock.lock();
        p1.checkonline();
        cout << "initiate lock acquired " << endl;
        if (ticket >= p1.s1.get_connectedclients() + 1){
            for (int i=0; i<foreign_hosts.size(); i++){
                //if (is_connected[i]) continue;
                    if (p1.initialise(i)){
                        is_connected[i] = 1;
                        ticket += 1;
                        t1[ctr] = thread(client_thread, root_folder, ctr);
                        ctr++;
                        send_to_all = true;
                    }
            }
            ticket = 0;
        }
        cout << "initiate lock released " << endl;
        m_lock.unlock();
        sleep(1);
    }
    for (int i=0; i<=ctr; i++){
        t1[i].join();
    }
}

int main(int argc, char *argv[])
{

    serv_port = atoi(argv[2]);

    client_port1 = atoi(argv[3]);
    client_port2 = atoi(argv[4]);
    client_port3 = atoi(argv[5]);
    int turn = atoi(argv[6]); turn_global = turn;


    foreign_hosts.push_back({(char *)"127.0.0.1", client_port1});
    foreign_hosts.push_back({(char *)"127.0.0.1", client_port2});
    foreign_hosts.push_back({(char *)"127.0.0.1", client_port3});
    is_connected.assign(foreign_hosts.size(), 0);
    // Initialize Server & Client!!
    p1 = p2p(serv_port, foreign_hosts);
    //p1.initialise();

    root_folder = argv[1];
    w1 = WatchDog(root_folder);
    cout << "Watchdog Ready" << endl;

    // |||||||||Call server add here|||||||||||||
    thread t1 = thread(server_thread, root_folder, foreign_hosts, serv_port, turn);
    thread t2 = thread(initiate_connection);
    //thread t2 = thread(client_thread, root_folder, 0);
    //thread t3 = thread(client_thread, root_folder, 1);

    t1.join();
    t2.join();
    //t3.join();

    return 0;
};