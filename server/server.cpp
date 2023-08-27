#include "server.h"
#include <netinet/in.h>
#include <thread>

/*******************************UTILITY FUNCTIONS*****************/

// logging function
void imsServer::log(std::string const & str)
{
    struct tm t;
    time_t now = time(0);
    char * tt = ctime(&now);
    std::cout << "\n" << tt << str << std::endl;
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

// function to extract all the data from the body and return it as a node
Node imsServer::extractData(char * buff, int size)
{
    // sample data to be parsed (lot of shit in header also there, following is just the body)
    // {"roll":"100","name":"baap","fatherName":"Papa ka naam","motherName":"Mata ka naam","phone":"100","email":"x@t.co","address":"White House, Patna"}

    // in the following approach we are just traversing the buffer
    // IF SPACE AFTER COMMAS IN REQUEST BODY, then make appropriate changes to index increments

    std::string str;
    Node node;

    int i = 0;
    while(i < size && buff[i] != '{') i++; // tryna reach the opening brace of json body

    // roll
    i += 9; // reached first digit of roll
    while(buff[i] != '"') str += buff[i++];
    node.roll = std::stoul(str);

    // name
    i += 10;
    str.clear();
    while(buff[i] != '"') str += buff[i++];
    node.name = str;

    // fatherName
    i += 16;
    str.clear();
    while(buff[i] != '"') str += buff[i++];
    node.fatherName = str;

    // motherName
    i += 16;
    str.clear();
    while(buff[i] != '"') str += buff[i++];
    node.motherName = str;

    // phone
    i += 11;
    str.clear();
    while(buff[i] != '"') str += buff[i++];
    node.phone = std::stoul(str);

    // email
    i += 11;
    str.clear();
    while(buff[i] != '"') str += buff[i++];
    node.email = str;

    // address
    i += 13;
    str.clear();
    while(buff[i] != '"') str += buff[i++];
    node.address = str;

    return node;
}

// function to jsonify a given node's data
std::ostringstream imsServer::JSONify(Node & node)
{
    // make as:
    // {"firstName":"shubham","lastName":"jha","phone":"9874563210","email":"21149@iiitu.ac.in"}
    std::ostringstream str;

    str << "{\"roll\":\"" << node.roll
        << "\",\"name\":\"" << node.name
        << "\",\"fatherName\":\"" << node.fatherName
        << "\",\"motherName\":\"" << node.motherName
        << "\",\"phone\":\"" << node.phone
        << "\",\"email\":\"" << node.email
        << "\",\"address\":\"" << node.address
        << "\"}";

    return str;
}

/*****************************UTIL FUNCS ENDS********************/




/*********************************GENERAL SERVER/SOCKET PART***********************/

// init sockaddr_in struct
// create socket and bind it to the address provided
// retrieve the tree if stored on HDD
imsServer::imsServer(std::string const & IP, short & port)
{
    // setting the atomic variables
    running.store(true); // the server should be running
    noOfThreads.store(0); // no threads currently

    log("init of atomic vars done");

    // need to convert everything in network byte order
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(port);
    // inet_addr converts from dotted decimal to network byte ordered binary
    sAddr.sin_addr.s_addr = inet_addr(IP.c_str());
    sAddrLen = sizeof(sAddr);

    log("init of sockaddr_in done");

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
        std::ostringstream str;
        str << "Failure in binding socket to port " << ntohs(sAddr.sin_port);
        log(str.str() + "\nExiting...");
        exit(1);
    }
    else
    {
        std::ostringstream str;
        str << "Socket successfully bound to port " << ntohs(sAddr.sin_port);
        log(str.str());
    }
    // BINDING DONE

    // init the tree
    // retrive the tree saved if any
    TreeRetriever<Node> retriever;

    // if the tree is NOT Saved then retriever would return NULL anyways
    tree.loadRetrievedTree(retriever.retrieve());

    if(tree.getRoot())// tree existed on HDD and was retrieved
    {
        log("Saved tree has been retrieved");
    }
}


