// this is an independent ims server module

#include "../tree/tree.h"
#include "../tree-saver/tree-saver.h"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <thread>
#include <mutex>
#include <cstdio>


/********************INTERFACE SPECIFICATION********************
Specify the following HTTP methods for the corresponding operations
search: GET; Pass roll in body
update: PUT; pass all the data fields in body
add: POST; pass all the data fields in body
delete: DELETE; pass roll in body

the order of fields in the body is hardcoded so it should be in that order only:
roll
name
fatherName
motherName
phone
email
address

for example:
body: JSON.stringify({
            roll: value,
            name: value,
            fatherName: value,
            motherName: value,
            phone: value,
            email: value,
            address: value
        })
*********************INTERFACE SPECS END*********************/


class imsServer
{
    // the tree to be used
    Tree tree;

    int socketFD; // socket FD for this server process
    sockaddr_in sAddr; // struct to hold the IP address and port number
    int sAddrLen; // length of sAddr, to be used in accept function

    // logging function
    void log(std::string const & str);

    // function to retrieve the HTTP method from the client connection's header
    std::string getMethod(int sock);

    // functions to handle differnet HTTP methods
    void handleGET(int sock);
    void handlePUT(int sock);
    void handlePOST(int sock);
    void handleDELETE(int sock);

    // utility functions to extract data from HTTP body; to be used with above 4 handle functions
    unsigned int extractRoll(char * buff, int size); // required in search and delete ops
    Node extractData(char * buff, int size); // required in update and add ops


    // semaphores and other vars needed for reader writer problem v3
    std::mutex wrt; // for exlusive access to writer threads
    std::mutex read; // to control access to shared var readercount
    int readcount; // number of current readers reading the resource
    std::mutex order; // to fix starvation as in v3 of reader writer problem

public:
    // to init the sAddr struct and retrieve the tree saved onto HDD (if any)
    imsServer(std::string const & IP, short & port);

    void startServer(); // server starts listening and responding
};
