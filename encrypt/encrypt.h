#pragma once
#pragma comment(lib, "libeay32.lib")

#include <random>
#include <sstream>
//open ssl
#include <openssl/md5.h>
#include <openssl/des.h>
#include <openssl/aes.h>

using namespace std;

vector<unsigned char> GetMD5Hash(string input, int size)
{    
    vector<unsigned char> digest(size);
    MD5((unsigned char*) input.c_str(), input.size(), (unsigned char*) &digest[0]);
    return digest;
}

static const string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline bool IsBase64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

string BytesToBase64(unsigned char* input, int size) {
    string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    while (size--) {
        char_array_3[i++] = *(input++);
        if (i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for(i = 0; (i <4) ; i++)
            {
                ret += base64Chars[char_array_4[i]];
            }            
            i = 0;
        }
    }
    if (i)
    {
        for(j = i; j < 3; j++)
        {
            char_array_3[j] = '\0';
        } 
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
        for (j = 0; (j < i + 1); j++)
        {
            ret += base64Chars[char_array_4[j]];
        }      
        while((i++ < 3))
        {
            ret += '=';
        }
    }
    return ret;
}

vector<unsigned char> Base64ToBytes(string input)
{
    int size = input.size();
    int i = 0;
    int j = 0;
    int seq = 0;
    unsigned char char_array_4[4], char_array_3[3];
    vector<unsigned char> output;

    while (size-- && ( input[seq] != '=') && IsBase64(input[seq]))
    {
        char_array_4[i++] = input[seq];
        seq++;
        if (i ==4)
        {
            for (i = 0; i <4; i++)
            {
                char_array_4[i] = base64Chars.find(char_array_4[i]);
            }
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            for (i = 0; (i < 3); i++)
            {
                output.push_back(char_array_3[i]);
            }
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j <4; j++)
        {
            char_array_4[j] = 0;
        }            
        for (j = 0; j <4; j++)
        {
            char_array_4[j] = base64Chars.find(char_array_4[j]);
        }            
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        for (j = 0; (j < i - 1); j++)
        {
            output.push_back(char_array_3[j]);
        }
    }
    return output;
}

vector<unsigned char> DESEncrypt(char* key, char* data, int size)
{           
    DES_cblock block;
    DES_key_schedule ks;
    memcpy(block, key, 8);
    DES_set_odd_parity(&block);
    DES_set_key_checked(&block, &ks);
    int n = 0;
    vector<unsigned char> result(size);           
    DES_cfb64_encrypt((unsigned char *) data, (unsigned char *) &result[0],
                    size, &ks, &block, &n, DES_ENCRYPT);    
    return result;
}
vector<unsigned char> DESDecrypt(char* key, vector<unsigned char> input, int size)
{            
    DES_cblock block;
    DES_key_schedule ks;
    memcpy(block, key, 8);            
    DES_set_odd_parity(&block);
    DES_set_key_checked(&block, &ks);
    int n = 0;
    vector<unsigned char> result(size);            
    DES_cfb64_encrypt((unsigned char *) &input[0], (unsigned char *) &result[0],
                    size, &ks, &block, &n, DES_DECRYPT);    
    return result;
}

vector<unsigned char> AESEncrypt(unsigned char* key, size_t keySize, unsigned char *iv, unsigned char* input, size_t size)
{    
    const size_t outputSize = ((size + AES_BLOCK_SIZE) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
    AES_KEY aesKey;
    AES_set_encrypt_key(key, keySize, &aesKey);
    vector<unsigned char> result(outputSize); 
    AES_cbc_encrypt(input, &result[0], size, &aesKey, iv, AES_ENCRYPT);       
    return result;
}

vector<unsigned char> AESDecrypt(unsigned char* key, size_t keySize, unsigned char *iv, vector<unsigned char> input, size_t size)
{   
    AES_KEY aesKey;
    AES_set_decrypt_key(key, keySize, &aesKey);
    vector<unsigned char> result(size);
    AES_cbc_encrypt(&input[0], &result[0], size, &aesKey, iv, AES_DECRYPT);
    return result;
}