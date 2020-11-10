#include <iostream>
#include <string.h> //for std::string
#include <fstream>
#include <map>
#include <list>
#include <dirent.h>
#include <unistd.h>
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
typedef list<string> filelist;
// Watchdog file watcher

class WatchDog
{

public:
    filemap Log;
    string rootdir;
    // Constructor, will initialize the Watchdog

    WatchDog(string path, string dest)
    {
        this->Log = getLog(path);
        this->rootdir = path;
        filemap empty;

        pair<filelist, filelist> adds = comparelog(this->Log, empty);
        filelist add_list = adds.first;
        filelist add_folder = adds.second;

        for (auto itr = add_list.begin(); itr != add_list.end(); itr++)
        {
            cout << endl
                 << "adding file- \t" << *itr;
            copyFile(this->rootdir, dest, *itr);
        }
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
            // cout << "Key not found";
            return "NotFound";
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
                    // cout << "\ninside hash checker- " << temp_path << endl;
                    tempstat.hash = md5_from_file(temp_path);
                }

                string new_path = temp_path.substr(temp_path.find("/", 0) + 1, temp_path.length());
                cout << new_path << " " << tempstat.folder << "\t Hash- " << tempstat.hash << endl;

                addkeyvalue(log, new_path, tempstat);
            }
        }
    }

    // Function to get the filemap of a root directory.
    filemap getLog(string path)
    {
        filemap newlog;
        cout << endl
             << "Getting Log for " << path << endl;
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

        // Make Directories

        string parent = DEST.substr(0, DEST.find_last_of("/"));
        string command = "mkdir -p " + parent;
        cout << "\nParent - " << parent << "\tCommand - " << (char *)command.c_str();
        system((char *)command.c_str());

        // Actuall Copying
        ifstream source(SRC, ios::binary);
        ofstream dest(DEST, ios::binary);

        istreambuf_iterator<char> begin_source(source);
        istreambuf_iterator<char> end_source;
        ostreambuf_iterator<char> begin_dest(dest);
        copy(begin_source, end_source, begin_dest);

        source.close();
        dest.close();
    }

    // removes files as per given path
    bool delFile(string dest, string file)
    {
        string path = dest + "/" + file;
        return (remove(path.c_str()) != 0) ? 0 : 1;
    }

    pair<filelist, filelist> comparelog(filemap src, filemap dest)
    {
        filelist modified_files;
        filelist modified_folders;

        for (auto itr = src.begin(); itr != src.end(); itr++)
        {
            string hash1, hash2;
            hash1 = gethash(src, itr->first);
            hash2 = gethash(dest, itr->first);
            cout << endl
                 << itr->first;
            if (hash1 != hash2)
            {
                if (itr->second.folder == false)
                    modified_files.push_back(itr->first);
                else
                {
                    modified_folders.push_back(itr->first);
                }
            }
        }
        return make_pair(modified_files, modified_folders);
    }

    void updatelog(filemap log)
    {
        this->Log = log;
    }
};

// To check if the Maps are same or not.
template <typename Map>
bool key_compare(Map const &lhs, Map const &rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin(),
                                                  [](auto a, auto b) { return a.first == b.first; });
}

void checkchanges(WatchDog &src, WatchDog &dest)
{

    filemap currlog;
    cout << "Root Dir - " << src.rootdir << endl;
    currlog = src.getLog(src.rootdir);

    pair<filelist, filelist> dels = src.comparelog(src.Log, currlog);
    filelist del_list = dels.first;
    filelist del_folder = dels.second;

    pair<filelist, filelist> adds = src.comparelog(currlog, src.Log);
    filelist add_list = adds.first;
    filelist add_folder = adds.second;

    for (auto itr = del_list.begin(); itr != del_list.end(); itr++)
    {
        cout << endl
             << "remove file- \t" << *itr;
        src.delFile(dest.rootdir, *itr);
    }

    for (auto itr = add_list.begin(); itr != add_list.end(); itr++)
    {
        cout << endl
             << "adding file- \t" << *itr;
        src.copyFile(src.rootdir, dest.rootdir, *itr);
    }

    for (auto itr = del_folder.begin(); itr != del_folder.end(); itr++)
    {
        string command = "rm -r " + dest.rootdir + "/" + *itr;
        cout << "\nDeleting folder... " << command;
        system((char *)command.c_str());
    }

    for (auto itr = add_folder.begin(); itr != add_folder.end(); itr++)
    {
        string command = "mkdir -p " + dest.rootdir + "/" + *itr;
        cout << "\nAdding folder... " << command;
        system((char *)command.c_str());
    }

    src.updatelog(currlog);
    dest.updatelog(currlog);
};

// Write file
int WriteFile(string fname, filemap *m)
{
    int count = 0;
    if (m->empty())
        return 0;

    FILE *fp = fopen(fname.c_str(), "w");
    if (!fp)
        return -errno;

    for (auto it = m->begin(); it != m->end(); it++)
    {
        fprintf(fp, "%s=%d,%s\n", it->first.c_str(), it->second.folder, it->second.hash.c_str());
        count++;
    }

    fclose(fp);
    return count;
}
/*
int main(int argc, char *argv[])
{

    //filestat temp;
    string folder1 = "test1";
    string folder2 = "test2";
    WatchDog g(folder1, folder2);
    WatchDog k(folder2, folder1);

    // ofstream file_obj("Log.txt", ios::app);
    // file_obj.write((char *)&g.Log, sizeof(g.Log));
    // file_obj.close();

    WriteFile("Log.txt", &g.Log);

    while (true)
    {
        // char hello;
        // cin >> hello;
        sleep(atoi(argv[1]));
        checkchanges(g, k);
        checkchanges(k, g);
    }
}
*/