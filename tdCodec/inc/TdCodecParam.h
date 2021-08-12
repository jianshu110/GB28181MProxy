#pragma once

class TdCodecParam
{
private:
    int mWidth=960;
    int mHeight = 540;
    int mRate = 25;
    int mMaxBitrate=300000;
    int mBaseBitrate = 150000;
    int mIsRefresh = false;
    /* data */
public:
    TdCodecParam(/* args */){};
    ~TdCodecParam(){};
    int getCodecWidth(){
        return mWidth ;
    }
    void setCodecWidth(int Width){
        mWidth = Width ;
    }
    int getCodecHeight(){
        return mHeight ;
    }
    void setCodecHeight(int Height){
        mHeight = Height ;
    }
    int getCodecRate(){
        return mRate ;
    }
    void setCodecRate(int Rate){
        mRate = Rate ;
    }
    int getCodecMaxBitrate(){
        return mMaxBitrate ;
    }
    void setCodecMaxBitrate(int MaxBitrate){
        mMaxBitrate = MaxBitrate ;
    }
    bool isCodecParamRefresh(){
        return mIsRefresh ;
    }
    void setCodecParamRefresh(bool status){
        mIsRefresh = status;
    }
    int getCodecBaseBitrate()
    {
        return mBaseBitrate ;
    }

    int setCodecBaseBitrate(int BaseBitrate)
    {
        return mBaseBitrate =BaseBitrate;
    }
};
