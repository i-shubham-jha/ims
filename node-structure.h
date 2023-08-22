// this file just declares the node struct to be used by all the modules
// functions in modules are currently hardcoded wrt to these data fields
// so DO NOT MODIFY THESE

#include <string> // all modules #including this module, will automatically get string lib

struct Node
{
    unsigned int roll;
    std::string name;
    std::string fatherName;
    std::string motherName;
    unsigned long int phone; // 9999999999 can't be stored in uint
    std::string email;
    std::string address;
};
