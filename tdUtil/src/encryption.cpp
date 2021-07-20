#include "../inc/encryption.h"
#include "../inc/AES.h"
#include "../inc/base64.h"
#include <iostream>
using namespace std;
/**
 * cbc方式加密
 * @param data
 * @param secretKey
 * @param iv
 * @return
 */
std::string encryptByAES(const char * data, const char* secretKey, const char* iv, int iMode) {
    std::string data_str(data);
    size_t length = data_str.length();
    int block_num = length / BLOCK_SIZE + 1;
    //明文
    char* szDataIn = new char[block_num * BLOCK_SIZE + 1];
    memset(szDataIn, 0x00, block_num * BLOCK_SIZE + 1);
    strcpy(szDataIn, data_str.c_str());

    //进行PKCS7Padding填充。
    int k = length % BLOCK_SIZE;
    int j = length / BLOCK_SIZE;
    int padding = BLOCK_SIZE - k;
    for (int i = 0; i < padding; i++)
    {
        szDataIn[j * BLOCK_SIZE + k + i] = padding;
    }
    szDataIn[block_num * BLOCK_SIZE] = '\0';

    //加密后的密文
    char* szDataOut = new char[block_num * BLOCK_SIZE + 1];
    memset(szDataOut, 0, block_num * BLOCK_SIZE + 1);

    //进行进行AES的CBC模式加密
    AES aes;
    aes.MakeKey(secretKey, iv, 16, 16);
    aes.Encrypt(szDataIn, szDataOut, block_num * BLOCK_SIZE,0);
	//printf("aa:: %02x %02x %d\r\n",szDataOut[0],szDataOut[1],block_num * BLOCK_SIZE);
    std::string str = base64_encode((unsigned char*)szDataOut,
                               block_num * BLOCK_SIZE);
    delete[] szDataIn;
    delete[] szDataOut;
    return str;
}

/**
 * cbc方式解密
 * @param data
 * @param secretKey
 * @param iv
 * @return
 */
std::string decryptByAES(const char * data, const char* secretKey, const char* iv, int iMode) {
    std::string data_str(data);
    std::string strData = base64_decode(data_str);
    size_t length = strData.length();
    //密文
    char* szDataIn = new char[length + 1];
    memcpy(szDataIn, strData.c_str(), length + 1);
    //明文
    char* szDataOut = new char[length + 1];
    memcpy(szDataOut, strData.c_str(), length + 1);

    //进行AES的CBC模式解密
    AES aes;
    aes.MakeKey(secretKey, iv, 16, 16);
    aes.Decrypt(szDataIn, szDataOut, length, iMode);

    //去PKCS7Padding填充
    if ((0x00 < szDataOut[length - 1])&&(szDataOut[length - 1] <= 0x16))
    {
        int tmp = szDataOut[length - 1];
        for (int i = length - 1; i >= (int)length - tmp; i--)
        {
            if (szDataOut[i] != tmp)
            {
                memset(szDataOut, 0, length);
                cout << "去填充失败！解密出错！！" << endl;
                break;
            }
            else
                szDataOut[i] = 0;
        }
    }
    std::string strDest(szDataOut);
    delete[] szDataIn;
    delete[] szDataOut;
    return strDest;

}
