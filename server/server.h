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
public:
    // to init the sAddr struct and retrieve the tree saved onto HDD (if any)
    imsServer(std::string const & IP, short & port);

    void startServer(); // server starts listening and responding
};
