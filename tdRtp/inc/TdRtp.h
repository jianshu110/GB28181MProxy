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
#include "inc/TdH264.h"
#include "inc/TdPs.h"
#include <fstream>
using namespace jrtplib;


#define CAMERA_SUPPORT_MAX      500
#define RTP_MAXBUF          4096
#define PS_BUF_SIZE         (1024*1024*4)


class TdRtp : public TdH264 ,public TdPs
{
private:
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;
    RTPSession sess;
    bool isRun = false;
    static void loopThread(TdRtp *rtp);
    /* data */
public:
    TdRtp(){
        fout = new std::ofstream("dumpPs.h264", std::ios::out | std::ios::binary);
    }
    ~TdRtp(){};
    void dumpHex(uint8_t*data,int start,int end);
    std::ofstream *fout ;
    RTPUDPv4TransmissionParams* getTransparams();
    RTPSessionParams* getSessparams();
    RTPSession* getRTPSession();
    int32_t setUp(std::string destIp,uint16_t destPort,uint16_t basePort);
    void h264CallBackUser(uint8_t*data,int size);
    uint32_t sendData(uint8_t * data,uint32_t size);
    void start(uint32_t time);
    void stop();
};

#endif