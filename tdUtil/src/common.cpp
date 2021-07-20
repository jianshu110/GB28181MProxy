//
// Created by Administrator on 2020/2/12 0012.
//

#include "../inc/json11.hpp"
#include "../inc/common.h"
#include <string>
#include <sstream>



std::string&  replaceAll(std::string&str, const  std::string& old_value, const std::string& new_value)
{
    while(true)
    {
        std::string::size_type   pos(0); if((pos=str.find(old_value)) != std::string::npos)
        {
            str.replace(pos,old_value.length(),new_value);
        }
        else {
            break;
        }
    }
    return   str;
}

std::string int2string(uint32_t data)
{
    std::stringstream ssTemp;
    ssTemp<<data;
    return ssTemp.str();
}

std::string long2string(uint64_t data)
{
    std::stringstream ssTemp;
    ssTemp<<data;
    return ssTemp.str();
}

long long string2longlong(std::string data)
{
	if(data.empty())
		return 0 ;
	return strtoll(data.c_str(), NULL, 10);
}

int string2int(std::string data)
{
    std::stringstream ss;
    ss<<data;
    int i;
    ss>>i ;
    return i ;
}

void splitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));

}

bool isCode0(std::string jsonData)
{
    std::string error ;
	if(jsonData.empty())
		return false ;
    auto jsonParseRet = json11::Json::parse(jsonData, error);
    if (jsonParseRet.is_null()) {
        printf("%s %d err::%s\r\n",__func__,__LINE__,error.c_str());
        return false ;
    }
    if(jsonParseRet["code"].is_string())
    {
        if(jsonParseRet["code"].string_value().compare("0"))
            return  false ;
    }
    else{
        if(jsonParseRet["code"].int_value()!=0)
            return  false ;
    }
    return true ;
}


int getCodeValue(std::string jsonData)
{
    std::string error ;
    auto jsonParseRet = json11::Json::parse(jsonData, error);
    if (jsonParseRet.is_null()) {
        printf("%s %d err::%s\r\n",__func__,__LINE__,error.c_str());
        return -1 ;
    }
    if(jsonParseRet["code"].is_string())
    {
        return  string2int(jsonParseRet["code"].string_value());
    }
    else{
        if(jsonParseRet["code"].is_number())
            return jsonParseRet["code"].int_value();
    }
    return -1 ;
}


std::string getRand()
{
    srand((unsigned)time(NULL));
    return int2string(rand()%10000);
}


std::string getMsStr()
{
    struct timeval xTime;
    gettimeofday(&xTime, NULL);
    long long xFactor = 1;
    long long now = (long long)(( xFactor * xTime.tv_sec * 1000) + (xTime.tv_usec / 1000));
    char timeMs[16];
    memset(timeMs,0,16);
    snprintf(timeMs,16,"%13lld",now);
    std::string retTimeMsStr = timeMs;
    return retTimeMsStr ;
}


std::string getMd5(std::string Data) {
    if (Data.empty())
        return Data;
    return MD5(Data).toStr();
}




