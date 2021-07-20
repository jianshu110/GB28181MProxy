#include "TdH264.h"
#include<cmath>
int32_t TdH264::decoderSetUp()
{
    long rv = WelsCreateDecoder (&decoder_);
    if(decoder_ != NULL);
    if (decoder_ == NULL) 
    {
        return (int32_t)rv;
    }
    memset (&decParam, 0, sizeof (SDecodingParam));
    decParam.uiTargetDqLayer = UCHAR_MAX;
    decParam.eEcActiveIdc = ERROR_CON_SLICE_COPY;
    //decParam.eEcActiveIdc = ERROR_CON_FRAME_COPY_CROSS_IDR;
    decParam.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_DEFAULT;
    rv = decoder_->Initialize (&decParam);
    return (int32_t)rv;
}

int32_t TdH264::encoderSetUp(){
    int rv = WelsCreateSVCEncoder (&encoder_);
    if(rv){
        return -1 ;
    }
    unsigned int uiTraceLevel = WELS_LOG_ERROR;
    encoder_->SetOption (ENCODER_OPTION_TRACE_LEVEL, &uiTraceLevel);
    timestamp_ = 0;
    return 0 ;
}
FramePacket* TdH264::newFramePacket(uint8_t*data,uint32_t size)
{
    FramePacket* frame = new FramePacket(data,size);
    return frame ;
}
void TdH264::DelFramePacket(FramePacket*frame)
{
    if(frame!=nullptr){
        if(frame->data!=nullptr){
            free(frame->data);
            frame->data=nullptr;
        }
        delete frame ;
    }
}
 FramePacket *TdH264::popFrameFromQue()
 {
    FramePacket * frame=nullptr;
    pthread_mutex_lock(&queMutex);
    if(!frameQue.empty()){
        frame=frameQue.front();
        frameQue.pop();
    }
    pthread_mutex_unlock(&queMutex);
    return frame ;
 }
