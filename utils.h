#ifndef utils
#define utils

#include <iostream>
#include <fstream>
#include <sstream>
#include "md5.h"
#include <vector>

using namespace std;

#include <vector>
//Works well on linux (stackoverflow had an answer about tellg giving slightly less filesize in windows)
int get_filesize(char *filename)
{
    ifstream input(filename, ios::binary);
    input.seekg(0, ios::end);
    return (int)input.tellg();
}

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
#endif