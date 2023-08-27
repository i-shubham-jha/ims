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
#include <string.h>
#include <atomic>


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

    // utility function to jsonify a given node's data
    std::ostringstream JSONify(Node & node);

    // semaphores and other vars needed for reader writer problem v3
    std::mutex wrt; // for exlusive access to writer threads
    std::mutex read; // to control access to shared var readercount
    int readcount; // number of current readers reading the resource
    std::mutex order; // to fix starvation as in v3 of reader writer problem

    // atomic vars to help in shutdown of server
    // these vars (their data representation in MM) are the same as their base types
    // just that the ops on them are atomic now (only which are supported by the hardware though)
    std::atomic<bool> running; // the  makes this false so that while(running.fetch()) exits and new requests are NOT being served
    std::atomic<unsigned int> noOfThreads; // number of threads which have been created
public:
    // to init the sAddr struct and retrieve the tree saved onto HDD (if any)
    imsServer(std::string const & IP, short & port);

    void startServer(); // server starts listening and responding


    // function to close the server and save using treeSaver
    void stopServer();
};
