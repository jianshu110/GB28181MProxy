#ifndef _TD_SCALE_H_
#define _TD_SCALE_H_ 
#include "libyuv/cpu_id.h"
#include "libyuv/scale.h"
#include "stdlib.h"
#include "stdio.h"


typedef struct {
    uint8_t * data ;
    uint8_t * mem;
}buff_t ;

static const int kMaxWidth = 32768;
static const int kMaxHeight = 32768;

static inline bool SizeValid(int src_width,
                             int src_height,
                             int dst_width,
                             int dst_height) {
  if (src_width > kMaxWidth || src_height > kMaxHeight ||
      dst_width > kMaxWidth || dst_height > kMaxHeight) {
    printf("Warning - size too large to test.  Skipping\n");
    return false;
  }
  return true;
}

// extern unsigned int fastrand_seed;
// inline int fastrand() {
//   fastrand_seed = fastrand_seed * 214013u + 2531011u;
//   return static_cast<int>((fastrand_seed >> 16) & 0xffff);
// }

// static inline void MemRandomize(uint8_t* dst, int64_t len) {
//   int64_t i;
//   for (i = 0; i < len - 1; i += 2) {
//     *reinterpret_cast<uint16_t*>(dst) = fastrand();
//     dst += 2;
//   }
//   for (; i < len; ++i) {
//     *dst++ = fastrand();
//   }
// }


#define align_buffer_page_end(var, size)                                \
  uint8_t* var##_mem =                                                  \
      reinterpret_cast<uint8_t*>(malloc(((size) + 4095 + 63) & ~4095)); \
  uint8_t* var = reinterpret_cast<uint8_t*>(                            \
      (intptr_t)(var##_mem + (((size) + 4095 + 63) & ~4095) - (size)) & ~63)

#define free_aligned_buffer_page_end(var) \
  free(var##_mem);                        \
  var = 0

typedef struct  {
    buff_t *data;
    int *stride;
    int width;
    int height;
}YUVFrame_t;

class TdScale
{
private:
    /* data */
public:
    TdScale(/* args */){};
    ~TdScale(){};

    // int align_buffer_page_end(buff_t* buff,int size){
    //     buff->mem = reinterpret_cast<uint8_t*>(malloc(((size) + 4095 + 63) & ~4095)); 
    //     buff->data = reinterpret_cast<uint8_t*>((intptr_t)(buff->mem + (((size) + 4095 + 63) & ~4095) - (size)) & ~63);
    //     return 0 ;
    // };
    // void free_aligned_buffer_page_end(buff_t* dataPtr){
    //     if(dataPtr->mem!=nullptr)
    //         free(dataPtr->mem);
    //     dataPtr->data = nullptr;
    //     //free(dataPtr);
    // }
    int I420TestFilter(int src_width,
                          int src_height,
                          int dst_width,
                          int dst_height,
                          int f,
                          int benchmark_iterations,
                          int disable_cpu_flags,
                          int benchmark_cpu_info);
    YUVFrame_t * newEmptyYUVFrame();
    YUVFrame_t * newYUVFrame(int Width,int Height,int Ystride,int UVstride);
    void freeYUVFrame(YUVFrame_t * YUVFrame);
    int scale(YUVFrame_t* srcFrame,YUVFrame_t* dstFrame) ;
};


#endif