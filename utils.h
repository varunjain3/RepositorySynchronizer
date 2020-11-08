#include <iostream>
#include <fstream>

using namespace std;

//Works well on linux (stackoverflow had an answer about tellg giving slightly less filesize in windows)
int get_filesize(char * filename){
    ifstream input(filename, ios::binary);
    input.seekg(0, ios::end);
    return (int)input.tellg();
}