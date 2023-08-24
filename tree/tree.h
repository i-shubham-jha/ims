// this indendent module provides the tree object and all its functions
// all the data fields are hardcoded

#include "../node-structure.h"
#include <vector>

class Tree
{
    Node * root;

    // functino to get height of a node x
    int height(Node * x);

    // function to update height of node x
    void updateHeight(Node * x);

    // functino to create a new node with given data
    Node * createNewNode(int & roll, std::string & name, std::string & fatherName, std::string & motherName, unsigned long int & phone, std::string & email, std::string & address);

    // function to insert as in a normal BST
    // return pointer to the newly inserted node
    Node * insertBST(int & roll, std::string & name, std::string & fatherName, std::string & motherName, unsigned long int & phone, std::string & email, std::string & address);



    // function to left rotate the tree rooted at x
    void leftRotate(Node * x);

    // function to right rotate the tree rooted at x
    void rightRotate(Node * x);

    // function to balance the nodes from x upwards
    void rebalance(Node * x);

    // inorder traversal function: helper for getRecords
    void inorder(Node * root, std::vector<Node> & result);
public:

    // default constructor
    Tree();

    // paremeterised constructor: use to init with already saved tree retriever using tree-saver
    Tree(Node * root);

    // function to add a new record
    void addRecord(int & roll, std::string & name, std::string & fatherName, std::string & motherName, unsigned long int & phone, std::string & email, std::string & address);

    // function to remove the record with the given roll no
    void removeRecord(int & roll);

    // function to search the record
    // if not found, then returns a node struct with roll set to -1
    // DON'T USE THIS CURRENTLY, use below search function to get the address directly, or NULL if roll DNE
    Node searchRecord(int & roll);

    // function to search the node with the given roll
    // returns the pointer to that node, NULL means no such node found
    // search just like in a BST
    Node * search(int & roll);

    // function to update a record with given roll no
    // send the entire data, even if some field not even updated
    // roll no can't be updated; here just for searching the node
    void updateRecord(int & roll, std::string & name, std::string & fatherName, std::string & motherName, unsigned long int & phone, std::string & email, std::string & address);

    // function to get all the records as a vector
    // returns in increasing order of roll number
    std::vector<Node> getRecords();
};
