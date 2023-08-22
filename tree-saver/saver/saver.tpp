

#include <sys/stat.h>
inline void log(const std::string & str)
{
    std::cout << str << std::endl;
}

template <typename T>
void TreeSaver<T>::save(T * root)
{
    log("starting save function");

    if(!root)
    {
        log("Tree does NOT exist\nExiting...");
        return;
    }

    // tree exists
    // doing single level order traversal and storing nodes/children in respective files

    std::ofstream fout; // for writing to "nodes" file
    std::ofstream childout; // for writing to "children" file
    fout.open("nodes.dat");
    childout.open("children.dat");

    std::queue<T *> q; // for BFS

    q.push(root);

    int count = 1; // number of nodes which have been explored till now

    // standard BFS implementation
    while(!q.empty())
    {
        root = q.front();
        q.pop();

        // adding this node to the nodes.dat file
        writeNode(fout, root );

        // adding its children in the queue
        int lc = -1, rc = -1;

        if(root->left)
        {
            count++;
            lc = count - 1;
            q.push(root->left);
        }
        // left child DNE, but lc is already -1, so no need of any modification

        if(root->right)
        {
            count++;
            rc = count - 1;
            q.push(root->right);
        }

        childout << lc << " " << rc << "\n";
    }

    fout.close();
    childout.close();
}

template <typename T>
void TreeSaver<T>::writeNode(std::ofstream & fout, T * nodeptr)
{
    T temp(*nodeptr);

    // won't store the actuall address in the file, just making them NULL
    temp.left = NULL;
    temp.right = NULL;

    fout.write((char * ) &temp, sizeof(temp) ); // all the data members of the OG node have been written to the hdd file nodes.dat
}
