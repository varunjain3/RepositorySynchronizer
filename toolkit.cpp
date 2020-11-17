// Extra Tools for the Programs
#include "toolkit.h"

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