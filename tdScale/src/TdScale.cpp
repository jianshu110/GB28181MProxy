#include "TdScale.h"
#include "stdlib.h"
#include <stdlib.h>
#include <math.h>
YUVFrame_t * TdScale::newEmptyYUVFrame(){
    YUVFrame_t * lYUVFrame = new YUVFrame_t();
    lYUVFrame->data = new buff_t[3];
    lYUVFrame->data[0].data = nullptr;
    lYUVFrame->data[1].data = nullptr;
    lYUVFrame->data[2].data = nullptr;
    lYUVFrame->stride = new int[2];
    lYUVFrame->stride[0] =  0;
    lYUVFrame->stride[1] =  0;
    return lYUVFrame ;
}

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

void TdScale::freeYUVFrame(YUVFrame_t * YUVFrame)
{
    if(YUVFrame==nullptr)
        return ;
    if(YUVFrame->stride!=nullptr)
    {
        delete []YUVFrame->stride;
        YUVFrame->stride = nullptr;
    }
    printf("%d %s\r\n",__LINE__,__FILE__);
    
    //delete []YUVFrame->data[0];

    printf("%d %s\r\n",__LINE__,__FILE__);
    printf("%d\r\n",YUVFrame->data[0]);
    // free_aligned_buffer_page_end(&YUVFrame->data[0]);
    free(YUVFrame->data);
    YUVFrame->data = nullptr;
    // delete []YUVFrame->data[1];
    // delete []YUVFrame->data[2];
    // if(YUVFrame->data!=nullptr)
    // {
    //     for (int i=0;i<3;i++){
    //         if(YUVFrame->data[i]!=nullptr)
    //         {
    //             printf("%d\r\n",__LINE__);
    //             delete []YUVFrame->data[i] ;
    //             //YUVFrame->data[i] = nullptr;
    //         }
    //     }
    // }
    printf("%d\r\n",__LINE__);
    delete YUVFrame;
    YUVFrame = nullptr;
    return ;
}

int TdScale::scale(YUVFrame_t* srcFrame,YUVFrame_t* dstFrame)
{
    if((srcFrame==nullptr)||(dstFrame==nullptr))
    {
        printf("nullptr\r\n");
        return -1 ;
    }
    return libyuv::I420Scale(
                    srcFrame->data[0].data, srcFrame->stride[0],
                    srcFrame->data[1].data, srcFrame->stride[1],
                    srcFrame->data[2].data, srcFrame->stride[1],
                    srcFrame->width, srcFrame->height,

                    dstFrame->data[0].data, dstFrame->stride[0],
                    dstFrame->data[1].data, dstFrame->stride[1],
                    dstFrame->data[2].data, dstFrame->stride[1],
                    dstFrame->width, dstFrame->height,
                    libyuv::kFilterNone);
}


// int TdScale::I420TestFilter(int src_width,
//                           int src_height,
//                           int dst_width,
//                           int dst_height,
//                           int f,
//                           int benchmark_iterations,
//                           int disable_cpu_flags,
//                           int benchmark_cpu_info) {
//   if (!SizeValid(src_width, src_height, dst_width, dst_height)) {
//     return 0;
//   }

//   int i, j;
//   int src_width_uv = (abs(src_width) + 1) >> 1;
//   int src_height_uv = (abs(src_height) + 1) >> 1;

//   int64_t src_y_plane_size = (abs(src_width)) * (abs(src_height));
//   int64_t src_uv_plane_size = (src_width_uv) * (src_height_uv);

//   int src_stride_y = abs(src_width);
//   int src_stride_uv = src_width_uv;

//   align_buffer_page_end(src_y, src_y_plane_size);
//   align_buffer_page_end(src_u, src_uv_plane_size);
//   align_buffer_page_end(src_v, src_uv_plane_size);
//   if (!src_y || !src_u || !src_v) {
//     printf("Skipped.  Alloc failed \n");
//     return 0;
//   }
//   MemRandomize(src_y, src_y_plane_size);
//   MemRandomize(src_u, src_uv_plane_size);
//   MemRandomize(src_v, src_uv_plane_size);

//   int dst_width_uv = (dst_width + 1) >> 1;
//   int dst_height_uv = (dst_height + 1) >> 1;

//   int64_t dst_y_plane_size = (dst_width) * (dst_height);
//   int64_t dst_uv_plane_size = (dst_width_uv) * (dst_height_uv);

//   int dst_stride_y = dst_width;
//   int dst_stride_uv = dst_width_uv;

//   align_buffer_page_end(dst_y_c, dst_y_plane_size);
//   align_buffer_page_end(dst_u_c, dst_uv_plane_size);
//   align_buffer_page_end(dst_v_c, dst_uv_plane_size);
//   align_buffer_page_end(dst_y_opt, dst_y_plane_size);
//   align_buffer_page_end(dst_u_opt, dst_uv_plane_size);
//   align_buffer_page_end(dst_v_opt, dst_uv_plane_size);
//   if (!dst_y_c || !dst_u_c || !dst_v_c || !dst_y_opt || !dst_u_opt ||
//       !dst_v_opt) {
//     printf("Skipped.  Alloc failed \n");
//     return 0;
//   }

//   MaskCpuFlags(disable_cpu_flags);  // Disable all CPU optimization.
  
 
//   I420Scale(src_y, src_stride_y, src_u, src_stride_uv, src_v, src_stride_uv,
//             src_width, src_height, dst_y_c, dst_stride_y, dst_u_c,
//             dst_stride_uv, dst_v_c, dst_stride_uv, dst_width, dst_height, f);

//   free_aligned_buffer_page_end(dst_y_c);
//   free_aligned_buffer_page_end(dst_u_c);
//   free_aligned_buffer_page_end(dst_v_c);
//   free_aligned_buffer_page_end(dst_y_opt);
//   free_aligned_buffer_page_end(dst_u_opt);
//   free_aligned_buffer_page_end(dst_v_opt);
//   free_aligned_buffer_page_end(src_y);
//   free_aligned_buffer_page_end(src_u);
//   free_aligned_buffer_page_end(src_v);

//   return 0;
//}