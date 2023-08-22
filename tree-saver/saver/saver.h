#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <queue>
#include <sys/stat.h> // for mkdir

// PLEASE ONLY PROVIDE TREE'S NODE STRUCTURE AS THE TEMPLATE TYPENAME
// THE LEFT AND RIGHT CHILDS SHOULD BE NAMED 'left' and 'right'
// and should be pointers to same structure
// make sure that the variables are publicly accessible

// a template so that any kind of tree could be saved
template <typename T> class TreeSaver
{
    // just writes the node to the output file "nodes"
    void writeNode(std::ofstream & fout, T * nodeptr);

public:
    // function to begin saving of the tree on hdd
    void save(T * root);
};

#include "saver.tpp"
