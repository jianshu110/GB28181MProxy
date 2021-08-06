#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
 #include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>

enum FIFOCmdType{
    ModifyCodecParam=2,
};

struct FifoMsg
{
    FIFOCmdType cmd ;
    char content[1024] ;
};


class FifoMsgSession {
private:
    std::string mFifoName ;
    int fd = -1;
    char buff[2048];
    int nread = 0 ;
    int isOpenW =false;
    int isOpenR = false;
public:
    FifoMsgSession();
    int Create(std::string mFifoName);
    int Wait();
    int OnceWrite(FIFOCmdType cmd,char*data,int size);
    FifoMsg *GetFifoMsg();
    int Close();
    std::string GetFifoName();
};