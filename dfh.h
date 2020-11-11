#include <iostream>
#include <map>
#include <fstream>
#include <unistd.h>
#include <string.h>

using namespace std;

struct filestat
{
    string hash;
    bool folder;
};

// Filemap function
typedef map<string, filestat> filemap;
typedef pair<string, filestat> fileobject;

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

// int main()
// {
//     ifstream file_obj("Log.txt", ios::app);
//     filemap obj;
//     obj = ReadFile("Log.txt");
//     cout << "Helloworld";
//     return 0;
// }