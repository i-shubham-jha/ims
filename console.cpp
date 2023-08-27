// this is the program which will use the server and run it
// we add a SIGINT handler for now
// will gracify later
#include "server/server.h"
#include <signal.h>

imsServer * ptr = NULL;

void handler(int dummy) // this var is needed
{
    ptr->stopServer();
    exit(1);
}

int main ()
{
    signal(SIGINT, handler);

    short PORT = 6969;
    std::string IP = "0.0.0.0";
    imsServer server( IP, PORT);
    ptr = &server;
    server.startServer();
}
