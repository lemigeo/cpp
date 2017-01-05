#pragma once

#ifndef LEMIGEO_ASIO_H
#define LEMIGEO_ASIO_H

#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include "protocol.h"

using namespace std;
using boost::asio::ip::tcp;

#define BUFSIZE 1024

class Session : public enable_shared_from_this<Session>
{
    public :
         Session(tcp::socket socket, Encoder* encoder, Decoder* decoder)
         :  socket(move(socket)),
            encoder(move(encoder)),
            decoder(move(decoder))
        {
        }
        ~Session()
        {
            free(encoder);
            free(decoder);
        }
        void Initialize()
        {
            printf("session initialized\n");
            BeginReceive();
        }
        void Send(vector<char> sendBytes)
        {
            auto self(shared_from_this());
            boost::asio::async_write(socket, boost::asio::buffer(sendBytes, sendBytes.size()),
            [this, self](boost::system::error_code ec, std::size_t size)
                {                    
                    if (!ec)
                    {
                        printf("sent a packet size(%zd)\n", size);
                    }
                    else
                    {
                        printf("send error %d %s\n", ec.value(), ec.message().c_str());
                        Disconnect();
                    }
                });
        }
    private :
        char buffer[BUFSIZE];
        tcp::socket socket;
        Encoder* encoder;
        Decoder* decoder;
        void BeginReceive()
        {
            auto self(shared_from_this());
            socket.async_read_some(boost::asio::buffer(buffer, BUFSIZE),
                [this, self](boost::system::error_code ec, size_t length)
                {
                    if (!ec)
                    {
                        vector<char> readBytes(length);                        
                        memcpy(&readBytes[0], buffer, length);
                        shared_ptr<Packet> p(make_shared<Packet>());
                        if(decoder->Decode(readBytes, p))
                        {
                            printf("received key %d, msg %s\n", p->GetKey(), p->GetMessage().c_str());
                            //echo                            
                            Send(encoder->Encode(p));
                        }
                        BeginReceive();
                    }
                    else
                    {
                        printf("receive error %d %s\n", ec.value(), ec.message().c_str());                        
                        Disconnect();
                    }
                });
        }
        void Disconnect()
        {
            if(socket.is_open())
            {
                socket.shutdown(tcp::socket::shutdown_both);
                socket.close();
            }
        }
};

class Server
{
    public :
        Server(boost::asio::io_service& io_service, short port)
        :   acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
            socket(io_service)
        {
            Start();
        }
    private:
        tcp::acceptor acceptor;
        tcp::socket socket;
        Encoder* encoder;
        Decoder* decoder;
        void Start()
        {
            encoder = new Encoder();
            decoder = new Decoder();
            printf("server started\n");            
            BeginAccept();
        }
        void BeginAccept()
        {
            acceptor.async_accept(socket, [this](boost::system::error_code ec)
            {
                if (!ec)
                {
                    //make_shared<Session>(std::move(socket))->Initialize();
                    shared_ptr<Session> session(make_shared<Session>(move(socket), move(encoder->Clone()), move(decoder->Clone())));                    
                    session->Initialize();      
                    printf("connected a new session\n");                    
                }
                else
                {
                    printf("accept error %d %s\n", ec.value(), ec.message().c_str());
                }
                BeginAccept();
            });
        }        
};

#endif // LEMIGEO_ASIO_H