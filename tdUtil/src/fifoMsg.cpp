#include "fifoMsg.h"

FifoMsgSession::FifoMsgSession()
{
    ;
}
int FifoMsgSession::Create(std::string fifoName)
{
    mFifoName = "";
    mFifoName = mFifoName+"/tmp/"+fifoName;
    if(mFifoName.empty())
        return -1 ;
    if((mkfifo(mFifoName.c_str(),0777)<0)&&(errno!=EEXIST))
    {
        printf("cannot create fifoserver\n");
        return -1 ;
    }
    return 0 ;
}

int FifoMsgSession::Wait()
{
    if(mFifoName.empty())
        return -1 ;
    if(!isOpenR)
    {
        fd=open(mFifoName.c_str(),O_RDONLY);
        if(fd==-1)
        {
            printf("open %s for read error\n",mFifoName.c_str());
            return -1 ;
        }
        isOpenR = true ;
    }
    nread = 0 ;
    nread=read(fd,buff,2048);
    //printf("nread:%d\r\n",nread);
    if(nread==sizeof(FifoMsg))
    {
        return 0;
    }
    return -1;
}

int FifoMsgSession::OnceWrite(FIFOCmdType cmd,char*data,int size)
{
    FifoMsg msg;
    memset(&msg,0,sizeof(FifoMsg));
    msg.cmd = cmd ;
    memcpy(msg.content,data,size);
    if(!isOpenW)
    {
        fd=open(mFifoName.c_str(),O_WRONLY);
        if(fd==-1)
        {
            return -1 ;
        }
        isOpenW=true;
    }
    write(fd,(const void*)&msg,sizeof(FifoMsg));
    return 0 ;
}
FifoMsg *FifoMsgSession::GetFifoMsg()
{
    return (FifoMsg*)buff;
}

int FifoMsgSession::Close()
{
    if(fd!=-1)
    {
        close(fd);
    }
}

std::string FifoMsgSession::GetFifoName()
{
    return mFifoName ;
}