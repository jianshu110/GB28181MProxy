#include "TdHttpServer.h"

uint32_t TdHttpServer::init(){
   
    // NoticeCenter::Instance().addListener(0,"once",
    //         [](int &a,const char * &b,double &c,string &d){
    //     printf("fdsffdsfdsfdsfds\r\n");
    
    // });
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
   
    httpSrv->mHttpServer.Post("/gmproxy/setconfig", [](const Request& req, Response& res) {
       
        printf("req:%s\r\n",req.body.c_str());
        std::string err;
        auto jsonData = json11::Json::parse(req.body,err);
        std::string contents="";
        if(jsonData.is_null())
        {
            contents = "{\"code\":\"10000\"}";
        }
        else{
            std::string contents ("{\"code\":\"0\"}");
        }
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

      httpSrv->mHttpServer.Post("/gmproxy/closeChannel2", [](const Request& req, Response& res) {
    
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
            printf("9999999999999999999999\r\n");
            std::string contents ("{\"code\":\"0\"}");
        }
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
        res.set_content(contents,"text/json");
    });
    httpSrv->mHttpServer.listen("0.0.0.0", 8080);
}