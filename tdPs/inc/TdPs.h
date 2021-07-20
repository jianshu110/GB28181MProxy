#ifndef _TDPS_H_
#define _TDPS_H_ 
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>
#include "pstream.h"
#include <fstream>
#include <string.h>


#define H264_FRAME_SIZE_MAX (1024*1024*2)
#define PS_HDR_LEN  14
#define SYS_HDR_LEN 18
#define PSM_HDR_LEN 24
#define PES_HDR_LEN 19

#define pesPayloadMax 64974

typedef struct
{
    int type;
    int length;
    char *start;
}H264Nalu_t;


typedef struct
{
    int is_iframe;
    unsigned long long pts;
    unsigned long long dts;
    uint32_t size ;
    uint8_t * data ;
}H264Stream_t;

typedef struct
{
    unsigned char* p_data;
    unsigned char  i_mask;
    int i_size;
    int i_data;
}bits_buffer_s;


#define bits_write(buffer, count, bits)\
{\
  bits_buffer_s *p_buffer = (buffer);\
  int i_count = (count);\
  uint64_t i_bits = (bits);\
  while( i_count > 0 )\
  {\
    i_count--;\
    if( ( i_bits >> i_count )&0x01 )\
    {\
      p_buffer->p_data[p_buffer->i_data] |= p_buffer->i_mask;\
    }\
    else\
    {\
      p_buffer->p_data[p_buffer->i_data] &= ~p_buffer->i_mask;\
    }\
    p_buffer->i_mask >>= 1;         /*操作完一个字节第一位后，操作第二位*/\
    if( p_buffer->i_mask == 0 )     /*循环完一个字节的8位后，重新开始下一位*/\
    {\
      p_buffer->i_data++;\
      p_buffer->i_mask = 0x80;\
    }\
  }\
}

class TdPs
{
private:
    /* data */
public:
    TdPs(/* args */){psfout = new std::ofstream("dumpOutPs.h264", std::ios::out | std::ios::binary);}
    ~TdPs(){}
    uint32_t pts = 0 ;
    //uint32_t setPsPara(int fps);
    std::ofstream *psfout ;
    uint32_t ProgramStreamMap(char* Pack, int length, char **NextPack, int *leftlength, char **PayloadData, int *PayloadDataLen);
    uint32_t ProgramShHead(char* Pack, int length, char **NextPack, int *leftlength, char **PayloadData, int *PayloadDataLen);
    uint32_t Pes(char* Pack, int length, char **NextPack, int *leftlength, char **PayloadData, int *PayloadDataLen);
    uint32_t ProgramStreamPackHeader(char* Pack, int length, char **NextPack, int *leftlength) ;
    uint32_t parsingPs(int8_t*srcdata,int length,uint8_t **outData, int *outlength);

    uint32_t makePsHeader(char *pData, unsigned long long s64Scr);
    uint32_t makeSysHeader(char *pData);
    uint32_t makePsmHeader(char *pData);
    uint32_t makePesHeader(char *pData, int stream_id, int payload_len, unsigned long long pts, unsigned long long dts);

    uint32_t packagingPs(H264Stream_t *h264_stream,uint8_t **outData, int *outlength);
};
#endif