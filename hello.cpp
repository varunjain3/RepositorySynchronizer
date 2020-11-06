#include <iostream>
#include <string.h> //for std::string
#include <fstream>
#include <map>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include "toolkit.h"
#include "md5.h"

using namespace std;

struct filestat
{
    string hash;
    bool folder;
};

// Filemap function
typedef map<string, filestat> filemap;
typedef pair<string, filestat> fileobject;
// Watchdog file watcher

class WatchDog
{

public:
    filemap Log;
    // Constructor, will initialize the Watchdog
    // WatchDog();

    WatchDog(string path)
    {
        this->Log = getLog(path);
    }

    void addkeyvalue(filemap &newlog, string key, filestat value)
    {
        newlog.insert(fileobject(key, value));
    }
    void addkey2self(string key, filestat value)
    {
        this->Log.insert(fileobject(key, value));
    }

    string gethash(filemap log, string key)
    {
        auto itr = log.find(key);
        if (itr == log.end())
        {
            cout << "Key not found";
            return NULL;
        }
        else
            return itr->second.hash;
    }

    // To be done by harshit
    // return a map of
    ////////////////////////////////////////////////////////////

    bool isdir(const char *name)
    {
        struct stat ps;
        if (stat(name, &ps) != 0)
            return false;

        return S_ISDIR(ps.st_mode);
    }

    // To file the file map with file objects
    void iterator(string path, filemap &log)
    {
        dirent *entity;
        DIR *folder;
        string temp_path;
        filestat tempstat;
        folder = opendir(path.c_str());
        while ((entity = readdir(folder)) != NULL)
        {
            if ((strcmp(entity->d_name, ".") != 0) && (strcmp(entity->d_name, "..") != 0))
            {
                char *fname = entity->d_name;
                temp_path = path + "/" + fname;

                if (isdir(temp_path.c_str()))
                {
                    tempstat.folder = true;
                    tempstat.hash = "NULL";
                    iterator(temp_path, log);
                }
                else
                {
                    tempstat.folder = false;
                    tempstat.hash = md5_from_file(temp_path);
                }

                cout << temp_path << " " << tempstat.folder << "\t Hash- " << tempstat.hash << endl;
                addkeyvalue(log, temp_path, tempstat);
            }
        }
    }

    // Function to get the filemap of a root directory.
    filemap getLog(string path)
    {
        filemap newlog;
        iterator(path, newlog);
        return newlog;
    }

    ///////////////////////////////////////////////////////////
    // Add Files as per given src and destination
    // bool copyFile(const char *SRC, const char *DEST)
    // {
    //     ifstream src(SRC, ios::binary);
    //     ofstream dest(DEST, ios::binary);
    //     dest << src.rdbuf();
    //     return src && dest;
    // }

    bool copyFile(string srcfolder, string destfolder, string filename)
    {
        string SRC = srcfolder + "/" + filename;
        string DEST = destfolder + "/" + filename;

        ifstream src(SRC, ios::binary);
        ofstream dest(DEST, ios::binary);
        dest << src.rdbuf();
        return src && dest;
    }

    // removes files as per given path
    bool delFile(string path)
    {
        return (remove(path.c_str()) != 0) ? 0 : 1;
    }
};

// To check if the Maps are same or not.
template <typename Map>
bool key_compare(Map const &lhs, Map const &rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin(),
                                                  [](auto a, auto b) { return a.first == b.first; });
}

int main(int argc, char *argv[])
{

    //filestat temp;
    string folder1 = "test1";
    //string folder2 = "test2";
    WatchDog g(folder1);
    filemap a;
    string in;
    cin >> in;
    a = g.getLog(folder1);

    if (key_compare(a, g.Log))
    {
        cout << "Same";
    }
    else
    {
        cout << "Not Same";
    }

    // a = g.getLog("/home/varun/os/RepositorySynchornizer/test1");
    cout << endl
         << folder1 + "/test11/file1.txt \t" << g.gethash(g.Log, folder1 + "/test11/file1.txt") << endl;
}
