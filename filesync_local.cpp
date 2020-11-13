#include <iostream>
#include "toolkit.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Enter Parameters correctly" << endl;
        cout << "usage- ./filesync_local (time2sleep) root1 root2";
    }

    //filestat temp;
    string folder1 = argv[2];
    string folder2 = argv[3];
    WatchDog g(folder1, folder2);
    WatchDog k(folder2, folder1);
    cout << "Folders initialized succesfully!" << endl;

    while (true)
    {
        sleep(atoi(argv[1]));
        cout << "Checking for changes..." << endl;
        checkchanges(g, k);
        checkchanges(k, g);
        cout << "Watchdog going to sleep..." << endl;
    }
}
