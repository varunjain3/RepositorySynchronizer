#include <iostream>
#include <string>
#include <vector>

using namespace std;

void tokenize(std::string const &str, const char delim,
              std::vector<std::string> &out)
{
    size_t start;
    size_t end = 0;

    start = str.find(delim, 0) + 1;
    end = str.length();
    string main = str.substr(start, end - start);
    cout << str << endl
         << main;
}

int main()
{
    std::string s = "C*C++*Java";
    const char delim = '*';

    std::vector<std::string> out;
    tokenize(s, delim, out);

    for (auto &s : out)
    {
        std::cout << s << '\n';
    }

    return 0;
}