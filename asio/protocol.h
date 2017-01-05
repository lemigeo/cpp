#pragma once

#ifndef LEMIGEO_PROTOCOL_H
#define LEMIGEO_PROTOCOL_H

#include <iostream>

using namespace std;

class Packet
{
    public :
        Packet()
        {
        }
        Packet(int key, string msg)
        {
            this->key = key;
            this->message = msg;
        }
        void SetKey(int key)
        {
            this->key = key;
        }
        int GetKey()
        {
            return key;
        }
        void SetMessage(string msg)
        {
            this->message = msg;
        }
        string GetMessage()
        {
            return message;
        }
    private :
        int key;
        string message;
};

class Encoder
{
    public :
        Encoder() : Encoder(6)
        {            
        }
        Encoder(int size)
        {
            headerSize = size;
        }
        vector<char> Encode(shared_ptr<Packet> p)
        {
            short length = p->GetMessage().size();                        
            int key = p->GetKey();
            vector<char> writeBytes(headerSize + length);           
            memcpy(&writeBytes[0], &length, sizeof(short));
            memcpy(&writeBytes[0] + 2, &key, sizeof(int));
            memcpy(&writeBytes[0] + 6, &p->GetMessage()[0], length);
            return writeBytes;
        }
        Encoder* Clone()
        {
            return new Encoder(headerSize);
        }
    private :
        int headerSize;
};

class Decoder
{
    public :
        Decoder() : Decoder(6)
        {
        }
        Decoder(int size)
        {
            headerSize = size;
            leftHeaderBytes = size;
            header = vector<char>(size);
        }
        bool Decode(vector<char> readBytes, shared_ptr<Packet> p)
        {
            int offset = 0;            
            int size = readBytes.size();
            while(size > 0)
            {
                if(!isReadHeader)
                {
                    int headerBytesLength = min(leftHeaderBytes, size);
                    memcpy(&header[0] + headerOffset, &readBytes[0], headerBytesLength);
                    headerOffset += headerBytesLength;
                    leftHeaderBytes -= headerBytesLength;
                    if(leftHeaderBytes > 0)
                    {
                        return false;
                    }
                    totalLength = *reinterpret_cast<const uint16_t*>(&header[0]);                    
                    if(totalLength <= 0)
                    {
                        Clear();
                        return false;
                    }
                    packet = vector<char>(totalLength);
                    size -= headerBytesLength;
                    offset += headerBytesLength;
                    leftReadBytes = totalLength;
                    isReadHeader = true;
                }
                int packetBytesLength = min(leftReadBytes, size);                
                memcpy(&packet[0] +(totalLength - leftReadBytes), &readBytes[0] + offset, packetBytesLength);                
                leftReadBytes -= packetBytesLength;
                if(leftReadBytes == 0)
                {
                    int key = *reinterpret_cast<const uint32_t*>(&header[0] + 2);
                    string message(packet.begin(), packet.end());
                    p->SetKey(key);
                    p->SetMessage(message);                    
                    Clear();
                    return true;
                }
                size -= packetBytesLength;
                offset += packetBytesLength;
            }            
            return false;
        }
        void Clear()
        {
            leftHeaderBytes = headerSize;
            headerOffset = 0;
            leftReadBytes = 0;
            totalLength = 0;
            isReadHeader = false;
            header.shrink_to_fit();
            packet.shrink_to_fit();
        }
        Decoder* Clone()
        {
            return new Decoder(headerSize);
        }
    private :
        int headerSize;
        vector<char> header;
        int leftHeaderBytes = 0;
        int headerOffset = 0;
        int leftReadBytes = 0;
        short totalLength = 0;
        bool isReadHeader = false;
        vector<char> packet;
};

#endif // LEMIGEO_PROTOCOL_H