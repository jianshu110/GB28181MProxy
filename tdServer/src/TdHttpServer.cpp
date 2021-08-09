#include "TdHttpServer.h"

uint32_t TdHttpServer::init(){
   
    httpTh = std::thread(httploop,this);
    return 0 ;
}

void TdHttpServer::httploop(TdHttpServer* httpSrv)
{
     httpSrv->mHttpServer.Get("/", [](const Request& req, Response& res) {
        std::ifstream t("www/settting.html");
        std::stringstream buffer; 
        buffer << t.rdbuf();  
        std::string contents(buffer.str());
        t.close();
        res.set_content(contents, "text/html");
    });
   
    httpSrv->mHttpServer.Post("/gmproxy/setGB28181config", [](const Request& req, Response& res) {
        printf("req:%s\r\n",req.body.c_str());
        std::string err;
        auto jsonData = json11::Json::parse(req.body,err);
        std::string contents="";
        TdConf* lConf = TdConf::getInstance();
        if(jsonData.is_null())
        {
            contents = "{\"code\":\"10000\"}";
        }
        else{
            printf("reqsss:%s\r\n",req.body.c_str());
            lConf->mGB28121Ctx.mBasePort = jsonData["SipLocalPort"].int_value();
            lConf->mGB28121Ctx.mDevideId = jsonData["DevideId"].string_value();

            lConf->mGB28121Ctx.mServerSipId = jsonData["SIPServerID"].string_value();
            lConf->mGB28121Ctx.mServerSipDomain = jsonData["ServerSipDomain"].string_value();
            lConf->mGB28121Ctx.mSipServerIp = jsonData["SipServerIp"].string_value();
            lConf->mGB28121Ctx.mSipServerPort = jsonData["SipServerPort"].int_value();
            lConf->mGB28121Ctx.mSipUserName = jsonData["SipUserName"].string_value();

            lConf->mGB28121Ctx.mSipPassWd = jsonData["SipPassWd"].string_value();
            lConf->mGB28121Ctx.mExpires = jsonData["Expires"].int_value();
            lConf->mGB28121Ctx.mManufacture = "TDWL";

            lConf->saveConfig();
            NoticeCenter::Instance().emitEvent("modifyRegister");
            std::string contents ("{\"code\":\"0\"}");
        }
        //res.body = contents;
        res.set_content(contents,"text/json");
    });

    httpSrv->mHttpServer.Post("/gmproxy/closeChannel", [](const Request& req, Response& res) {
        
        printf("req:%s\r\n",req.body.c_str());
        std::string err;
        auto jsonData = json11::Json::parse(req.body,err);
        std::string contents="";
        if(jsonData.is_null())
        {
            contents = "{\"code\":\"10000\"}";
        }
        else{
            std::string channel = jsonData["channel"].string_value();
            NoticeCenter::Instance().emitEvent("closeChannel",channel);
            std::string contents ("{\"code\":\"0\"}");
        }
        res.set_content(contents,"text/json");
    });

      httpSrv->mHttpServer.Post("/gmproxy/creatChannel2", [](const Request& req, Response& res) {
    
        printf("req:%s\r\n",req.body.c_str());
        std::string err;
        auto jsonData = json11::Json::parse(req.body,err);
        std::string contents="";
        if(jsonData.is_null())
        {
            contents = "{\"code\":\"10000\"}";
        }
        else{
            std::string channel = jsonData["channel"].string_value();
            NoticeCenter::Instance().emitEvent("createChannel",channel);
            std::string contents ("{\"code\":\"0\"}");
        }
        res.set_content(contents,"text/json");
    });

    httpSrv->mHttpServer.Get("/gmproxy/allChannels", [](const Request& req, Response& res) {
    
        //printf("req:%s\r\n",req.body.c_str());
        std::string contents="";
        contents = TdChanManager::getInstance()->channelsInfo2Json();
        res.set_content(contents,"text/json");

    });



    httpSrv->mHttpServer.Post("/gmproxy/setvideoparam", [](const Request& req, Response& res) {
       
        printf("req:%s\r\n",req.body.c_str());
        std::string err;
        auto jsonData = json11::Json::parse(req.body,err);
        std::string contents="";
        if(jsonData.is_null())
        {
            contents = "{\"code\":\"10000\"}";
        }
        else{
            std::string Resolution = jsonData["Resolution"].string_value();
            std::string Channel = jsonData["channel"].string_value();
            int Rate = jsonData["Rate"].int_value();
            int MaxBitrate = jsonData["MaxBitrate"].int_value();
            NoticeCenter::Instance().emitEvent("setCodecParam",Channel,Rate,Resolution,MaxBitrate);
            contents = "{\"code\":\"0\"}";
        }
        //res.body = contents;
        res.set_content(contents,"text/json");
    });
    if(httpSrv->mHttpServer.listen("0.0.0.0", 8080)!=true)
    {
        printf("http 启动失败\r\n");
    }
}