#include "TdPs.h"


uint32_t TdPs::ProgramStreamPackHeader(char* Pack, int length, char **NextPack, int *leftlength) 
{
        //printf("[%s]%x %x %x %x\n", __FUNCTION__, Pack[0], Pack[1], Pack[2], Pack[3]);
        //通过 00 00 01 ba头的第14个字节的最后3位来确定头部填充了多少字节
        program_stream_pack_header *PsHead = (program_stream_pack_header *)Pack;
        unsigned char pack_stuffing_length = PsHead->stuffinglen & '\x07';
        //printf("pack_stuffing_length:%d\r\n",pack_stuffing_length);
        //填充为0
        // printf("program_stream_pack_header:%d\r\n",sizeof(program_stream_pack_header));
        // exit(0);
        *leftlength = length - sizeof(program_stream_pack_header) - pack_stuffing_length;//减去头和填充的字节
        *NextPack = Pack+sizeof(program_stream_pack_header) + pack_stuffing_length;
        if(*leftlength<4) return 0;
        return *leftlength;
}
uint32_t TdPs::ProgramStreamMap(char* Pack, int length, char **NextPack, int *leftlength, char **PayloadData, int *PayloadDataLen)
{
    program_stream_map* PSMPack = (program_stream_map*)Pack;

    //no payload
    *PayloadData = 0;
    *PayloadDataLen = 0;
    
    if((unsigned int)length < sizeof(program_stream_map)) return 0;

    littel_endian_size psm_length;
    psm_length.byte[0] = PSMPack->PackLength.byte[1];
    psm_length.byte[1] = PSMPack->PackLength.byte[0];

    //printf("psm_length.length:%d %02x %02x\r\n",psm_length.length,psm_length.byte[0],psm_length.byte[1]);

    *leftlength = length - psm_length.length - sizeof(program_stream_map);
    if(*leftlength<=0) return 0;

    *NextPack = Pack + psm_length.length + sizeof(program_stream_map);
    //printf("%02x %02x\r\n",(uint8_t)Pack[3],(uint8_t)*NextPack[0]);
    return *leftlength;
}

uint32_t TdPs::ProgramShHead(char* Pack, int length, char **NextPack, int *leftlength, char **PayloadData, int *PayloadDataLen) {
    program_stream_map* PSMPack = (program_stream_map*)Pack;

    //no payload
    *PayloadData = 0;
    *PayloadDataLen = 0;
    
    if((unsigned int)length < sizeof(program_stream_map)) return 0;

    littel_endian_size psm_length;
    psm_length.byte[0] = PSMPack->PackLength.byte[1];
    psm_length.byte[1] = PSMPack->PackLength.byte[0];

    *leftlength = length - psm_length.length - sizeof(program_stream_map);
    if(*leftlength<=0) return 0;

    *NextPack = Pack + psm_length.length + sizeof(program_stream_map);

    return *leftlength;
}

uint32_t TdPs::Pes(char* Pack, int length, char **NextPack, int *leftlength, char **PayloadData, int *PayloadDataLen)
{
    program_stream_e* PSEPack = (program_stream_e*)Pack;

    *PayloadData = 0;
    *PayloadDataLen = 0;
    //dumpHex((uint8_t*)Pack,0,30);
    if((unsigned int)length < sizeof(program_stream_e)) return 0;
    
    //printf("stuffing_length: %02x\r\n",PSEPack->stuffing_length);

    littel_endian_size pse_length;
    pse_length.byte[0] = PSEPack->PackLength.byte[1];
    pse_length.byte[1] = PSEPack->PackLength.byte[0];
    
    
    *PayloadDataLen = pse_length.length - 2 - 1 - PSEPack->stuffing_length;
    if(*PayloadDataLen>0)
        *PayloadData = Pack + sizeof(program_stream_e) + PSEPack->stuffing_length;

    *leftlength = length - pse_length.length - sizeof(pack_start_code) - sizeof(littel_endian_size);
    if(*leftlength<=0) return 0;

    *NextPack = Pack + sizeof(pack_start_code) + sizeof(littel_endian_size) + pse_length.length;

    return *leftlength;
}

