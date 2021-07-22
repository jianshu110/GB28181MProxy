#include "TdConf.h"

TdConf* TdConf::mConf ;

TdConf* TdConf::getInstance()
{
    if(mConf==nullptr)
    {
        mConf = new TdConf();
        mConf->readConfig();
    }
    return mConf ;
}


int TdConf::saveConfig()
{
    mIni.SetStringValue("GB28181","DevideId",mGB28121Ctx.mDevideId);
    mIni.SetStringValue("GB28181","ServerSipId",mGB28121Ctx.mServerSipId);
    mIni.SetStringValue("GB28181","SipServerIp",mGB28121Ctx.mSipServerIp);
    mIni.SetIntValue("GB28181","SipServerPort",mGB28121Ctx.mSipServerPort);
    mIni.SetStringValue("GB28181","SipUserName",mGB28121Ctx.mSipUserName);
    mIni.SetStringValue("GB28181","SipPassWd",mGB28121Ctx.mSipPassWd);
    mIni.SetIntValue("GB28181","Expires",mGB28121Ctx.mExpires);
    mIni.SetIntValue("GB28181","BasePort",mGB28121Ctx.mBasePort);
    mIni.SetStringValue("GB28181","Manufacture",mGB28121Ctx.mManufacture);
    mIni.SaveAs(confFile);
    return 0 ;
}

int TdConf::readConfig()
{
    if(mIni.Load(confFile)){
        saveConfig();
    }
    mIni.GetStringValue("GB28181","DevideId",&mGB28121Ctx.mDevideId);
    mIni.GetStringValue("GB28181","ServerSipId",&mGB28121Ctx.mServerSipId);
    mIni.GetStringValue("GB28181","SipServerIp",&mGB28121Ctx.mSipServerIp);
    mIni.GetIntValue("GB28181","SipServerPort",&mGB28121Ctx.mSipServerPort);
    mIni.GetStringValue("GB28181","SipUserName",&mGB28121Ctx.mSipUserName);
    mIni.GetStringValue("GB28181","SipPassWd",&mGB28121Ctx.mSipPassWd);
    mIni.GetIntValue("GB28181","Expires",&mGB28121Ctx.mExpires);
    mIni.GetIntValue("GB28181","BasePort",&mGB28121Ctx.mBasePort);
    mIni.GetStringValue("GB28181","Manufacture",&mGB28121Ctx.mManufacture);
    return 0 ;
}