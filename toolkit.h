#include <iostream>
#include <string.h> //for std::string
#include <fstream>
#include <map>
#include <list>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include "md5.h"

using namespace std;

//////////////////////////////////////////////////////////////////
// Structure to hold the filesats for each file
struct filestat
{
    string hash;
    bool folder;
};

//////////////////////////////////////////////////////////////////
// Some typedef for easy access
typedef map<string, filestat> filemap;
typedef pair<string, filestat> fileobject;
typedef list<string> filelist;
typedef pair<filelist, filelist> file_pairlist;

//////////////////////////////////////////////////////////////////

// Watchdog Class
class WatchDog
{

public:
    filemap Log;
    string rootdir;
    // Constructor, will initialize the Watchdog

    WatchDog() { ; }
    WatchDog(string path, string dest);

    void addkeyvalue(filemap &newlog, string key, filestat value)
    {
        newlog.insert(fileobject(key, value));
    }
    void addkey2self(string key, filestat value)
    {
        this->Log.insert(fileobject(key, value));
    }
    void updatelog(filemap log)
    {
        this->Log = log;
    }

    string gethash(filemap log, string key);
    bool isdir(const char *name);
    void iterator(string path, filemap &log);
    filemap getLog(string path);
    bool copyFile(string srcfolder, string destfolder, string filename);
    bool delFile(string dest, string file);
    file_pairlist comparelog(filemap src, filemap dest);
};

// Sends changes from one watchdog to other
void checkchanges(WatchDog &src, WatchDog &dest);

// md5_from_file
// This function calculates the unique md5 hash of a file.
// path - path of the file.
string md5_from_file(const string path);
