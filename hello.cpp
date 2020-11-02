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
private:
public:
    filemap Log;
    // Constructor, will initialize the Watchdog
    WatchDog();
    WatchDog(string path)
    {
        this->Log = getLog(path);
    }

    void addkeyvalue(filemap newlog, string key, filestat value)
    {
        newlog.insert(fileobject(key, value));
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
};

int main(int argc, char *argv[])
{

    WatchDog g();
    filestat temp;
    temp.folder = true;
    temp.hash = "path";
    g.addkeyvalue(g.Log, "myworld", temp);
    temp.folder = true;
    temp.hash = "path2";
    g.addkeyvalue(g.Log, "helloworld", temp);
    g.gethash("myworld");
    g.gethash("helloworld");
}