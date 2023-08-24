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


// function to insert as in a normal BST
// returns address of newly created nodes
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



Node * Tree::search(int & roll) // search just as in BST
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


void Tree::leftRotate(Node * x)
{
    Node * y = x->right;

    // adjusting root of this new subtree
    y->parent = x->parent; // y is the new root of this subtree
    if(!y->parent) // if x was the root, the new root becomes y
    {
        this->root = y;
    }
    else // the parent of x exists
    {
        // need to check ki x was the LC or RC of its parent
        if(x == y->parent->left) // x was LC
        {
            y->parent->left = y; // make y as the new LC
        }
        else // x == y->parent->right, x was RC
        {
            y->parent->right = y; // make y as new RC
        }
    }

    // need to move y ka LC to x ka RC
    x->right = y->left;
    if(y->left) // LC of y exists
    {
        y->left->parent = x; // its new parent is x
    }

    // establishing final connections between x and y
    y->left = x; // x now becoms LC of y
    x->parent = y;

    //need to update the heights of x and y
    updateHeight(x);
    updateHeight(y);
}


// function to rotate right
void Tree::rightRotate(Node * x)
{
    Node * y = x->left;

    // making x's parent the parent of y
    y->parent = x->parent;

    if(!y->parent) // if x was root, now y becomes the root
    {
        this->root = y;
    }
    else // x was NOT the root
    {
        if(x == x->parent->left) // x was LC, so should y be
        {
            y->parent->left = y;
        }
        else // x == x->parent->right, x was RC, so should y be
        {
            y->parent->right = y;
        }
    }

    // making y's RC as x's LC
    x->left = y->right;
    if(x->left) // if LC exists
    {
        x->left->parent = x;
    }

    // making final connections between x and y
    x->parent = y;
    y->right = x;

    // updating the heights
    updateHeight(x);
    updateHeight(y);
}

// function to rebalance
void Tree::rebalance(Node * x)
{
    while(x)
    {
        updateHeight(x); // just to be sure

        if( height(x->left) >= 2 + height(x->right) )
        {
            // x is left heavy and NOT following AVL property

            if( height(x->left->left) >= height(x->left->right) ) // x's LC is left heavy or balanced
            {
                rightRotate(x);
            }
            else // x's LC is right heavy
            {
                leftRotate(x->left);
                rightRotate(x);
            }
        }
        else if( height(x->right) >= 2 + height(x->left) )
        {
            // x is right heavy and NOT following AVL property

            if( height(x->right->right) >= height(x->right->left) ) // x's RC is right heavy or balanced
            {
                leftRotate(x);
            }
            else // x's RC is left heavy
            {
                rightRotate(x->right);
                leftRotate(x);
            }
        }
    }
}