// function to get the method from HTTP connection
std::string imsServer::getMethod(int sock)
{
    std::string method;
    char buff[2] = {0};

    // recv with fourth arg = 0 has similar functionality to read
    // TBD: investigate: read is giving problems when including <mutex> or <pthread.h>
    recv(sock, buff, 1, 0);

    while(buff[0] != ' ')
    {
        method += buff[0];
        recv(sock, buff, 1, 0);
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
        std::ostringstream str;
        str << ntohs(sAddr.sin_port);
        log("Server started listening on port " + str.str());
    }
    // LISTENING HAS STARTED


    // ACCEPTING NEW CONNECTIONS AND SERVING THEM
    while(running.load()) // atomic readin of this running value. stopServer makes this false, and hence no new connections get entertained
    {
        // accept function removes the first connection request from the pending queue of socketFD
        // saves the address of the caller client process in the struct pointed to by the second parameter
        // if second and third params are NULL, then the caller's address are Not stored anywhere
        // allocates and returns a new socket with the same type as socketFD
        sockaddr_in temp;
        socklen_t len = sizeof(temp);
        int newSocket = accept(socketFD,(sockaddr *) &temp, &len);

        std::ostringstream str;
        str << "Accepted a new connection from " << ntohl(temp.sin_addr.s_addr) << ":" << ntohs(temp.sin_port);
        log(str.str());

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

// function to stop the server
// save the tree on hdd using tree-saver
// close all the threads
void imsServer::stopServer()
{
    log("called stopServer");
    // make the running var false atomically
    // so that no new connections are getting entertained now
    running.store(false); // now the while(running) loop in startServer exits

    log("Stopped accepting new requests");

    // now no new threads for responses would be created
    // but some threads may be accessing the tree or waiting on some mutex
    // they will all execute and finish eventually
    // and noOfThreads will become 0 eventually.
    // so we can wait till noOfThreads becomes 0
    // and then call the tree-saver

    while(noOfThreads.load() > 0)
    {
        // there are still threads which are executing.
        // so we need to wait but don't want a busy waiting solution.
        // so we move this current thread to the end of running queue.
        // other threads (which we want to close before moving forward),
        // will now be scheduled.
        std::this_thread::yield();
    }

    log("all threads accessing the tree have stopped");

    // all threads have finished
    // can now save the tree
    TreeSaver<Node> saver;
    saver.save(tree.getRoot());
    log("saved the tree onto HDD");
    // the tree has been saved
    // can close everything now and stop exec of program

    close(socketFD);
}

/**************************GENERAL SOCKET/SERVER ENDS*******************/



//************************READER/WRITER PROBLEM v3**********************/

/*******************************

general templates for:

writer
{
    wait(order)
    wait(wrt)
    signal(order)
    // write
    signal(wrt)
}

reader
{
    wait(order)
    wait(mutex) // to protect readcount
    readcount++
    if(readcount == 1) wait(wrt); // first reader, block all the writers
    signal (order);
    signal(mutex); // so that other readers may join in
    // read
    wait(mutex);
    readcount--;
    if(readcount == 0) signal(wrt); // last reader, allow the writer to come in
    signal(mutex);

}

*****************************/



// function to handle search for a roll GET
void imsServer::handleGET(int sock)
{
    // one more thread
    noOfThreads.fetch_add(1);

    // first need to read everything into a buff
    // first need to parse the roll out of the body
    // then search for it in critical section
    char buffer[4096] = {0};
    recv(sock,buffer, sizeof(buffer), 0);

    // all data is in buffer
    unsigned int roll = extractRoll(buffer, 4096);

    std::ostringstream response;


    // CS Begins
    order.lock();
    read.lock(); // to protect the readcount var
    readcount++;
    if(readcount == 1)
    {
        // first reader lock wrt so that no reader could come in
        wrt.lock();
    }
    order.unlock(); // we have been served
    read.unlock(); // so that other readers could enter

    Node * temp = tree.search(roll);

    if(!temp) // this roll DNE
    {
        // not need the critical section now

        read.lock();
        readcount--;
        if(readcount == 0) // this is the last reader, needs to unlock wrt so that writers may come in
        {
            wrt.unlock();
        }
        read.unlock();

        std::string body("{\"message\":\"Roll number does NOT exist\"}");

        response << "HTTP/1.1 404 Not Found\nContent-Type: application/json\nContent-Length: " << body.size() << "\n\n" << body;
    }
    else // roll number was, need to send the data back in JSON in the hardcoded order only
    {
        // get the data stored in a temp variable, so that we don't have
        // to reference the OG tree nodes. We will be able to unlock the tree earlier
        Node node = *temp;

        // can free tree now
        read.lock();
        readcount--;
        if(readcount == 0)
        {
            wrt.unlock();
        }
        read.unlock();

        // now just JSONify the data in node and then send it back
        std::ostringstream body = JSONify(node);

        response << "HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: " << body.str().size() << "\n\n" << body.str();
    }

    if(write(sock, response.str().c_str(), response.str().size()) == -1) log("error in sending response of DELETE");

    close(sock);

    // one thread is exiting
    noOfThreads.fetch_sub(1);
}


// function to handle adding a new record: POST
void imsServer::handlePOST(int sock)
{
    // one more thread
    noOfThreads.fetch_add(1);

    // reading all data first
    char buffer[4096] = {0};
    recv(sock, buffer, sizeof(buffer), 0);

    Node newNode = extractData(buffer, 4096);

    // CS starts, accessing tree
    // this is writer
    order.lock();
    wrt.lock();
    order.unlock(); // as we got the x-lock on resource: matlab resource hi mil gaya hai

    tree.addRecord(newNode); // accessing the shared resource

    wrt.unlock(); // common resource ka use ho gaya hai, ab non-CS kaam karna hai karna hai

    std::ostringstream body, response;

    body << "{\"message\":\"Roll number "<< newNode.roll << " added succesfully\"}";

    response << "HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: " << body.str().size() << "\n\n" << body.str();

    if(write(sock, response.str().c_str(), response.str().size()) == -1) log("error in sending response of DELETE");

    close(sock);

    // one less thread
    noOfThreads.fetch_sub(1);
}


// function to handle updating an existing record
void imsServer::handlePUT(int sock)
{
    // one more thread
    noOfThreads.fetch_add(1);

    // call this only when this roll exists

    // reading all data first
    char buffer[4096] = {0};
    recv(sock, buffer, sizeof(buffer), 0);

    Node node = extractData(buffer, 4096);

    // CS begins
    order.lock();
    wrt.lock();
    order.unlock();

    this->tree.updateRecord(node);

    wrt.unlock();

    std::ostringstream body, response;

    body << "{\"message\":\"Roll number "<< node.roll << " updated succesfully\"}";

    response << "HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: " << body.str().size() << "\n\n" << body.str();

    if(write(sock, response.str().c_str(), response.str().size()) == -1) log("error in sending response of DELETE");

    close(sock);

    //one less thread
    noOfThreads.fetch_sub(1);
}

// functino to handle deleting a record: DELETE
void imsServer::handleDELETE(int sock)
{
    // increment the number of threads atomically
    noOfThreads.fetch_add(1); // atomic increment

    // first gett all the body data
    char buffer[4096] = {0};
    recv(sock,buffer, sizeof(buffer), 0);

    // get the roll number to be deleted
    unsigned int roll = extractRoll(buffer, 4096);

    std::ostringstream response; // the response which will be sent

    // CS starts
    order.lock(); // to fix starvation problem in v3
    wrt.lock(); // we got exclusive lock on the tree
    order.unlock(); // so now we have been served and now can release the

    Node * node = tree.search(roll);


    if(!node) // not found the roll
    {
        wrt.unlock(); // hamara shared resource tree se kaam ho gaya
        std::string body("{\"message\":\"Roll number does NOT exist\"}");
        response << "HTTP/1.1 404 Not Found\nContent-Type: application/json\nContent-Length: " << body.size() << "\n\n" << body;
    }
    else // if roll exists
    {
        tree.removeRecord(roll);
        wrt.unlock(); // hamara shared resource tree se kaam ho gaya, need to release the lock ASAP to increase performance


        std::ostringstream body;

        // this body is the HTTP response body

        body << "{\"message\":\"Roll number "<< roll << " deleted\"}";

        // response starts with the header, where each attrib should be on a separate line
        // then comes a blank line between the header and the body
        // then comes the body, size of which should be mentioned in the header
        response << "HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: " << body.str().size() << "\n\n" << body.str();
    }
    // CS ended already in if..else

    // sending back the response
    if(write(sock, response.str().c_str(), response.str().size()) == -1) log("error in sending response of DELETE");

    close(sock);

    // decrement the number of threads
    noOfThreads.fetch_sub(1);
}


/*****************************READER/WRITER ENDS********************/
