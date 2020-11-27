#ifndef utils
#define utils

#include <iostream>
#include <fstream>
#include <sstream>
#include "md5.h"
#include <vector>

using namespace std;

vector<int> is_connected;
//Works well on linux (stackoverflow had an answer about tellg giving slightly less filesize in windows)
int get_filesize(char *filename)
{
    ifstream input(filename, ios::binary);
    input.seekg(0, ios::end);
    return (int)input.tellg();
}

/////////////////////////////////////////////////////////////
// md5_from_file
// This function calculates the unique md5 hash of a file.
// path - path of the file.
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

/////////////////////////////////////////////////////////////
struct filestat
{
    string hash;
    bool folder;
};

// Filemap function
typedef map<string, filestat> filemap;
typedef pair<string, filestat> fileobject;

/////////////////////////////////////////////////////////////
// Function to write the log file in txt to send to other clients
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
        fprintf(fp, "%s=%d,%s\n", it->first.c_str(), it->second.folder, (char *)(it->second.hash.c_str()));
        count++;
    }

    fclose(fp);
    return count;
}

/////////////////////////////////////////////////////////////
// Function to read the log file sent as text file
filemap ReadFile(string fname)
{
    int count = 0;

    FILE *fp = fopen(fname.c_str(), "r");

    char *buf = 0;
    size_t buflen = 0;
    filemap m;
    while (getline(&buf, &buflen, fp) > 0)
    {
        char *nl = strchr(buf, '\n');
        if (nl == NULL)
            continue;
        *nl = 0;

        char *sep = strchr(buf, '=');
        if (sep == NULL)
            continue;
        *sep = 0;
        sep++;

        char *sep2 = strchr(sep, ',');
        if (sep2 == NULL)
            continue;
        *sep2 = 0;
        sep2++;

        string s1 = buf;
        filestat s2;

        s2.hash = sep2;
        s2.folder = atoi(sep);

        m.insert(fileobject(s1, s2));

        count++;
    }

    if (buf)
        free(buf);

    fclose(fp);
    return m;
}

#endif