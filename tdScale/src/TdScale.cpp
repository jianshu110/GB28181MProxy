#include "TdScale.h"
#include "stdlib.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
YUVFrame_t * TdScale::newEmptyYUVFrame(){
    YUVFrame_t * lYUVFrame = (YUVFrame_t *)malloc(sizeof(YUVFrame_t));
    if(lYUVFrame==nullptr)
        return nullptr ;
    memset(lYUVFrame,0,sizeof(YUVFrame_t));
    return lYUVFrame ;
}

// int TdScale::initYUVFrame(YUVFrame_t* frame)
// {
    
// }

YUVFrame_t * TdScale::newYUVFrame(int Width,int Height,int Ystride,int UVstride)
{
    YUVFrame_t * lYUVFrame = newEmptyYUVFrame();
    int size  = Width*Height;
    printf("%d  %d\r\n",__LINE__,size);
    // align_buffer_page_end(&lYUVFrame->data[0],size);
    // align_buffer_page_end(&lYUVFrame->data[1],size/2);
    // align_buffer_page_end(&lYUVFrame->data[2],size/2);
    lYUVFrame->stride[0] =  Ystride;
    lYUVFrame->stride[1] =  UVstride;
    lYUVFrame->height = Width;
    lYUVFrame->width = Height;
    return lYUVFrame ;
}

int TdScale::scale(YUVFrame_t* srcFrame,YUVFrame_t* dstFrame)
{
    int dst_width_uv,dst_height_uv,dst_y_plane_size;
    int dst_uv_plane_size,dst_stride_y,dst_stride_uv;
    int status ;
    if((srcFrame==nullptr)||(dstFrame==nullptr))
    {
        printf("nullptr\r\n");
        return -1 ;
    }
    dst_width_uv = (abs(dstFrame->width) + 1) >> 1;
    dst_height_uv = (abs(dstFrame->height) + 1) >> 1;

    dst_stride_y = dstFrame->width;
    dst_stride_uv = dst_width_uv;

    dst_y_plane_size = (dstFrame->width) * (dstFrame->height);
    dst_uv_plane_size = (dst_width_uv) * (dst_height_uv);


    
    align_buffer_page_end(dst_y_c,dst_y_plane_size);
    align_buffer_page_end(dst_u_c,dst_uv_plane_size);
    align_buffer_page_end(dst_v_c,dst_uv_plane_size);

    status = libyuv::I420Scale(srcFrame->data[0].data, srcFrame->stride[0],srcFrame->data[1].data, 
    srcFrame->stride[1], srcFrame->data[2].data,srcFrame->stride[1],
    srcFrame->width, srcFrame->height, dst_y_c, dst_stride_y, dst_u_c,
    dst_stride_uv, dst_v_c, dst_stride_uv, dstFrame->width, dstFrame->height,libyuv::kFilterNone);
    if(status){
        return -1 ;
    }

    dstFrame->data[0].size =  dst_y_plane_size;
    dstFrame->data[0].data = (uint8_t*)malloc(dst_y_plane_size);

    dstFrame->data[1].size =  dst_uv_plane_size;
    dstFrame->data[1].data = (uint8_t*)malloc(dst_uv_plane_size);

    dstFrame->data[2].size =  dst_uv_plane_size;
    dstFrame->data[2].data = (uint8_t*)malloc(dst_uv_plane_size);

    memcpy(dstFrame->data[0].data,dst_y_c,dst_y_plane_size);
    memcpy(dstFrame->data[1].data,dst_u_c,dst_uv_plane_size);
    memcpy(dstFrame->data[2].data,dst_v_c,dst_uv_plane_size);

    dstFrame->stride[0] = dst_stride_y;
    dstFrame->stride[1] = dst_stride_uv ; 

    free_aligned_buffer_page_end(dst_y_c);
    free_aligned_buffer_page_end(dst_u_c);
    free_aligned_buffer_page_end(dst_v_c);
    return 0 ;
}

void TdScale::freeYUVFrame(YUVFrame_t * YUVFrame)
{
    for(int i = 0;i<3;i++)
    {
        if(YUVFrame->data[i].data!=nullptr)
        {
            free(YUVFrame->data[i].data);
            YUVFrame->data[i].data = nullptr;
        }
    }
    memset(YUVFrame,0,sizeof(YUVFrame_t));
}