uint32_t TdPs::parsingPs(int8_t*srcdata,int length,uint8_t **outData, int *outlength)
{
     int leftlength = 0;
    char *NextPack = 0;
    *outlength = 0;
    *outData = nullptr;
     //printf("sizestruct:%d %d %d %d\r\n",sizeof(program_stream_e),sizeof(program_stream_e1),sizeof(pack_start_code),sizeof(littel_endian_size));
    if(ProgramStreamPackHeader((char*)srcdata, length, &NextPack, &leftlength)==0)
        return 0;
    char *PayloadData=NULL; 
    int PayloadDataLen=0;
    //dumpHex((uint8_t*)NextPack,0,6);
    while((unsigned int)leftlength >= sizeof(pack_start_code)) {
        PayloadData=NULL;
        PayloadDataLen=0;
        
        if(NextPack 
        && NextPack[0]=='\x00' 
        && NextPack[1]=='\x00' 
        && NextPack[2]=='\x01' 
        && NextPack[3]=='\xE0') {
             //printf("----------------------------------------------------------E0--\r\n");
            //接着就是流包，说明是非i帧
            if(Pes(NextPack, leftlength, &NextPack, &leftlength, &PayloadData, &PayloadDataLen)) {
                if(PayloadDataLen) {
                    if(PayloadDataLen + *outlength < H264_FRAME_SIZE_MAX) {
                        *outData = (uint8_t*)realloc(*outData,*outlength+PayloadDataLen);
                        memcpy(*outData+*outlength, PayloadData, PayloadDataLen);
                        *outlength += PayloadDataLen;
                        //printf("PayloadDataLen:%d\r\n",PayloadDataLen);
                    }
                    else {
                        printf("h264 frame size exception!! %d:%d\r\n", PayloadDataLen, *outlength);
                    }
                }
            }
            else {
                if(PayloadDataLen) {
                    if(PayloadDataLen + *outlength < H264_FRAME_SIZE_MAX) {
                        *outData = (uint8_t*)realloc(*outData,*outlength+PayloadDataLen);
                        memcpy(*outData+*outlength, PayloadData, PayloadDataLen);
                        *outlength += PayloadDataLen;
                    }
                    else {
                        printf("h264 frame size exception!! %d:%d\r\n", PayloadDataLen, *outlength);
                    }
                }
            }
        }
        else if(NextPack 
            && NextPack[0]=='\x00' 
            && NextPack[1]=='\x00'
            && NextPack[2]=='\x01'
            && NextPack[3]=='\xBB') {
            //printf("----------------------------------------------------------BB--\r\n");
            if(ProgramShHead(NextPack, leftlength, &NextPack, &leftlength, &PayloadData, &PayloadDataLen)==0);
                //break;
        }
        else if(NextPack 
            && NextPack[0]=='\x00' 
            && NextPack[1]=='\x00'
            && NextPack[2]=='\x01'
            && NextPack[3]=='\xBC') {
            //printf("----------------------------------------------------------BC--\r\n");
            if(ProgramStreamMap(NextPack, leftlength, &NextPack, &leftlength, &PayloadData, &PayloadDataLen)==0);
                //break;
        }
        else if(NextPack 
            && NextPack[0]=='\x00' 
            && NextPack[1]=='\x00'
            && NextPack[2]=='\x01'
            && (NextPack[3]=='\xC0' || NextPack[3]=='\xBD')) {
            //printf("----------------------------------------------------------BD--\r\n");
            //printf("audio ps frame, skip it\n");
            break;
        }
        else {
            //printf("no know %x %x %x %x\n", NextPack[0], NextPack[1], NextPack[2], NextPack[3]);
            break;
        }
    }
    return *outlength;
}


