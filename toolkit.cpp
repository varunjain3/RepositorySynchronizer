// Extra Tools for the Programs
#include "toolkit.h"

///////////////////////////////////////////////////////////
// Constructor for WatchDog
WatchDog::WatchDog(string src, string dest)
{
    // Giving handles of the src folder to the object
    this->Log = getLog(src);
    this->rootdir = src;
    filemap empty;

    // Making the initial changes list that needs to be added to the other folder
    pair<filelist, filelist> adds = comparelog(this->Log, empty);
    filelist add_list = adds.first;
    filelist add_folder = adds.second;

    // Adding initial changes
    cout << "Initializing contents of " << src << " to " << dest << " ..." << endl;
    for (auto itr = add_list.begin(); itr != add_list.end(); itr++)
    {
        cout << "Copying file " << *itr << " ..." << endl;
        copyFile(this->rootdir, dest, *itr);
    }
}
/////////////////////////////////////////////////////////////
// To retrieve the hash of the file from the log
string WatchDog::gethash(filemap log, string key)
{
    auto itr = log.find(key);
    if (itr == log.end())
        return "NotFound";
    else
        return itr->second.hash;
}
//////////////////////////////////////////////////////////////
// To check if the file is directory or not
bool WatchDog::isdir(const char *name)
{
    struct stat ps;
    if (stat(name, &ps) != 0)
        return false;

    return S_ISDIR(ps.st_mode);
}
//////////////////////////////////////////////////////////////
void WatchDog::iterator(string path, filemap &log)
{
    dirent *entity;
    DIR *folder;
    string temp_path;
    filestat tempstat;
    folder = opendir(path.c_str());
    // iterates for all the files in the folder
    while ((entity = readdir(folder)) != NULL)
    {
        if ((strcmp(entity->d_name, ".") != 0) && (strcmp(entity->d_name, "..") != 0))
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

            string new_path = temp_path.substr(temp_path.find("/", 0) + 1, temp_path.length());
            // Adds files to the log
            addkeyvalue(log, new_path, tempstat);
        }
    }
}
//////////////////////////////////////////////////////////////
filemap WatchDog::getLog(string path)
{
    filemap newlog;
    iterator(path, newlog);
    return newlog;
}
//////////////////////////////////////////////////////////////
bool WatchDog::copyFile(string srcfolder, string destfolder, string filename)
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
//////////////////////////////////////////////////////////////
bool WatchDog::delFile(string dest, string file)
{
    string path = dest + "/" + file;
    return (remove(path.c_str()) != 0) ? 0 : 1;
}
//////////////////////////////////////////////////////////////
file_pairlist WatchDog::comparelog(filemap src, filemap dest)
{
    filelist modified_files;
    filelist modified_folders;

    for (auto itr = src.begin(); itr != src.end(); itr++)
    {
        string hash1, hash2;
        hash1 = gethash(src, itr->first);
        hash2 = gethash(dest, itr->first);
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
//////////////////////////////////////////////////////////////

string md5_from_file(const string path)
{
    ifstream t(path);
    stringstream buffer;
    buffer << t.rdbuf();

    //Calculate MD5 hash
    string Temp = md5(buffer.str());

    //Clean up
    buffer.str("");

    return Temp;
}
//////////////////////////////////////////////////////////////
void checkchanges(WatchDog &src, WatchDog &dest)
{
    filemap currlog;
    currlog = src.getLog(src.rootdir);

    pair<filelist, filelist> dels = src.comparelog(src.Log, currlog);
    filelist del_list = dels.first;
    filelist del_folder = dels.second;

    pair<filelist, filelist> adds = src.comparelog(currlog, src.Log);
    filelist add_list = adds.first;
    filelist add_folder = adds.second;

    for (auto itr = del_list.begin(); itr != del_list.end(); itr++)
    {
        cout << "Removed File- \t" << *itr << endl;
        src.delFile(dest.rootdir, *itr);
    }

    for (auto itr = add_list.begin(); itr != add_list.end(); itr++)
    {
        cout << "Added File- \t" << *itr << endl;
        src.copyFile(src.rootdir, dest.rootdir, *itr);
    }

    for (auto itr = del_folder.begin(); itr != del_folder.end(); itr++)
    {
        string command = "rm -r " + dest.rootdir + "/" + *itr;
        cout << "Deleting folder- " << *itr << endl;
        system((char *)command.c_str());
    }

    for (auto itr = add_folder.begin(); itr != add_folder.end(); itr++)
    {
        string command = "mkdir -p " + dest.rootdir + "/" + *itr;
        cout << "Adding folder- " << *itr << endl;
        system((char *)command.c_str());
    }

    src.updatelog(currlog);
    dest.updatelog(currlog);
};