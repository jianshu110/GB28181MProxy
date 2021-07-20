
#ifndef _ENCYPTION_
#define _ENCYPTION_
#include <string>

std::string encryptByAES(const char* data, const char* secretKey, const char* iv, int iMode);
std::string decryptByAES(const char* data, const char* secretKey, const char* iv, int iMode);
int getModeByName(const char * iModeName);

#endif

