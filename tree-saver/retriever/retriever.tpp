
template <typename T>
T * TreeRetriever<T>::retrieve()
{
    nodesFile.open("nodes.dat");
    childrenFile.open("children.dat");

    if(nodesFile.fail() || childrenFile.fail())
    {
        puts("Error in opening files. May not exist, access denied, etc");
        return NULL;
    }

    // everything is fine
    // but not checking for if the data is correct or not
    // or if the structure while storing and retrieving are even same or not

    createNodes();

    // creation of all nodes has been done.
    // all the nodes's addresses are stored in hash indexed according to what we assigned during bfs
    // now just need to link them all up
    link();

    nodesFile.close();
    childrenFile.close();
    return hash[0];
}

template <typename T>
void TreeRetriever<T>::createNodes()
{
    T temp;

    while(!nodesFile.eof())
    {
        nodesFile.read((char *) &temp, sizeof(temp)); // all pointers are NULL, as we did so in TreeSaver::writeNode
        hash.push_back(new T(temp)); // added that node into the hash table at its correct index according to bfs
    }
}


template <typename T>
void TreeRetriever<T>::link()
{
    // traverse the children file and keep on mapping children indices to actuall addreses of new nodes created

    int i = -1; // need to index of the current node whose children are being attached

    while(!childrenFile.eof())
    {
        i++;
        int lc = -1, rc = -1;
        childrenFile >> lc >> rc;

        if(lc != -1) hash[i]->left = hash[lc];
        if(rc != -1) hash[i]->right = hash[rc];
    }
}
