#include "TdRtp.h"

uint32_t TdRtp::sendData(uint8_t * data,uint32_t size)
{
   int leftsize =  size;
   int dataPtrOffset = 0 ;
   while(leftsize>1280){
       sess.SendPacket((void*)data+dataPtrOffset,1280,96,false,10);
       dataPtrOffset+=1280;
       leftsize-=1280;
   }
   sess.SendPacket((void*)data+dataPtrOffset,leftsize,96,true,10);
   return 0 ;
}

int32_t TdRtp::setUp(std::string channel,std::string destIp,uint16_t destPort,uint16_t basePort){
 
	uint32_t lDestip = inet_addr(destIp.c_str());
	lDestip = ntohl(lDestip);
    lable = channel ;
	sessparams.SetOwnTimestampUnit(1.0/10.0);		
	
	sessparams.SetAcceptOwnPackets(true);
	transparams.SetPortbase(basePort);
    //transparams.SetRTCPMultiplexing(false);
	int status = sess.Create(sessparams,&transparams,RTPTransmitter::TransmissionProtocol::IPv4UDPProto);
    if(status<0)
    {
        spdlog::error("创建本地RTP会话失败");
        return -1 ;
    }	
	RTPIPv4Address addr(lDestip,destPort);
	
	status = sess.AddDestination(addr);
    if(status<0)
    {
        spdlog::error("连接远端RTP会话失败");
        return -2 ;
    }	
    sess.SetDefaultPayloadType(96);
    sess.SetDefaultMark(true);
    sess.SetDefaultTimestampIncrement(50);
    TdCodec::init(VideoH264);    
	return 0;
}
RTPUDPv4TransmissionParams* TdRtp::getTransparams()
{
    return &transparams ;
}
RTPSessionParams* TdRtp::getSessparams()
{
    return &sessparams;
}
RTPSession* TdRtp::getRTPSession()
{
    return &sess;
}

void TdRtp::dumpHex(uint8_t*data,int start,int end){
    for (int i = start;i<end;i++){
        printf("%02x ",data[i]);
    }
    printf("\r\n");
}


uint32_t TdRtp::start(uint32_t timeout){
    rtpRecvTh = std::thread(recvLoopThread,this);
    rtpSendTh = std::thread(sendLoopThread,this);
    
    if(lable.empty())
    {
        exit(1); //todo
    }
    fifoMsg = new FifoMsgSession();
    if(fifoMsg->Create(lable))
    {
        exit(0);
    }
    spdlog::debug("Rtp 启动成功\r\n");
    CodecParam *param = nullptr ;
    FifoMsg * ffMsgPtr=nullptr;
    while(1)
    {
        if(!fifoMsg->Wait())
        {
            ffMsgPtr = fifoMsg->GetFifoMsg();
            //printf("ffMsgPtr->cmd: %d \n",ffMsgPtr->cmd);
            if(ffMsgPtr->cmd = ModifyCodecParam)
            {
                param = (CodecParam*)ffMsgPtr->content;
                printf("get: %d %d %d %d \n",param->height,param->width,param->maxBitrate,param->rate);
                //printf("%s: FIFO read %s \n",fifoStr.c_str(),buf_r);
                setParam(*param); //设置编码参数 ;
            }
        }
    }
    return 0 ;
}


