#include "TdX264Handle.h"
#include<cmath>

using namespace Td;

int32_t TdX264Handle::encoderSetUp(){
    //  if( x264_param_default_preset( &encParam, "medium", NULL ) < 0 )
    //     return -1;
    // /* Configure non-default params */
    // //encParam.i_bitdepth = 8;
    // encParam.i_csp = X264_CSP_I420;
    // encParam.i_width  = 1920;
    // encParam.i_height = 1080;
    // encParam.b_vfr_input = 0;
    // encParam.b_repeat_headers = 1;
    // encParam.b_annexb = 1;
    // encParam.b_intra_refresh = 1;
    // encParam.i_keyint_max = 25;
    // if( x264_param_apply_profile( &encParam, "high" ) < 0 )
    //    return -1;
    
    // //x264_picture_init( &picture_ );
    return 0 ;
}

int32_t TdX264Handle::setEncoderParam(int width,int height,int bitrate,int iMaxBitrate,int fps)
{
    // if(encoder_==nullptr)
    //     return -1 ;
    // if( x264_param_default_preset( &encParam, "medium", NULL ) < 0 )
    //     return -1;
    // /* Configure non-default params */
    // //encParam.i_bitdepth = 8;
    // encParam.i_csp = X264_CSP_I420;
    // encParam.i_width  = width;
    // encParam.i_height = height;
    // encParam.b_vfr_input = 0;
    // encParam.b_repeat_headers = 1;
    // encParam.b_annexb = 1;
    // encParam.rc.i_bitrate = bitrate;
    // encParam.i_fps_den = 1;
    // encParam.i_fps_num = fps;
    //x264_param_t reEncParam;
    //x264_encoder_parameters(encoder_,&encParam);
    // encParam.i_width  = width;
    // encParam.i_height = height;
    // // if(x264_encoder_reconfig( encoder_, &encParam )< 0)
    // // {
    // //     printf("d33333333333333333\r\n");
    // //     return -1 ;
    // // }

    // x264_picture_clean( &picture_);
    // if( x264_picture_alloc( &picture_, encParam.i_csp, encParam.i_width, encParam.i_height ) < 0 )
    // {
    //     return -1 ;
    // }

    // // x264_encoder_intra_refresh( encoder_ );


    if( x264_param_default_preset( &encParam, "medium", NULL ) < 0 )
        return -1;
    /* Configure non-default params */
    //encParam.i_bitdepth = 8;
    encParam.i_csp = X264_CSP_I420;
    encParam.i_width  = width;
    encParam.i_height = height;
    encParam.b_vfr_input = 0;
    encParam.b_repeat_headers = 1;
    encParam.b_annexb = 1;
    encParam.b_intra_refresh = 1;
    encParam.i_keyint_max = 25;
    if( x264_param_apply_profile( &encParam, "high" ) < 0 )
       return -1;
    if(encoder_!=nullptr)
    {
        x264_encoder_close(encoder_);
        encoder_ = nullptr ;
    }
    encoder_ = x264_encoder_open( &encParam );
    if(encoder_==nullptr)
        return -1;
    x264_picture_clean( &picture_);
    if( x264_picture_alloc( &picture_, encParam.i_csp, encParam.i_width, encParam.i_height ) < 0 )
    {
        return -1 ;
    }
    return 0 ;
}
void TdX264Handle::setPicture(int width,int height,int Ystride,int UVstride)
{
    picParam.height = height;
    picParam.width = width ;
    picParam.Ystride = width * height;
    picParam.UVstride = picParam.Ystride/4;
}
int32_t TdX264Handle::encode(uint8_t* Ydata,uint8_t* Udata,uint8_t* Vdata,uint8_t** pkt,uint32_t* pkt_size,bool& is_keyframe,bool& got_output){
    x264_nal_t * nals = nullptr;
    int i_nal =0 ;
    int i_frame_size = 0 ;
    x264_picture_t pic_out;
    *pkt = nullptr ;
    *pkt_size = 0;
    memcpy(picture_.img.plane[0],Ydata,picParam.Ystride);
    memcpy(picture_.img.plane[1],Udata,picParam.UVstride);
    memcpy(picture_.img.plane[2],Vdata,picParam.UVstride);
    picture_.i_pts = timestamp_++;
    is_keyframe = false ;
    i_frame_size = x264_encoder_encode( encoder_, &nals, &i_nal, &picture_, &pic_out );
    if( i_frame_size < 0 )
          return -1 ;
    else if( i_frame_size )
    {
        
        x264_nal_t *nal;
        for (nal = nals; nal < nals + i_nal; nal++) {
            //printf("%d %d\r\n",nal->i_payload,*pkt_size);
            *pkt = (uint8_t*)realloc(*pkt,*pkt_size+nal->i_payload);
            memcpy(*pkt+*pkt_size,nals->p_payload,nal->i_payload);
            *pkt_size+=nal->i_payload;
        }
        //printf("nal->i_type:%d i_frame_size:%d nal->i_payload:%d i_nal:%d\r\n",nals->i_type,i_frame_size,nals->i_payload,i_nal);
        if((nals->i_type==NAL_PPS)||(nals->i_type==NAL_SLICE_IDR)||(nals->i_type==NAL_SPS))
        {
             is_keyframe = true;
        }
    }
    return 0 ;
}

int32_t TdX264Handle::create()
{   
    return 0 ;
}


int32_t TdX264Handle::destory()
{
    if(encoder_)
        x264_encoder_close(encoder_);
    return 0 ;
}