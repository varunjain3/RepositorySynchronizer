#include <fstream>
#include <string.h>

using namespace std;

// copy in binary mode
bool copyFile(const char *SRC, const char *DEST)
{
    ifstream src(SRC, ios::binary);
    ofstream dest(DEST, ios::binary);
    dest << src.rdbuf();
    return src && dest;
}

bool delFile(const char *path)
{
    return (remove(path) != 0) ? 0 : 1;
}

int main(int argc, char *argv[])
{
    return delFile(argv[1]);
    // return copyFile(argv[1], argv[2]) ? 0 : 1;
}
