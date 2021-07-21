//
// Created by Administrator on 2020/2/12 0012.
//

#ifndef TDWLSDK_COMMON_H
#define TDWLSDK_COMMON_H
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "md5.h"
#include "uuid4.h"
#include "encryption.h"
#include "json11.hpp"

#define SIZE1024 1024
#define SIZE2048 2048
#define SIZE4096 4096
#define SIZE40960 40960

#define  IM_DATA_AES_KEY "Kel7Y3LDkMNJY1Qw"

const std::string splitTag ="td;&*%" ;
bool isCode0(std::string jsonData);
std::string& replaceAll(std::string& str, const std::string& old_value, const std::string& new_value);
int getCodeValue(std::string jsonData);
std::string getRand();
std::string getMsStr();
std::string getMd5(std::string Data);
long long string2longlong(std::string data);
int string2int(std::string data);

#endif //TDWLSDK_COMMON_H
