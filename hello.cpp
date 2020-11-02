#include <iostream>
#include <string.h> //for std::string
#include <fstream>
#include <map>

using namespace std;

struct filestat
{
    string hash;
    bool folder;
};

class Log
{
private:
    map<string, filestat> hello;

public:
    void addkeyvalue(string key, filestat value)
    {
        this->hello.insert(pair<string, filestat>(key, value));
    }
    void gethash(string key)
    {
        auto itr = this->hello.find(key);

        if (itr == this->hello.end())
        {
            cout << "Key not found";
        }
        else
        {
            cout << "Address is " << itr->first << " hash is " << itr->second.hash << endl;
        }
    }

    // To be done by harshit
    void getLog(string path)
    {
        filestat temp;
        temp.folder = true;
        temp.hash = "asdfasd";
        this->addkeyvalue("path", temp);
    }
};

///Your node class
class A_Node
{
public:
    A_Node(string path, bool folder)
    {
        this->folder = folder;
        this->path = path;
        this->Nsubdir = 0;
        // this->subdirs = NULL;
        this->subdirs = (A_Node **)malloc(sizeof(A_Node *));
    }

    //in the destructor of a node it would need some special care
    ~A_Node()
    {
        //for every child call the destructor of each child
        for (int i = 0; i < this->Nsubdir; i++)
        {
            delete this->subdirs[i];
        }
        //and only then free the buffer of the pointers to the children
        free(this->subdirs);
    }
    A_Node *getChild()
    {
        A_Node *newNode = (A_Node *)malloc(sizeof(A_Node));
        if (!newNode)
        {
            cout << "Memory Error A_Node";
            exit(1);
        }
        return newNode;
    }

    //adds a child
    void addChild(A_Node *subdir)
    {
        //reallocate if needed
        // this->subdirs[Nsubdir] = getChild();

        this->subdirs[Nsubdir] = subdir;
        // realloc(this->subdirs, sizeof(A_Node *));
        this->Nsubdir++;
    }

    void printpath()
    {
        cout << "This is the Path for this" << this->path << endl;
    }

    // A_Node *getChild(unsigned int i)
    // {
    //     if (i >= this->childrenN)
    //     {
    //         return 0;
    //     }

    //     return this->children[i];
    // }
    A_Node **subdirs;

private:
    bool folder;
    string path;
    int Nsubdir;
};

/*
///Your tree class
class A_Tree
{
public:
    //constructor
    A_Tree(int rootValue)
    {
        root = new A_Node(rootValue);
    }
    //destructor
    ~A_Tree()
    {
        //recursively kills all the nodes due to the destructor of node
        delete root;
    }
    //your root node
    A_Node *root;
};
*/
int main(int argc, char *argv[])
{
    // A_Node my("hello", true);
    // my.printpath();
    // my.addChild(new A_Node("gg", false));
    // my.subdirs[0]->printpath();
    // my.addChild(new A_Node("bb", false));
    // my.subdirs[1]->printpath();
    // my.addChild(new A_Node("ff", false));
    // my.subdirs[2]->printpath();

    Log g;
    filestat temp;
    temp.folder = true;
    temp.hash = "path";
    g.addkeyvalue("myworld", temp);
    temp.folder = true;
    temp.hash = "path2";
    g.addkeyvalue("helloworld", temp);
    g.gethash("myworld");
    g.gethash("helloworld");
}