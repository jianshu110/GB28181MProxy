#include "TdConf.h"

TdConf* TdConf::mConf ;

TdConf* TdConf::getInstance()
{
    if(mConf==nullptr)
    {
        mConf = new TdConf();
    }
    return mConf ;
}

std::string TdConf::getUuid()
{
    if(!mUuid.empty())
    {
        return mUuid ;
    }
    char out[256];
    memset(out,0,256);
    std::ifstream fr = std::ifstream("mod.conf",std::ios::binary);
    fr.getline (out, 100);
    if(out[0]>0)
    {
        setUuid(out);
        return out;
    }
    else{
        fr.close();
        std::ofstream fw = std::ofstream("mod.conf",std::ios::binary);
        uuid4_init();
        uuid4_generate(out);
        fw<<out;
        fw.close();
        setUuid(out);
        return out;
    }
}
void TdConf::setUuid(std::string Uuid)
{
    mUuid = Uuid ;
}
