// Extra Tools for the Programs
#include "toolkit.h"

string md5_from_file(const string path)
{

    //Start opening your file
    ifstream inBigArrayfile;
    inBigArrayfile.open(path, ios::binary | ios::in);

    //Find length of file
    inBigArrayfile.seekg(0, ios::end);
    long Length = inBigArrayfile.tellg();
    inBigArrayfile.seekg(0, ios::beg);

    //read in the data from your file
    char *InFileData = new char[Length];
    inBigArrayfile.read(InFileData, Length);

    //Calculate MD5 hash
    string Temp = md5(InFileData);

    //Clean up
    delete[] InFileData;

    return Temp;
}