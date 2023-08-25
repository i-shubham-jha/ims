#include "server.h"
#include <sys/socket.h>

// logging function
void imsServer::log(std::string const & str)
{
    std::cout << str << std::endl;
}

// init sockaddr_in struct
// create socket and bind it to the address provided
// retrieve the tree if stored on HDD
imsServer::imsServer(std::string const & IP, short & port)
{
    // need to convert everything in network byte order
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(port);
    // inet_addr converts from dotted decimal to network byte ordered binary
    sAddr.sin_addr.s_addr = inet_addr(IP.c_str());
    sAddrLen = sizeof(sAddr);

    // inits done

    // CREATING A NEW SOCKET
    // SOCK_STREAM: sequenced, reliable, two-way, connection-based byte streams.
    socketFD = socket(AF_INET, SOCK_STREAM, 0);

    if(socketFD == -1)
    {
        log("Failed to create new socket\nExiting...");
        exit(1);
    }
    else
    {
        std::ostringstream str;
        str << "Created socket: " << socketFD;
        log(str.str());
    }
    // SOCKET CREATION DONE


    // BINDING THE SOCKET
    if(bind(socketFD,(sockaddr *) &sAddr, sAddrLen) == -1)
    {
        log("Failed to bind socket\nExiting...");
        exit(1);
    }
    else
    {
        std::ostringstream str;
        str << "Socket bound to port: " << ntohs(sAddr.sin_port);
        log(str.str());
    }
    // BINDING DONE

    // init the tree
    // retrive the tree saved if any
    TreeRetriever<Node> retriever;

    // if the tree is NOT Saved then retriever would return NULL anyways
    tree.loadRetrievedTree(retriever.retrieve());
}


// function to actually start listening and responding
void imsServer::startServer()
{
    // STARTING LISTENING ON SOCKET
    if(listen(socketFD, SOMAXCONN)) // SOMAXCONN is the max number of requests in queue
    {
        log("Failed to start listening\nExiting...");
        exit(1);
    }
    else
    {
        log("Listening has started");
    }
    // LISTENING HAS STARTED


    // ACCEPTING NEW CONNECTIONS AND SERVING THEM
    while(true)
    {
        // accept function removes the first connection request from the pending queue of socketFD
        // saves the address of the caller client process in the struct pointed to by the second parameter
        // if second and third params are NULL, then the caller's address are Not stored anywhere
        // allocates and returns a new socket with the same type as socketFD
        int newSocket = accept(socketFD, NULL, NULL);
    }
}







