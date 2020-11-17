#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include "md5.h"

using namespace std;

// md5_from_file
// This function calculates the unique md5 hash of a file.
// path - path of the file.
string md5_from_file(const string path);
