#include "tree.h"

//*************INTERFACE FUNCTIONS**************/

// default constructor
Tree::Tree()
{
    root = NULL;
}



// parametrised constructor
Tree::Tree(Node * root)
{
    this->root = root;
}




void Tree::addRecord(int & roll, std::string & name, std::string & fatherName, std::string & motherName, unsigned long int & phone, std::string & email, std::string & address)
{

}


void Tree::removeRecord(int & roll)
{

}


Node Tree::searchRecord(int & roll)
{
    Node * temp = search(roll);

    if(temp) return * temp; // node found with this roll number

    Node * newNode = new Node;
    newNode->roll = -1; // to indicate above that the searched roll was not found
    // won't even read other attribs, so can safely NOT init them
    return * newNode;
}


void Tree::updateRecord(int & roll, std::string & name, std::string & fatherName, std::string & motherName, unsigned long int & phone, std::string & email, std::string & address)
{

}


std::vector<Node> getRecords()
{

}

/************PRIVATE FUNCTIONS*******/

int Tree::height(Node * x)
{
    if(!x) return -1; // no node no height. Helps in function updateHeight
    else return x->height;
}



void Tree::updateHeight(Node * x)
{
    if(x)
    {
        x->height = std::max(height(x->left), height(x->right)) + 1; //agar children nahi hai then max gives -1 ==> 0 height of this node
    }
}


Node * Tree::createNewNode(int & roll, std::string & name, std::string & fatherName, std::string & motherName, unsigned long int & phone, std::string & email, std::string & address)
{
    Node * newNode = new Node;

    newNode->left = newNode->right = newNode->parent = NULL; // anaath hai yeh node abhi

    // a new node will be inserted as a leaf (initially) so has height 0 only
    newNode->height = 0; //kyunki both child NULL hai with heights -1

    // adding data to this node
    newNode->roll = roll;
    newNode->name = name;
    newNode->fatherName = fatherName;
    newNode->motherName = motherName;
    newNode->phone = phone;
    newNode->email = email;
    newNode->address = address;

    return newNode;
}



Node * Tree::insertBST(int & roll, std::string & name, std::string & fatherName, std::string & motherName, unsigned long int & phone, std::string & email, std::string & address)
{
    Node * newNode = createNewNode(roll, name, fatherName, motherName, phone, email, address);

    if(!root)
    {
        // no need of any balancing
        root = newNode; // parent is already NULL
        return newNode;
    }

    // tree already exists
    Node * temp = root, * tempParent = NULL; // root's parent is NULL

    while(temp)
    {
        if(roll < temp->roll)
        {
            tempParent = temp;
            temp = temp->left;
        }

        if(roll > temp->roll)
        {
            tempParent = temp;
            temp = temp->right;
        }

        // Not considering equality case, if needed update any one of the above with equality
    }

    // tempParent is the parent of newNode
    if(roll < tempParent->roll) tempParent->left = newNode;
    else tempParent->right = newNode;

    // updating the parent of newNode
    newNode->parent = tempParent;

    return newNode;
}



Node * Tree::search(int & roll)
{
    Node * temp = root;

    while(temp)
    {
        if(roll == temp->roll) break;
        else if(roll < temp->roll) temp = temp->left;
        else temp = temp->right;
    }

    return temp;
}