uint32_t TdPs::makePsHeader(char *pData, unsigned long long s64Scr)
{
    unsigned long long lScrExt = (s64Scr) % 100;
    //s64Scr = s64Scr / 100;

    // 这里除以100是由于sdp协议返回的video的频率是90000，帧率是25帧/s，所以每次递增的量是3600,
    // 所以实际你应该根据你自己编码里的时间戳来处理以保证时间戳的增量为3600即可，
    //如果这里不对的话，就可能导致卡顿现象了
    bits_buffer_s   bitsBuffer;
    bitsBuffer.i_size = PS_HDR_LEN;
    bitsBuffer.i_data = 0;
    bitsBuffer.i_mask = 0x80; // 二进制：10000000 这里是为了后面对一个字节的每一位进行操作，避免大小端夸字节字序错乱
    bitsBuffer.p_data = (unsigned char *)(pData);
    memset(bitsBuffer.p_data, 0, PS_HDR_LEN);
    bits_write(&bitsBuffer, 32, 0x000001BA);      /*start codes*/
    bits_write(&bitsBuffer, 2,  1);           /*marker bits '01b'*/
    bits_write(&bitsBuffer, 3,  (s64Scr>>30)&0x07);     /*System clock [32..30]*/
    bits_write(&bitsBuffer, 1,  1);           /*marker bit*/
    bits_write(&bitsBuffer, 15, (s64Scr>>15)&0x7FFF);   /*System clock [29..15]*/
    bits_write(&bitsBuffer, 1,  1);           /*marker bit*/
    bits_write(&bitsBuffer, 15, s64Scr&0x7fff);         /*System clock [14..0]*/
    bits_write(&bitsBuffer, 1,  1);           /*marker bit*/
    bits_write(&bitsBuffer, 9,  lScrExt&0x01ff);    /*System clock ext*/
    bits_write(&bitsBuffer, 1,  1);           /*marker bit*/
    bits_write(&bitsBuffer, 22, (255)&0x3fffff);    /*bit rate(n units of 50 bytes per second.)*/
    bits_write(&bitsBuffer, 2,  3);           /*marker bits '11'*/
    bits_write(&bitsBuffer, 5,  0x1f);          /*reserved(reserved for future use)*/
    bits_write(&bitsBuffer, 3,  0);           /*stuffing length*/
    return 0;
}
uint32_t TdPs::makeSysHeader(char *pData)
{
    bits_buffer_s   bitsBuffer;
    bitsBuffer.i_size = SYS_HDR_LEN;
    bitsBuffer.i_data = 0;
    bitsBuffer.i_mask = 0x80;
    bitsBuffer.p_data = (unsigned char *)(pData);
    memset(bitsBuffer.p_data, 0, SYS_HDR_LEN);
    /*system header*/
    bits_write( &bitsBuffer, 32, 0x000001BB); /*start code*/
    bits_write( &bitsBuffer, 16, SYS_HDR_LEN-6);/*header_length 表示次字节后面的长度，后面的相关头也是次意思*/
    bits_write( &bitsBuffer, 1,  1);            /*marker_bit*/
    bits_write( &bitsBuffer, 22, 50000);    /*rate_bound*/
    bits_write( &bitsBuffer, 1,  1);            /*marker_bit*/
    bits_write( &bitsBuffer, 6,  1);            /*audio_bound*/
    bits_write( &bitsBuffer, 1,  0);            /*fixed_flag */
    bits_write( &bitsBuffer, 1,  1);          /*CSPS_flag */
    bits_write( &bitsBuffer, 1,  1);          /*system_audio_lock_flag*/
    bits_write( &bitsBuffer, 1,  1);          /*system_video_lock_flag*/
    bits_write( &bitsBuffer, 1,  1);          /*marker_bit*/
    bits_write( &bitsBuffer, 5,  1);          /*video_bound*/
    bits_write( &bitsBuffer, 1,  0);          /*dif from mpeg1*/
    bits_write( &bitsBuffer, 7,  0x7F);       /*reserver*/
    /*audio stream bound*/
    bits_write( &bitsBuffer, 8,  0xC0);         /*stream_id*/
    bits_write( &bitsBuffer, 2,  3);          /*marker_bit */
    bits_write( &bitsBuffer, 1,  0);            /*PSTD_buffer_bound_scale*/
    bits_write( &bitsBuffer, 13, 512);          /*PSTD_buffer_size_bound*/
    /*video stream bound*/
    bits_write( &bitsBuffer, 8,  0xE0);         /*stream_id*/
    bits_write( &bitsBuffer, 2,  3);          /*marker_bit */
    bits_write( &bitsBuffer, 1,  1);          /*PSTD_buffer_bound_scale*/
    bits_write( &bitsBuffer, 13, 2048);       /*PSTD_buffer_size_bound*/
    return 0;
}
uint32_t TdPs::makePsmHeader(char *pData)
{
    bits_buffer_s   bitsBuffer;
    bitsBuffer.i_size = PSM_HDR_LEN;
    bitsBuffer.i_data = 0;
    bitsBuffer.i_mask = 0x80;
    bitsBuffer.p_data = (unsigned char *)(pData);
    memset(bitsBuffer.p_data, 0, PSM_HDR_LEN);
    bits_write(&bitsBuffer, 24,0x000001);   /*start code*/
    bits_write(&bitsBuffer, 8, 0xBC);       /*map stream id*/
    bits_write(&bitsBuffer, 16,18);         /*program stream map length*/
    bits_write(&bitsBuffer, 1, 1);          /*current next indicator */
    bits_write(&bitsBuffer, 2, 3);          /*reserved*/
    bits_write(&bitsBuffer, 5, 0);          /*program stream map version*/
    bits_write(&bitsBuffer, 7, 0x7F);       /*reserved */
    bits_write(&bitsBuffer, 1, 1);          /*marker bit */
    bits_write(&bitsBuffer, 16,0);          /*programe stream info length*/
    bits_write(&bitsBuffer, 16, 8);         /*elementary stream map length  is*/
    /*audio*/
    bits_write(&bitsBuffer, 8, 0x90);       /*stream_type*/
    bits_write(&bitsBuffer, 8, 0xC0);       /*elementary_stream_id*/
    bits_write(&bitsBuffer, 16, 0);         /*elementary_stream_info_length is*/
    /*video*/
    bits_write(&bitsBuffer, 8, 0x1B);       /*stream_type*/
    bits_write(&bitsBuffer, 8, 0xE0);       /*elementary_stream_id*/
    bits_write(&bitsBuffer, 16, 0);         /*elementary_stream_info_length */
    /*crc (2e b9 0f 3d)*/
    bits_write(&bitsBuffer, 8, 0x45);       /*crc (24~31) bits*/
    bits_write(&bitsBuffer, 8, 0xBD);       /*crc (16~23) bits*/
    bits_write(&bitsBuffer, 8, 0xDC);       /*crc (8~15) bits*/
    bits_write(&bitsBuffer, 8, 0xF4);       /*crc (0~7) bits*/
    return 0;
}
uint32_t TdPs::makePesHeader(char *pData, int stream_id, int payload_len, unsigned long long pts, unsigned long long dts)
{
    bits_buffer_s   bitsBuffer;
    bitsBuffer.i_size = PES_HDR_LEN;
    bitsBuffer.i_data = 0;
    bitsBuffer.i_mask = 0x80;
    bitsBuffer.p_data = (unsigned char *)(pData);
    memset(bitsBuffer.p_data, 0, PES_HDR_LEN);
    /*system header*/
    bits_write( &bitsBuffer, 24,0x000001);  /*start code*/
    bits_write( &bitsBuffer, 8, (stream_id)); /*streamID*/
    bits_write( &bitsBuffer, 16,(payload_len)+13);  /*packet_len*/ //指出pes分组中数据长度和该字节后的长度和
    bits_write( &bitsBuffer, 2, 2 );    /*'10'*/
    bits_write( &bitsBuffer, 2, 0 );    /*scrambling_control*/
    bits_write( &bitsBuffer, 1, 0 );    /*priority*/
    bits_write( &bitsBuffer, 1, 0 );    /*data_alignment_indicator*/
    bits_write( &bitsBuffer, 1, 0 );    /*copyright*/
    bits_write( &bitsBuffer, 1, 0 );    /*original_or_copy*/
    bits_write( &bitsBuffer, 1, 1 );    /*PTS_flag*/
    bits_write( &bitsBuffer, 1, 1 );    /*DTS_flag*/
    bits_write( &bitsBuffer, 1, 0 );    /*ESCR_flag*/
    bits_write( &bitsBuffer, 1, 0 );    /*ES_rate_flag*/
    bits_write( &bitsBuffer, 1, 0 );    /*DSM_trick_mode_flag*/
    bits_write( &bitsBuffer, 1, 0 );    /*additional_copy_info_flag*/
    bits_write( &bitsBuffer, 1, 0 );    /*PES_CRC_flag*/
    bits_write( &bitsBuffer, 1, 0 );    /*PES_extension_flag*/
    bits_write( &bitsBuffer, 8, 10);    /*header_data_length*/
    // 指出包含在 PES 分组标题中的可选字段和任何填充字节所占用的总字节数。该字段之前
    //的字节指出了有无可选字段
    /*PTS,DTS*/
    bits_write( &bitsBuffer, 4, 3 );                    /*'0011'*/
    bits_write( &bitsBuffer, 3, ((pts)>>30)&0x07 );     /*PTS[32..30]*/
    bits_write( &bitsBuffer, 1, 1 );
    bits_write( &bitsBuffer, 15,((pts)>>15)&0x7FFF);    /*PTS[29..15]*/
    bits_write( &bitsBuffer, 1, 1 );
    bits_write( &bitsBuffer, 15,(pts)&0x7FFF);          /*PTS[14..0]*/
    bits_write( &bitsBuffer, 1, 1 );
    bits_write( &bitsBuffer, 4, 1 );                    /*'0001'*/
    bits_write( &bitsBuffer, 3, ((dts)>>30)&0x07 );     /*DTS[32..30]*/
    bits_write( &bitsBuffer, 1, 1 );
    bits_write( &bitsBuffer, 15,((dts)>>15)&0x7FFF);    /*DTS[29..15]*/
    bits_write( &bitsBuffer, 1, 1 );
    bits_write( &bitsBuffer, 15,(dts)&0x7FFF);          /*DTS[14..0]*/
    bits_write( &bitsBuffer, 1, 1 );
    return 0;
}
uint32_t TdPs::packagingPs(H264Stream_t *h264_stream,uint8_t **outData, int *outlength)
{
    int i = 0;
    int remainSize = h264_stream->size;
    int dataPtrOffset = 0 ;
    *outlength = 0;
    *outData = (uint8_t*)realloc(*outData,*outlength+PS_HDR_LEN);
    /*PS header*/
    makePsHeader((char*)*outData + *outlength, h264_stream->pts);
    *outlength += PS_HDR_LEN;

    if (h264_stream->is_iframe)
    {
        /*如果是I帧,添加 PS system header 和 PS system map*/
        *outData = (uint8_t*)realloc(*outData,*outlength+SYS_HDR_LEN+PSM_HDR_LEN);
        makeSysHeader((char*)*outData + *outlength);
        *outlength += SYS_HDR_LEN;
        makePsmHeader((char*)*outData + *outlength);
        *outlength += PSM_HDR_LEN;
    }

    while(remainSize>pesPayloadMax)
    {
        *outData = (uint8_t*)realloc(*outData,*outlength+PES_HDR_LEN+pesPayloadMax);
        makePesHeader((char*)*outData + *outlength, 0xE0, pesPayloadMax, h264_stream->pts, h264_stream->dts);
        *outlength += PES_HDR_LEN;
        memcpy(*outData + *outlength, h264_stream->data+dataPtrOffset, pesPayloadMax);
        *outlength += pesPayloadMax;
        dataPtrOffset = pesPayloadMax;
        remainSize -=pesPayloadMax;
    }
    *outData = (uint8_t*)realloc(*outData,*outlength+PES_HDR_LEN+remainSize);
    makePesHeader((char*)*outData + *outlength, 0xE0, remainSize, h264_stream->pts, h264_stream->dts);
    *outlength += PES_HDR_LEN;
    memcpy(*outData + *outlength, h264_stream->data+dataPtrOffset, remainSize);
    *outlength += remainSize;
    // dataPtrOffset = pesPayloadMax;
    // remainSize -=pesPayloadMax;
    return 0 ;
}