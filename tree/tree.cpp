#include "tree.h"

//*************INTERFACE FUNCTIONS**************/

// default constructor
Tree::Tree()
{
    root = NULL;
}



// parametrised constructor
void Tree::loadRetrievedTree(Node * root)
{
    this->root = root;
}




void Tree::addRecord(Node & node)
{
    Node * newNode = insertBST(node.roll, node.name, node.fatherName, node.motherName, node.phone, node.email, node.address);

    // balance factor may be off, so need to balance from this node upwards
    rebalance(newNode);
}


void Tree::removeRecord(unsigned int roll)
{
    Node * x = search(roll);

    if(x) // this record exists
    {
        x = deleteBST(x);
        // x contains address of physically to be deleted node, not yet deleted though
        rebalance(x->parent);

        free(x);
    }
}


Node Tree::searchRecord(unsigned int roll)
{
    Node * temp = search(roll);

    if(temp) return * temp; // node found with this roll number

    Node * newNode = new Node;
    newNode->roll = -1; // to indicate above that the searched roll was not found
    // won't even read other attribs, so can safely NOT init them
    return * newNode;
}


void Tree::updateRecord(Node & node)
{
    // assuming that this function will only be called when this roll already exists
    // this can be checked using the search function first

    Node * temp = search(node.roll);

    // roll NO cant be updated
    if(temp->name != node.name) temp->name = node.name;
    if(temp->fatherName != node.fatherName) temp->fatherName = node.fatherName;
    if(temp->motherName != node.motherName) temp->motherName = node.motherName;
    if(temp->phone != node.phone) temp->phone = node.phone;
    if(temp->email != node.email) temp->email = node.email;
    if(temp->address != node.address) temp->address = node.address;
}


std::vector<Node> Tree::getRecords()
{
    std::vector<Node> result;
    inorder(this->root, result);
    return result;
}



Node * Tree::search(unsigned int roll) // search just as in BST
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


// inorder traversal: helper function for getRecords
void Tree::inorder(Node * root, std::vector<Node> & result)
{
    // here root is more local than this->root, so that we will be used
    if(root)
    {
        inorder(root->left, result);
        result.push_back(*root);
        inorder(root->right, result);
    }
}


Node * Tree::createNewNode(unsigned int & roll, std::string & name, std::string & fatherName, std::string & motherName, unsigned long int & phone, std::string & email, std::string & address)
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
Node * Tree::insertBST(unsigned int & roll, std::string & name, std::string & fatherName, std::string & motherName, unsigned long int & phone, std::string & email, std::string & address)
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

        x = x->parent;
    }
}


// delete as in BST
// but return x's parent's pointer
Node * Tree::deleteBST(Node * x)
{
    Node * parent = x->parent;

    if(!x->left && !x->right) // no children of x
    {
        // need to make parent's children NULL
        if(x == parent->left) parent->left = NULL; // x was LC
        else parent->right = NULL; // x was RC
        return x;
    }
    else if(!x->left) // right child exists only
    {
        // child pointing to new parent
        x->right->parent = parent;

        // parent pointing to new child
        if(x == parent->left) parent->left = x->right;
        else parent->right = x->right;

        return x;
    }
    else if(!x->right) // left child exists only
    {
        // child pointing to new parent
        x->left->parent = parent;

        // parent pointing to new child
        if(x == parent->left) parent->left = x->left;
        else parent->right = x->left;

        return x;
    }
    else // both child exist
    {
        // replace with successor, then delete the successor
        // as both children exist successor is in RST

        Node * temp = x->right;

        while(temp->left) temp = temp->left;

        *x = *temp; // copy data of temp to x; overwrites here, no problem

        return deleteBST(temp); // as we need to return the address of the physically deleted node, which is now the earlier successor node
    }
}
