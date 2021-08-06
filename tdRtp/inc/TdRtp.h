#ifndef _TDRTP_H_
#define _TDRTP_H_ 
#include "rtpsession.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtppacket.h"
#include "rtplibraryversion.h"
#include "spdlog/spdlog.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>
#include "inc/TdCodec.h"
#include "inc/TdPs.h"
#include <fstream>
#include "inc/Queue.h"
#include <chrono>
#include <inc/fifoMsg.h>

using namespace jrtplib;
typedef void(*closeOnCallBack)(void* argv);

#define CAMERA_SUPPORT_MAX      500
#define RTP_MAXBUF          4096
#define PS_BUF_SIZE         (1024*1024*4)

class TdRtp :public TdPs,public TdCodec,public Queue
{
private:
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;
    RTPSession sess;
    std::string lable;
    bool isRun = false;
    static void recvLoopThread(TdRtp *rtp);
    static void sendLoopThread(TdRtp *rtp);
    std::thread rtpSendTh ;
    std::thread rtpRecvTh ;
    int startTeg = 0 ;
    closeOnCallBack mCloseOnCallBack ;
    FifoMsgSession * fifoMsg ;
    //Queue frameQue;
    /* data */
public:
    TdRtp(){
        //fout = new std::ofstream("dumpPs.h264", std::ios::out | std::ios::binary);
        ;
    }
    ~TdRtp(){printf("%s\r\n",__func__);};
    void dumpHex(uint8_t*data,int start,int end);
    std::ofstream *fout ;
    RTPUDPv4TransmissionParams* getTransparams();
    RTPSessionParams* getSessparams();
    RTPSession* getRTPSession();
    int32_t setUp(std::string channel,std::string destIp,uint16_t destPort,uint16_t basePort);
    //void h264CallBackUser(uint8_t*data,int size);
    uint32_t sendData(uint8_t * data,uint32_t size);
    uint32_t start(uint32_t timeout);
    uint32_t stop();
};

#endif