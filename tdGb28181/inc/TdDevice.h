#ifndef DEVICE_INCLUDE
#define DEVICE_INCLUDE

#include <string>
#include <tuple>
#include <memory>
#include "eXosip2/eXosip.h"
#include "inc/TdChanManager.h"
#include "pugixml.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <string>
#include <sstream>
#include <thread>
#include <tuple> 
#include <inc/common.h> 
#include "inc/noticeCenter.h"

using namespace std;

class TdDevice {
public:

    TdDevice() {}
    TdDevice(string server_sip_id, string server_ip, int server_port,
            string device_sip_id, string username, string password,
            int local_port,
            string manufacture): 
            server_sip_id(server_sip_id), 
            server_ip(server_ip),
            server_port(server_port),
            device_sip_id(device_sip_id),
            username(username),
            password(password),
            local_port(local_port),
            manufacture(manufacture){
        sip_context = nullptr;
        is_running = false;
        is_register = false;
        local_ip = string(128, '0');
    }

    ~TdDevice(){}

    void start();

    void stop();

    void Register();

    void process_request();

    void process_catalog_query(string sn);

    void process_deviceinfo_query(string sn);

    void process_devicestatus_query(string sn);

    void process_devicecontrol_query(string sn);

    void heartbeat_task();

    void send_request(osip_message_t * request);

    void send_response(shared_ptr<eXosip_event_t> evt, osip_message_t * msg);

    osip_message_t * create_msg();

    void send_response_ok(shared_ptr<eXosip_event_t> evt);
    
    void send_response_err(shared_ptr<eXosip_event_t> evt);

    std::tuple<string, string> get_cmd(const char * body);
    std::string getValueByNodeName(const char * body,std::string nodeName);
    std::string getValueByAttrKey(const char * sdp,std::string AttrKey);
    int createSession(std::string deviceId,std::string dest,int destPort,int basePort);
    int destorySession(std::string deviceId);
public:
    string server_sip_id;
    string server_ip;
    int server_port;
    string device_sip_id;
    string username;
    string password;
    string local_ip;
    int local_port;

    string manufacture;
    string rtp_ip;
    int rtp_port;
    string rtp_protocol;

private:
    eXosip_t* sip_context;
    bool is_running;
    bool is_register;
    bool is_pushing;

    string from_sip;
    string to_sip;
    string ssrc;

    int sockfd;
    int bind();
    void send_network_packet(const char * data, int length);
};

#endif