int32_t TdH264::pushFrameToQue(FramePacket * frame)
{
    pthread_mutex_lock(&queMutex);
    if(frameQue.size()<128){
         frameQue.push(frame);
    }
    pthread_mutex_unlock(&queMutex);
    return 0 ;
}
int32_t TdH264::setEncoderParam(int width,int height,int bitrate,int iMaxBitrate,int fps)
{
    memset(&encParam,0,sizeof(encParam));
    encoder_->GetDefaultParams(&encParam);
    encParam.iUsageType = CAMERA_VIDEO_REAL_TIME;
    encParam.fMaxFrameRate = fps;
    encParam.iPicWidth = width;
    encParam.iPicHeight = height;
    encParam.iTargetBitrate = bitrate;
    encParam.iRCMode = RC_QUALITY_MODE;
    encParam.iTemporalLayerNum = 1;
    encParam.iSpatialLayerNum = 1;
    encParam.bEnableDenoise = false;
    encParam.bEnableBackgroundDetection = true;
    encParam.bEnableAdaptiveQuant = false;
    encParam.bEnableFrameSkip = false;
    encParam.bEnableLongTermReference = false;
    encParam.uiIntraPeriod = fps;
    encParam.eSpsPpsIdStrategy = CONSTANT_ID;
    encParam.bPrefixNalAddingCtrl = false;
    encParam.sSpatialLayers[0].iVideoWidth = width;
    encParam.sSpatialLayers[0].iVideoHeight = height;
    encParam.sSpatialLayers[0].fFrameRate = fps;
    encParam.sSpatialLayers[0].iSpatialBitrate = bitrate;
    encParam.sSpatialLayers[0].iMaxSpatialBitrate =iMaxBitrate;
    encoder_->InitializeExt (&encParam);
    mIsEncoderSetUp = true;
    return 0 ;
}
void TdH264::setPicture(int width,int height,int Ystride,int UVstride)
{
    memset(&picture_, 0, sizeof(SSourcePicture));
    picture_.iPicHeight = height ;
    picture_.iPicWidth = width ;
    picture_.iColorFormat = videoFormatI420;
    picture_.iStride[0] = Ystride;
    picture_.iStride[1] = UVstride;
    //printf("%d %d\r\n",picture_.iStride[0],picture_.iStride[1]);
}
int32_t TdH264::encode(uint8_t* Ydata,uint8_t* Udata,uint8_t* Vdata,uint8_t* pkt,size_t& pkt_size,bool& is_keyframe,bool& got_output){
    got_output = false;
	pkt_size = 0;
	if (!encoder_)
	{
		return false;
	}
	picture_.uiTimeStamp =timestamp_++; 
    picture_.pData[0] = (unsigned char*)Ydata;
    picture_.pData[1] = (unsigned char*)Udata;
    picture_.pData[2] = (unsigned char*)Vdata;

	int iFrameSize = 0;
    uint32_t temporal_id = 0;

	SFrameBSInfo encoded_frame_info;

	int err = encoder_->EncodeFrame(&picture_, &encoded_frame_info);
	if (err) {
        printf("enCodec error\r\n");
		return 0;
	}

	if (encoded_frame_info.eFrameType == videoFrameTypeInvalid) {
        printf("enCodec videoFrameTypeInvalid\r\n");
		return 0;
	}

	if (encoded_frame_info.eFrameType != videoFrameTypeSkip) {
		int iLayer = 0;
        
		while (iLayer < encoded_frame_info.iLayerNum) {
			SLayerBSInfo* pLayerBsInfo = &(encoded_frame_info.sLayerInfo[iLayer]);
			if (pLayerBsInfo != NULL) {
				int iLayerSize = 0;
				temporal_id = pLayerBsInfo->uiTemporalId;
				int iNalIdx = pLayerBsInfo->iNalCount - 1;
				do {
					iLayerSize += pLayerBsInfo->pNalLengthInByte[iNalIdx];
					--iNalIdx;
				} while (iNalIdx >= 0);
				memcpy(pkt + iFrameSize, pLayerBsInfo->pBsBuf, iLayerSize);
				iFrameSize += iLayerSize;
			}
			++iLayer;
		}
		got_output = true;
	}
	else {
		printf("!!!!videoFrameTypeSkip---!\n");
		is_keyframe = false;
	}
    //printf("FrameType: %d FrameSizeInBytes: %d LayerNum: %d\r\n",encoded_frame_info.eFrameType,encoded_frame_info.iFrameSizeInBytes,encoded_frame_info.iLayerNum);
	if (iFrameSize > 0)
	{
		pkt_size = iFrameSize;

		EVideoFrameType ft_temp = encoded_frame_info.eFrameType;
		if (ft_temp == 1 || ft_temp == 2)
		{
			is_keyframe = true;
		}
		else if (ft_temp == 3)
		{
			is_keyframe = false;
		}
		else
		{
			is_keyframe = false;
		}
	}

	return iFrameSize;
}
int save_tag = 0 ;
int32_t TdH264::decode(uint8_t * inData,uint32_t inSize)
{
    if((inData==nullptr))
    {
        return -1;
    }
    uint8_t *oData[3];
    SBufferInfo obufInfo ;
    int status;
    int src_width,src_height,src_width_uv,src_height_uv,src_stride_y,src_stride_uv;
    int dst_width,dst_height,dst_width_uv,dst_height_uv,dst_stride_y,dst_stride_uv;
    int64_t dst_y_plane_size,dst_uv_plane_size;
    int64_t src_y_plane_size,src_uv_plane_size;
    memset (&obufInfo, 0, sizeof (SBufferInfo));
    DECODING_STATE rv = decoder_->DecodeFrame2 (inData, (int) inSize, oData, &obufInfo);
    if (obufInfo.iBufferStatus == 1) 
    {  
        if(obufInfo.UsrData.sSystemBuffer.iFormat==videoFormatI420){

            //rtpfout->write((const char*)inData,inSize);

            src_height = obufInfo.UsrData.sSystemBuffer.iHeight;
            src_width = obufInfo.UsrData.sSystemBuffer.iWidth;

        
            src_width_uv = (abs(src_width) + 1) >> 1;
            src_height_uv = (abs(src_height) + 1) >> 1;

            src_y_plane_size = obufInfo.UsrData.sSystemBuffer.iStride[0]*src_height;
            src_uv_plane_size = obufInfo.UsrData.sSystemBuffer.iStride[1]*src_height;

            // yuvfout->write((const char*)oData[0],src_y_plane_size);
            // yuvfout->write((const char*)oData[1],src_uv_plane_size);
            // yuvfout->write((const char*)oData[2],src_uv_plane_size);
            // yuvfout->flush();



            dst_height = (abs(src_height) + 1) >> 1;
            dst_width = (abs(src_width) + 1) >> 1;

            dst_width_uv = (abs(dst_width) + 1) >> 1;
            dst_height_uv = (abs(dst_height) + 1) >> 1;

        
            dst_stride_y = dst_width;
            dst_stride_uv = dst_width_uv;


            dst_y_plane_size = (dst_width) * (dst_height);
            dst_uv_plane_size = (dst_width_uv) * (dst_height_uv);

            align_buffer_page_end(dst_y_c,dst_y_plane_size);
            align_buffer_page_end(dst_u_c,dst_uv_plane_size);
            align_buffer_page_end(dst_v_c,dst_uv_plane_size);


            status = libyuv::I420Scale(oData[0], obufInfo.UsrData.sSystemBuffer.iStride[0],oData[1], 
                obufInfo.UsrData.sSystemBuffer.iStride[1], oData[2],
                obufInfo.UsrData.sSystemBuffer.iStride[1],
                src_width, src_height, dst_y_c, dst_stride_y, dst_u_c,
                dst_stride_uv, dst_v_c, dst_stride_uv, dst_width, dst_height,libyuv::kFilterNone);
            if(status){
                return -1 ;
            }
            //printf("src: %d(%d)*%d dst: %d*%d \r\n",src_width,obufInfo.UsrData.sSystemBuffer.iStride[0],src_height,dst_width,dst_height);

            // yuvScalefout->write((const char*)dst_y_c,dst_y_plane_size);
            // yuvScalefout->write((const char*)dst_u_c,dst_uv_plane_size);
            // yuvScalefout->write((const char*)dst_v_c,dst_uv_plane_size);
            // yuvScalefout->flush();

            uint8_t* pkt = new uint8_t[1024*1024*5];
            size_t pkt_size = 0 ;
            bool is_keyframe=false ,got_output=false;

            //printf("status:%d  %d %d %d %d \r\n",status,dst_width,dst_height,dst_stride_uv,dst_stride_y);
            if(!isEncoderSetUp()){
                setEncoderParam(dst_width,dst_height,20000,30000,25);
            }
            setPicture(dst_width,dst_height,dst_stride_y,dst_stride_uv);
            encode(dst_y_c,dst_u_c,dst_v_c,pkt,pkt_size,is_keyframe,got_output);
     
            if(got_output)
            {
                h264fout->write((const char*)pkt,pkt_size);
                h264CallBackUser(pkt,pkt_size);
            }
            free(pkt);
            free_aligned_buffer_page_end(dst_y_c);
            free_aligned_buffer_page_end(dst_u_c);
            free_aligned_buffer_page_end(dst_v_c);
        }
    }
    return 0 ;
}

void TdH264::loopThread(TdH264 *h264){
    h264->loopThrStatus =1;
    while(h264->loopThrStatus==1)
    {
        FramePacket * frame = h264->popFrameFromQue();
        if(frame!=nullptr)
        {
            h264->decode(frame->data,frame->size);
            free(frame->data);
            frame->data=nullptr;
            free(frame);
        }
        usleep(1000);
    }
}