#include "iocp.h"

int main(int argc, char *argv[])
{
    // initialize the winsock 2.2 dll
    WSADATA wsaData;    
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        OnErrored("failed to WSAStartup");
    }
    printf("Initailzed the winsock 2.2. dll\n");;

    //create completion port handle 
    HANDLE hPort;
    hPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
        
    //make completion port handles per cpu
    int cpuPerPort = 2;
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    for(auto i=0; i<sys_info.dwNumberOfProcessors * cpuPerPort; i++)
    {
        _beginthreadex(NULL, 0, OnReceived, (LPVOID)hPort, 0, NULL);
    }
    
    //make server socket
    SOCKET server;
    server = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);    
    short port = 9091;
    SOCKADDR_IN servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
    bind(server, (SOCKADDR*)&servAddr, sizeof(servAddr));
    listen(server, 4000);    
    printf("Listen\n");

    SESSION session;
    while(TRUE)
    { 
        SOCKET client;
        SOCKADDR_IN addr; 
        int addrLen=sizeof(addr);

        //connected a client
        client = accept(server, (SOCKADDR*)&addr, &addrLen);        
        //create session
        session = (SESSION)malloc(sizeof(SESSION_DATA));
        session->socket = client;
        memcpy(&(session->addr), &addr, addrLen);
        printf("Created a new session [ip %s]\n", inet_ntoa(session->addr.sin_addr));

        //connect a client socket to completion port
        CreateIoCompletionPort((HANDLE)client, hPort, (ULONG_PTR)session, 0);
        
        //begin receive
        BeginReceive(session);
    }

    return 0;
}
