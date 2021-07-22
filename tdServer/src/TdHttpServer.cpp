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
        std::string contents ("{\"code\":\"0\"}");
        res.set_content(contents,"text/json");
    });


    // NoticeCenter::Instance().emitEvent("once",1,(const char *)"b",2,"d");
    httpSrv->mHttpServer.listen("0.0.0.0", 8080);
}