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
void TdRtp::h264CallBackUser(uint8_t*data,int size){
    //int status ;
    uint8_t *outData = nullptr;
    int outlength =0 ;
    if((data!=nullptr)&&(size!=0)){
        H264Stream_t h264_stream;
        memset(&h264_stream,0,sizeof(H264Stream_t));
        
        if(data[4]==0x67){
            
            h264_stream.is_iframe = 1;
            h264_stream.data = data ;
            h264_stream.size = size ;
            h264_stream.dts = pts ;
            h264_stream.pts = pts;
            pts+=3600;
            packagingPs(&h264_stream,&outData, &outlength);
            sendData(outData,outlength);
            //dumpHex(outData,0,10);
            //printf("rtp outlength:%d\r\n",outlength);
            //psfout->write((const char*)outData,outlength);
            free(outData);
            outData = nullptr;
        }
        else{
            h264_stream.is_iframe = 0;
            h264_stream.data = data ;
            h264_stream.size = size ;
            h264_stream.dts = pts ;
            h264_stream.pts = pts;
            pts+=3600;
            packagingPs(&h264_stream,&outData, &outlength);
            sendData(outData,outlength);
            //dumpHex(outData,0,10);
            //printf("rtp outlength:%d\r\n",outlength);
            //psfout->write((const char*)outData,outlength);
            free(outData);
            outData = nullptr;
        } 
    }
}

int32_t TdRtp::setUp(std::string destIp,uint16_t destPort,uint16_t basePort){
 
	uint32_t lDestip = inet_addr(destIp.c_str());
	lDestip = ntohl(lDestip);

	sessparams.SetOwnTimestampUnit(1.0/10.0);		
	
	sessparams.SetAcceptOwnPackets(true);
	transparams.SetPortbase(basePort);
    transparams.SetRTCPMultiplexing(false);
	int status = sess.Create(sessparams,&transparams);
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
    TdH264::decoderSetUp();
    TdH264::encoderSetUp();
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

uint32_t TdRtp::start(uint32_t time){
    rtpTh = std::thread(loopThread,this);
    //t.detach();
    spdlog::debug("Rtp 启动成功\r\n");
    return 0 ;
}
// int startTeg = 0 ;
// int count = 0;
void TdRtp::loopThread(TdRtp *rtp){
    int status ;
    rtp->isRun = true;
    bool i=false;
    YUVFrame_t* iFrame = nullptr;
    YUVFrame_t* oFrame = nullptr;
    RTPSession *sessPtr = rtp->getRTPSession();
    size_t len;
	uint8_t *loaddata;
	RTPPacket *pack;
	uint8_t buff[1024*1000] = {0};
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
        status = sessPtr->WaitForIncomingData(RTPTime(1,1),&i);
        // checkerror(status);
        if(i==true)
        {
            if (sessPtr->GotoFirstSourceWithData())
            {
                do
                {
                    RTPPacket *pack;
                    while ((pack = sessPtr->GetNextPacket()) != NULL)
                    {
                        loaddata = pack->GetPayloadData();
					    len	= pack->GetPayloadLength();
                        if(pack->GetPayloadType() == 96) //H264
                        {
                            if(pack->HasMarker()) // the last packet
                            {
                                memcpy(&buff[pos],loaddata,len);
                                if(buff[3]!=0xba){
                                    pos = 0;
                                    continue;
                                }
                                rtp->parsingPs((int8_t*)buff,pos+len,&obuff, &oLen);
                                //rtp->dumpHex(obuff,0,5);
                                spdlog::debug("oLen:%d pos+len:%d",oLen,pos+len);

                                if(rtp->startTeg==0){
                                    if(obuff[4]==0x67)
                                    {
                                        rtp->startTeg = 1;
                                        FramePacket *frame = new FramePacket(obuff,oLen);
                                        rtp->pushFrameToQue(frame);
                                    }
                                }
                                else{
                                    FramePacket *frame = new FramePacket(obuff,oLen);
                                    rtp->pushFrameToQue(frame);
                                }
                                pos = 0;
                            }
                            else
                            {
                                memcpy(&buff[pos],loaddata,len);
                                pos = pos + len;
                                spdlog::debug("posc:%d\r\n",pos);
                            }
                        }else{
                            spdlog::info("!!!  GetPayloadType = %d !!!! ",pack->GetPayloadType());
                        }
                        sessPtr->DeletePacket(pack);
                    }
                } while (sessPtr->GotoNextSourceWithData());
            }
        }
		
		sessPtr->EndDataAccess();
	}
    return ;
}

uint32_t TdRtp::stop()
{
    isRun = false;
    rtpTh.join();
    sess.BYEDestroy(RTPTime(10,0),0,0);
    spdlog::debug("=== rtp joined ===");
    TdH264::destory();
    return 0;
}