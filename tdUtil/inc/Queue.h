#ifndef _QUEUE_H_
#define _QUEUE_H_
#include <queue>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


typedef struct Packet_s{
    Packet_s(uint8_t * Data,uint32_t Size):data(Data),size(Size){;};
    uint8_t * data ;
    uint32_t size ;
    bool empty(){
        if(size==0)
        {
            return true ;
        }
        return false ;
    }
    void clear(){
        if(data!=nullptr)
        {
            free(data);
            data=nullptr;
            size =0 ;
        }
    }
}Packet;

class Queue
{
private:
    /* data */
    std::queue <Packet> packetQue ;
    std::queue <Packet> outPacketQue ;
    std::queue <Packet> *inPacketQue ;
    bool mQueAccess = true;
    pthread_mutex_t queMutex;
public:

    Queue(/* args */){pthread_mutex_init(&queMutex,NULL);};
    ~Queue(){pthread_mutex_destroy(&queMutex);printf("%s\r\n",__func__);};
    bool isAccess(){
        return mQueAccess ;
    }
    void setAccess(bool status){
        mQueAccess = status ;
    }
    Packet pop()
    {
        Packet packet(nullptr,0);
        pthread_mutex_lock(&queMutex);
        if(!packetQue.empty()){
            packet=packetQue.front();
            packetQue.pop();
        }
        pthread_mutex_unlock(&queMutex);
        return packet ;
    }
    void push(Packet packet)
    {
        if(!isAccess())
        {
            //队列状态为false 数据直接丢掉
            return  ;
        }
        pthread_mutex_lock(&queMutex);
        if(packetQue.size()<128){
            packetQue.push(packet);
        }
        pthread_mutex_unlock(&queMutex);
        return  ;
    }
    void clear(){
        pthread_mutex_lock(&queMutex);
        while (!packetQue.empty()) 
        {
            Packet frame = packetQue.front();
            frame.clear();
            packetQue.pop();
        }
        pthread_mutex_unlock(&queMutex);
    }

    static int link(Queue &src,Queue &dest);
};


#endif