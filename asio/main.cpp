#include <iostream>
#include "asio.h"

using namespace std;

int main(int argc, char *argv[])
{
    short port = 9091;
    boost::asio::io_service io_service;    
    Server s(io_service, port);
    io_service.run();    
    return 0;
}