void TdRtp::recvLoopThread(TdRtp *rtp){
    int status ;
    rtp->isRun = true;
    bool isComing = false;
    YUVFrame_t* iFrame = nullptr;
    YUVFrame_t* oFrame = nullptr;
    RTPSession *sessPtr = rtp->getRTPSession();
    size_t len;
	uint8_t *loaddata;
	RTPPacket *pack;
    uint8_t *rtpOut=nullptr;
    uint8_t *obuff;
    int oLen = 0 ;
	int pos = 0;
    if(sessPtr==NULL)
    {
        spdlog::error("sess is NULL");
        return ;
    }
    while(rtp->isRun)
	{
	
        status = sessPtr->Poll();
		sessPtr->BeginDataAccess();
		// check incoming packets
        status = sessPtr->WaitForIncomingData(RTPTime(1,1),&isComing);
        // checkerror(status);
        if(isComing==true)
        {
            if (sessPtr->GotoFirstSourceWithData())
            {
                do
                {
                    RTPPacket *pack;
                    while ((rtp->isRun)&&((pack = sessPtr->GetNextPacket()) != NULL))
                    {
                        loaddata = pack->GetPayloadData();
					    len	= pack->GetPayloadLength();
                        if(pack->GetPayloadType() == 96) //H264
                        {
                            if(pack->HasMarker()) // the last packet
                            {
                                rtpOut = (uint8_t*)realloc(rtpOut,pos + len);
                                memcpy(rtpOut+pos, loaddata, len);
                                pos = pos + len;
                                rtp->parsingPs((int8_t*)rtpOut,pos,&obuff, &oLen);
                                //rtp->sendData(rtpOut,pos);
                                //printf("oLen:%d\r\n",oLen);
                                //rtp->dumpHex(obuff,0,5);
                                spdlog::debug("oLen:{} pos+len:{}",oLen,pos+len);
                                if(oLen>0)
                                {
                                    Packet frame(obuff,oLen); 
                                    rtp->push(frame);
                                }
                                if(rtpOut!=nullptr)
                                {
                                    free(rtpOut);
                                    rtpOut = nullptr;
                                }
                                pos = 0;
                            }
                            else
                            {
                                rtpOut = (uint8_t*)realloc(rtpOut,pos + len);
                                memcpy(rtpOut+pos, loaddata, len);
                                pos = pos + len;
                                //spdlog::debug("posc:%d\r\n",pos);
                            }
                        }else{
                            spdlog::info("!!!  GetPayloadType = %d !!!! ",pack->GetPayloadType());
                        }
                        sessPtr->DeletePacket(pack);
                    }
                } while ((rtp->isRun)&&(sessPtr->GotoNextSourceWithData()));
            }
        }
		
		sessPtr->EndDataAccess();
	}
    if(rtpOut!=nullptr)
    {
        free(rtpOut);
        rtpOut = nullptr;
    }
    sessPtr->EndDataAccess();
    printf("recvLoopThread stop\r\n");
    return ;
}


void TdRtp::sendLoopThread(TdRtp *rtp)
{
    rtp->isRun = true ;
    while(rtp->isRun){
        Packet frame = rtp->pop();
        if(!frame.empty())
        {
            uint8_t * codecOutDate = nullptr;
            uint32_t codecOutSize ;
            uint8_t *psOut = nullptr;
            int32_t psOutSize ;
            bool isKeyFrame ;
            rtp->convert(frame.data,frame.size,&codecOutDate,&codecOutSize,isKeyFrame);
            if((codecOutDate!=nullptr)&&(codecOutSize>0))
            {
                H264Stream_t h264_stream;
                memset(&h264_stream,0,sizeof(H264Stream_t));
                h264_stream.is_iframe = isKeyFrame;
                h264_stream.data = codecOutDate ;
                h264_stream.size = codecOutSize ;
                h264_stream.dts = rtp->pts ;
                h264_stream.pts = rtp->pts;
                rtp->pts+=3600;
                //printf("osize:%d iskeyframe:%d\r\n",codecOutSize,isKeyFrame);
                rtp->packagingPs(&h264_stream,&psOut,&psOutSize);
                if((psOut!=nullptr)&&(psOutSize>0))
                {
                    rtp->sendData(psOut,psOutSize);
                    ;
                }
                if(psOut!=nullptr)
                {
                    free(psOut);
                    psOut = nullptr;
                }
                free(codecOutDate);
                codecOutDate = nullptr;
            }
            //printf("frame len:%d\r\n",frame);
            
            frame.clear();
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    printf("sendLoopThread stop\r\n");
}

uint32_t TdRtp::stop()
{
    isRun = false;
    rtpRecvTh.join();
    rtpSendTh.join();
    sess.BYEDestroy(RTPTime(10,0),0,0);
    clear();
    TdCodec::destroy();
    spdlog::debug("=== rtp joined ===");
    return 0;
}