
#include <iostream>
#include <string>
#include "encrypt.h"

using namespace std;

int main(int argc, char* argv[])
{
    //MD5 hash
    string input = "0123456789";
    int length = input.size();
    vector<unsigned char> digest = GetMD5Hash(input, length);
    string output = BytesToBase64(digest.data(), length);    
    printf("MD5 result : %s\n", output.c_str());
    
    //DES    
    char key[] = "password";    
    char data[] = "test message";
    int size = sizeof(data);
    string desEncrypted = BytesToBase64(DESEncrypt(key, data, size).data(), size);
    printf("DES encryption result : %s\n", desEncrypted.c_str());
    vector<unsigned char> desBytes = Base64ToBytes(desEncrypted);
    vector<unsigned char> desDecrypted = DESDecrypt(key, desBytes, size);
    printf("DES decrypttion result : %s\n", desDecrypted.data());    

    //AES
    unsigned char aesKey[] = "0123456789012345678901234567890123456789";
    unsigned char enIV[AES_BLOCK_SIZE] = "012345678901234";
    unsigned char deIV[AES_BLOCK_SIZE] = "012345678901234";
    unsigned char text[] = "test message";
    auto textSize = sizeof(text);
    size_t keySize = 128;
    vector<unsigned char> aesEncrypted = AESEncrypt(aesKey, keySize, enIV, text, textSize);    
    printf("AES128 encryption result : %s\n", BytesToBase64(aesEncrypted.data(), textSize).c_str());    
    vector<unsigned char> aesDecrypted = AESDecrypt(aesKey, keySize, deIV, aesEncrypted, textSize);
    printf("AES128 decryption result : %s\n", aesDecrypted.data());
  
    string line;
    cin >> line;
}