#include "server.h"


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


// function to get the method from HTTP connection
std::string imsServer::getMethod(int sock)
{
    std::string method;
    char buff[2] = {0};

    read(sock, buff, 1);

    while(buff[0] != ' ')
    {
        method += buff[0];
        read(sock, buff, 1);
    }
    return method;
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

        std::string method = getMethod(newSocket); // getting the method associated


        // the functions to be called below need to first parse the request
        // sent by the client, so that tree functions could be called with
        // appropriate params
        // Parsing the data is NOT part of critical section
        // tree is the shared resource, so accessing its functions are CS
        //
        if(method == "GET") // search record; reader
        {
            std::thread threadObj(&imsServer::handleGET, this, newSocket);
            threadObj.detach();
        }
        else if(method == "PUT") // update record; writer
        {
            std::thread threadObj(&imsServer::handlePUT, this, newSocket);
            threadObj.detach();
        }
        else if(method == "POST") // add new record; writer
        {
            std::thread threadObj(&imsServer::handlePOST, this, newSocket);
            threadObj.detach();
        }
        else if(method == "DELETE") // delete record; writer
        {
            std::thread threadObj(&imsServer::handleDELETE, this, newSocket);
            threadObj.detach();
        }
        else // other shit
        {

        }
    }
}

// function to extract roll from body
unsigned int imsServer::extractRoll(char * buffer, int size)
{
    // data in body is like:
    // {"roll":"21149"}
    int i = 0;
    while(i < size && buffer[i] != '{') i++;

    // reached the bracket before "roll"
    i += 9; // now we have reached the first digit of roll
    std::string roll;

    // we don't know the length of roll so just read till " encountered
    while(buffer[i] != '"') roll += buffer[i++];

    return std::stoul(roll);
}


// function to handle search for a roll GET
void imsServer::handleGET(int sock)
{
    // first need to read everything into a buff
    // first need to parse the roll out of the body
    // then search for it in critical section
    char buffer[4096] = {0};
    read(sock,buffer, sizeof(buffer));

    // all data is in buffer
    unsigned int roll = extractRoll(buffer, 4096);

    Node * node = tree.search(roll);

}


// function to handle adding a new record: POST
void imsServer::handlePOST(int sock)
{

}


// function to handle updating an existing record
void imsServer::handlePUT(int sock)
{

}

// functino to handle deleting a record: DELETE
void imsServer::handleDELETE(int sock)
{
    // first gett all the body data
    char buffer[4096] = {0};
    read(sock,buffer, sizeof(buffer));

    // get the roll number to be deleted
    unsigned int roll = extractRoll(buffer, 4096);

    Node * node = tree.search(roll);

    std::ostringstream response; // the response which will be sent

    if(!node) // not found the roll
    {
        std::string body("{\"message\":\"Roll number does NOT exist\"}");
        response << "HTTP/1.1 404 Not Found\nContent-Type: application/json\nContent-Length: " << body.size() << "\n\n" << body;
    }
    else // if roll exists
    {
        tree.removeRecord(roll);

        std::ostringstream body;

        // this body is the HTTP response body

        body << "{\"message\":\"Roll number "<< roll << " deleted\"}";

        // response starts with the header, where each attrib should be on a separate line
        // then comes a blank line between the header and the body
        // then comes the body, size of which should be mentioned in the header
        response << "HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: " << body.str().size() << "\n\n" << body.str();
    }

    // sending back the response
    if(write(sock, response.str().c_str(), response.str().size()) == -1) log("error in sending response of DELETE");
}

