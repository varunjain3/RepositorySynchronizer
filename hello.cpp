#include <iostream>
#include <string.h> //for std::string
#include <fstream>
#include <map>

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
    // WatchDog(string path)
    // {
    //     this->Log = getLog(path);
    // }

    void addkeyvalue(filemap &newlog, string key, filestat value)
    {
        newlog.insert(fileobject(key, value));
    }
    void addkey2self(string key, filestat value)
    {
        this->Log.insert(fileobject(key, value));
    }

    string gethash(string key)
    {
        auto itr = this->Log.find(key);

        if (itr == this->Log.end())
        {
            cout << "Key not found";
            return NULL;
        }

        else
            return itr->second.hash;
    }

    // To be done by harshit
    // return a map of
    filemap getLog(string path)
    {
        filemap newlog;
        filestat temp;
        temp.folder = true;
        temp.hash = "asdfasd";

        this->addkeyvalue(newlog, "path", temp);
        return newlog;
    }

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

int main(int argc, char *argv[])
{

    WatchDog g;
    filemap a;
    filestat temp;
    string folder1 = "test1";
    string folder2 = "test2";

    temp.folder = true;
    temp.hash = "path";
    g.addkeyvalue(g.Log, "Hello", temp);
    g.addkey2self("myworld", temp);
    temp.folder = true;
    temp.hash = "path2";
    g.addkey2self("helloworld", temp);
    cout << g.gethash("myworld") << endl;
    cout << g.gethash("helloworld") << endl;
    g.copyFile(folder1, folder2, "file1.txt");
    g.delFile("test2/file1.txt");
}