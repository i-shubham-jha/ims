#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

template <typename T> class TreeRetriever
{
    std::vector<T *> hash; // to store the new physical addresses of new nodes created

    std::ifstream nodesFile;

    std::ifstream childrenFile;

    // function to read the nodes.dat file, create nodes and store their addresses in hash
    void createNodes();

    void link();
public:
    T * retrieve(); // function to retrieve the tree
};


// including the implementations using tpp file
// normal cpp file won't work
#include "retriever.tpp"
