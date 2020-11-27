#include <iostream>
#include <string.h>
#include <fstream>
#include <map>
#include <list>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include "utils.h"
#include "md5.h"

using namespace std;

// Filemap function
typedef list<string> filelist;
typedef map<string, filestat> filemap;
typedef pair<string, filestat> fileobject;
typedef pair<filelist, filelist> filepair;

// Watchdog file watcher

class WatchDog
{

public:
    filemap Log;
    string rootdir;
    // Constructor, will initialize the Watchdog

    WatchDog() { ; }
    WatchDog(string path)
    {
        this->Log;
        this->rootdir = path;
    }

    /////////////////////////////////////////////////////////////
    // To initialize the wathdog instance
    filepair initialize()
    {
        filemap empty;
        pair<filelist, filelist> adds = comparelog(this->Log, empty);
        return adds;
    }

    /////////////////////////////////////////////////////////////
    // A function to add new key-value pairs to the logs
    void addkeyvalue(filemap &newlog, string key, filestat value)
    {
        newlog.insert(fileobject(key, value));
    }

    /////////////////////////////////////////////////////////////
    // To retrieve the hash of the file from the log
    string gethash(filemap log, string key)
    {
        auto itr = log.find(key);
        if (itr == log.end())
            return "NotFound";
        else
            return itr->second.hash;
    }
    ////////////////////////////////////////////////////////////
    // To check if the file is directory or not
    bool isdir(const char *name)
    {
        struct stat ps;
        if (stat(name, &ps) != 0)
            return false;

        return S_ISDIR(ps.st_mode);
    }
    ////////////////////////////////////////////////////////////
    // To file the file map with file objects
    void iterator(string path, filemap &log)
    {
        dirent *entity;
        DIR *folder;
        string temp_path;
        filestat tempstat;
        folder = opendir(path.c_str());
        // iterates for all the files in the folder
        while ((entity = readdir(folder)) != NULL)
        {
            if ((strcmp(entity->d_name, ".") != 0) && (strcmp(entity->d_name, "..") && strcmp(entity->d_name, "Log.txt")) != 0)
            {
                char *fname = entity->d_name;
                temp_path = path + "/" + fname;

                if (isdir(temp_path.c_str()))
                {
                    //Makes a filestat if file exists
                    tempstat.folder = true;
                    tempstat.hash = "NULL";
                    iterator(temp_path, log);
                }
                else
                {
                    tempstat.folder = false;
                    tempstat.hash = md5_from_file(temp_path);
                }

                string new_path = temp_path.substr(temp_path.find("/", 0) + 2, temp_path.length());
                // Adds files to the log
                addkeyvalue(log, new_path, tempstat);
            }
        }
    }
    ///////////////////////////////////////////////////////////
    // Function to get the filemap of a root directory.
    filemap getLog(string path)
    {
        filemap newlog;
        iterator(path, newlog);
        return newlog;
    }

    ///////////////////////////////////////////////////////////
    // Add Files as per given src and destination
    bool copyFile(string srcfolder, string destfolder, string filename)
    {
        string SRC = srcfolder + "/" + filename;
        string DEST = destfolder + "/" + filename;

        // Make Directories
        string parent = DEST.substr(0, DEST.find_last_of("/"));
        string command = "mkdir -p " + parent;
        system((char *)command.c_str());

        // Actuall Copying
        ifstream source(SRC, ios::binary);
        ofstream dest(DEST, ios::binary);
        istreambuf_iterator<char> begin_source(source);
        istreambuf_iterator<char> end_source;
        ostreambuf_iterator<char> begin_dest(dest);
        copy(begin_source, end_source, begin_dest);

        // Closing files
        source.close();
        dest.close();
    }

    ///////////////////////////////////////////////////////////
    // removes files as per given path
    bool delFile(string dest, string file)
    {
        string path = dest + "/" + file;
        return (remove(path.c_str()) != 0) ? 0 : 1;
    }

    ///////////////////////////////////////////////////////////
    // A functino to compare to given filemap(log) to tell changes
    // that are in src but not in dest
    filepair comparelog(filemap src, filemap dest)
    {
        filelist modified_files;
        filelist modified_folders;

        for (auto itr = src.begin(); itr != src.end(); itr++)
        {
            string hash1, hash2;
            hash1 = gethash(src, itr->first);
            hash2 = gethash(dest, itr->first);
            cout << itr->first << endl;
            if (hash1 != hash2)
            {
                if (itr->second.folder == false)
                    modified_files.push_back(itr->first);
                else
                    modified_folders.push_back(itr->first);
            }
        }
        return make_pair(modified_files, modified_folders);
    }

    ///////////////////////////////////////////////////////////
    // Function to update the current log with the given log
    void updatelog(filemap log)
    {
        this->Log = log;
    }

    ///////////////////////////////////////////////////////////
    // A function to add the log.txt file in the list that is to be
    // sent to the server
    void addlogtolist(filepair &adds, filemap log)
    {
        filelist addfiles = adds.first;
        string logfile = this->rootdir + "Log.txt";
        cout << "Saving Logs..." << endl;
        WriteFile(logfile, &log);
        addfiles.push_back("Log.txt");

        adds = make_pair(addfiles, adds.second);
    }
    ///////////////////////////////////////////////////////////
    // To check if there are new changes in the current folder
    filepair checkchanges()
    {
        filemap currlog;
        cout << "Root Dir - " << this->rootdir << endl;
        currlog = this->getLog(this->rootdir);
        filepair adds = this->comparelog(currlog, this->Log);
        filepair dells = this->comparelog(this->Log, currlog);

        addlogtolist(adds, currlog);
        this->updatelog(currlog);
        return adds;
    }
};
