#pragma once
#pragma comment(lib, "Ws2_32.lib")

#ifndef LEMIGEO_IOCP_H
#define LEMIGEO_IOCP_H

#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>

using namespace std;

#define BUFSIZE 1024

typedef struct //user session
{
    SOCKET socket;
    SOCKADDR_IN addr;
} SESSION_DATA, *SESSION;
 
typedef struct // socket read buffer
{
    OVERLAPPED overlapped;
    char buffer[BUFSIZE];
    WSABUF wsaBuf;
} IO_DATA, *BUFFER;

void BeginReceive(SESSION session);
unsigned int __stdcall OnReceived(LPVOID pComPort);
void OnErrored(char *message);
void OnDisconnected(SESSION session);

void BeginReceive(SESSION session)
{
    BUFFER readBuffer;
    int recvBytes;
    int flags;

    readBuffer = (BUFFER)malloc(sizeof(IO_DATA));
    memset(&(readBuffer->overlapped), 0, sizeof(OVERLAPPED)); 
    readBuffer->wsaBuf.len = BUFSIZE;
    readBuffer->wsaBuf.buf = readBuffer->buffer;
    
    flags = 0;    
    WSARecv(session->socket,
            &(readBuffer->wsaBuf),
            1,
            (LPDWORD)&recvBytes,
            (LPDWORD)&flags,
            &(readBuffer->overlapped),
            NULL
            );    
}

unsigned int __stdcall OnReceived(LPVOID pComPort)
{
    HANDLE hPort =(HANDLE)pComPort;   
    DWORD recvBytes;
    SESSION session;
    BUFFER readBuffer;
    DWORD flags;
    ULONG_PTR ulKey = 0;
    while(1)
    {
        bool result = GetQueuedCompletionStatus(hPort,
                &recvBytes,
                (ULONG_PTR*)&session,
                (LPOVERLAPPED*)&readBuffer,
                INFINITE
                );
        
        if(!result)
        {
            OnErrored("failed to get completion status");
            continue;
        }

        //closed socket
        if(recvBytes == 0)
        {
            free(readBuffer);
            OnDisconnected(session);
            continue;
        }
        
        //echo
        printf("Recv[%s]\n",readBuffer->wsaBuf.buf);
        WSASend(session->socket, &(readBuffer->wsaBuf), 1, NULL, 0, NULL, NULL);

        // begin receive
        free(readBuffer);
        BeginReceive(session);
    }
    return 0;
}

void OnDisconnected(SESSION session)
{
    printf("Disconncted a session [ip %s]\n", inet_ntoa(session->addr.sin_addr));
    closesocket(session->socket);
    free(session);    
}

void OnErrored(char *message)
{
    printf("%s\n", message);
}
#endif // LEMIGEO_IOCP_H