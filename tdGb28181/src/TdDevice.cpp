#include "inc/TdDevice.h"


static int SN_MAX = 99999999;
static int sn;

static int get_sn() {
	if (sn >= SN_MAX) {
		sn = 0;
	}
	sn++;
	return sn;
}

void TdDevice::start() {
    spdlog::debug("sip init begin.");

    sip_context = eXosip_malloc();

    if (OSIP_SUCCESS != eXosip_init(sip_context)) {
        spdlog::error("sip init failed.");
        return;
    }

    if (OSIP_SUCCESS != eXosip_listen_addr(sip_context, IPPROTO_UDP, nullptr, local_port, AF_INET, 0)) {
        spdlog::critical("sip port bind failed.");
        eXosip_quit(sip_context);
        sip_context = nullptr;
        return;
    }

    // run
    is_running = true;

    ostringstream from_uri;
    ostringstream contact;
    ostringstream proxy_uri;

    // local ip & port
    eXosip_guess_localip(sip_context, AF_INET, /*data(local_ip)*/(char*)local_ip.c_str(), local_ip.length());
    spdlog::debug("local ip is {}", local_ip);

    from_uri << "sip:" << device_sip_id << "@" << local_ip << ":" << local_port;
    contact << "sip:" << device_sip_id << "@" << local_ip << ":" << local_port;
    proxy_uri << "sip:" << server_sip_id << "@" << server_ip << ":" << server_port;

    from_sip = from_uri.str();
    to_sip = proxy_uri.str();

    spdlog::debug("from uri is {}", from_sip);
    spdlog::debug("contact is {}", contact.str());
    spdlog::debug("proxy_uri is {}", to_sip);

    // clear auth
    eXosip_clear_authentication_info(sip_context);

    osip_message_t * register_message = nullptr;
    int register_id = eXosip_register_build_initial_register(sip_context, from_sip.c_str(), 
                    to_sip.c_str(), 
                    contact.str().c_str(), 3600, &register_message);
    if (nullptr == register_message) {
        spdlog::error("eXosip_register_build_initial_register failed");
        return;
    }

    eXosip_lock(sip_context);
	eXosip_register_send_register(sip_context, register_id, register_message);
	eXosip_unlock(sip_context);

    thread heartbeat_task_thread(&TdDevice::heartbeat_task, this);
    heartbeat_task_thread.detach();
    spdlog::info("启动GB28181协议栈成功");
    this->process_request();
   
}

void TdDevice::process_request() {
    while (is_running) {
        auto evt = shared_ptr<eXosip_event_t>(
            eXosip_event_wait(sip_context, 0, 100),
            eXosip_event_free);

        eXosip_lock(sip_context);
        eXosip_automatic_action(sip_context);
        eXosip_unlock(sip_context);

        if (evt == nullptr) {
            continue;
        }

        switch (evt->type)
        {
        case eXosip_event_type::EXOSIP_REGISTRATION_SUCCESS: {
            spdlog::info("GB28181注册成功");
            is_register = true;
            break;
        }
        case eXosip_event_type::EXOSIP_REGISTRATION_FAILURE: {
            spdlog::info("GB28181注册失败");
            if (evt->response == nullptr) {
                spdlog::error("register 401 has no response !!!");
                break;
            }

            if (401 == evt->response->status_code) {
                osip_www_authenticate_t * www_authenticate_header;

                osip_message_get_www_authenticate(evt->response, 0, &www_authenticate_header);

                if (eXosip_add_authentication_info(sip_context, device_sip_id.c_str(), username.c_str(), password.c_str(), 
                                    "MD5", www_authenticate_header->realm)) {
                    spdlog::error("register add auth failed");
                    break;
                };
            };
            break;
        }
        case eXosip_event_type::EXOSIP_MESSAGE_NEW: {
            spdlog::debug("got MESSAGE_NEW");

            if (MSG_IS_MESSAGE(evt->request)) {
                osip_body_t * body = nullptr;
                osip_message_get_body(evt->request, 0, &body);
                if (body == nullptr) {
                    //spdlog::info("new message request: \n{}", body->body);
                    break;
                }   
                auto cmd_sn = this->get_cmd(body->body);
                string cmd = get<0>(cmd_sn);
                string sn = get<1>(cmd_sn);
                spdlog::debug("got new cmd: {}", cmd);
                if ("Catalog" == cmd) {
                    this->process_catalog_query(sn);
                } else if ("DeviceStatus" == cmd) {
                    this->process_devicestatus_query(sn);
                } else if ("DeviceInfo" == cmd) {
                    this->process_deviceinfo_query(sn);
                } else if ("DeviceControl" == cmd) {
                    this->process_devicecontrol_query(sn);
                }else if ("mediaProxy" == cmd) {
                    std::string channel = getValueByNodeName(body->body,"DeviceID");
                    std::string subCmd = getValueByNodeName(body->body,"CmdSubType");
                    spdlog::debug("got channel: {} subCmd:{} ", channel,subCmd);
                    if(!subCmd.compare("start"))
                    {
                        std::string destHost = getValueByNodeName(body->body,"DestHost");
                        std::string destPort = getValueByNodeName(body->body,"DestPort");
                        std::string basePort = getValueByNodeName(body->body,"BasePort");
                        spdlog::debug("got destHost: {} destPort:{} basePort:{}", destHost,destPort,basePort);
                        if(!createSession(channel,destHost,string2int(destPort),string2int(basePort)))
                        {
                            spdlog::info("通道({}) -> 启动成功",channel);
                            usleep(500);
                            this->send_response_ok(evt);
                        }
                        else
                        {
                            spdlog::info("通道({}) -> 启动失败",channel);
                            this->send_response_err(evt);
                        }
                    }
                    else if(!subCmd.compare("end"))
                    {
                        destorySession(channel);
                        this->send_response_ok(evt);
                        spdlog::info("通道({}) -> 关闭成功",channel);
                    }
                }
                else{
                    spdlog::error("unhandled cmd: {}", cmd);
                }
            } else if (MSG_IS_BYE(evt->request)) {
                spdlog::info("got BYE message");
                this->send_response_ok(evt);
                break;
            }
            break;
        }
        case eXosip_event_type::EXOSIP_CALL_INVITE: {
            spdlog::info("got CALL_INVITE");

            sdp_message_t *sdp_msg = eXosip_get_remote_sdp(sip_context, evt->did);
            if (!sdp_msg) {
                spdlog::error("eXosip_get_remote_sdp failed");
                break;
            }
        
            char * sdpStr ;
            sdp_message_to_str(sdp_msg,&sdpStr);

            //getValueByAttrKey(sdpStr,"rtpmap");

            printf("sdpStr:%s\r\n",sdpStr);
            auto connection = eXosip_get_video_connection(sdp_msg);
            if (!connection) {
                spdlog::error("eXosip_get_video_connection failed");
                break;                
            }

            rtp_ip = connection->c_addr;

            auto video_sdp = eXosip_get_video_media(sdp_msg);
            if (!video_sdp) {
                spdlog::error("eXosip_get_video_media failed");
                break;                  
            }

            rtp_port = atoi(video_sdp->m_port);

            spdlog::info("rtp server: {}:{}", rtp_ip, rtp_port);

            rtp_protocol = video_sdp->m_proto;

            spdlog::info("rtp protocol: {}", rtp_protocol);

            osip_body_t *sdp_body = NULL;
			osip_message_get_body(evt->request, 0, &sdp_body);
            if (nullptr == sdp_body) {
                spdlog::error("osip_message_get_body failed");
                break; 
            }

            string body = sdp_body->body;
            auto y_sdp_first_index = body.find("y=");
            auto y_sdp = body.substr(y_sdp_first_index);
            auto y_sdp_last_index = y_sdp.find("\r\n");
            ssrc = y_sdp.substr(2, y_sdp_last_index-1);
            spdlog::info("ssrc: {}", ssrc);

             osip_message_t * message = evt->request;
            int status = eXosip_call_build_answer(sip_context, evt->tid, 200, &message);
            if (status != 0) {
                spdlog::error("call invite build answer failed");
                break;
            }
            this->send_response_ok(evt);
             break;
        }
        case eXosip_event_type::EXOSIP_CALL_ACK: {
            spdlog::info("got CALL_ACK: begin pushing rtp stream...");
            if (is_pushing) {
                spdlog::info("already pushing rtp stream");
            } else {
                printf("推流\r\n");
                //thread t(&Device::push_rtp_stream, this);
                //t.detach();
            }
            break;
        }
        case eXosip_event_type::EXOSIP_CALL_CLOSED: {
            spdlog::info("got CALL_CLOSED: stop pushing rtp stream...");

            break;
        }
        case eXosip_event_type::EXOSIP_MESSAGE_ANSWERED: {
            //spdlog::info("got MESSAGE_ANSWERED: unhandled");
            break;
        }
        
        default: {
            spdlog::info("unhandled sip evt type: {}", evt->type);
            break;
        }
        }
    }
}

void TdDevice::process_catalog_query(string sn) {
    stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n";
    ss << "<Response>\r\n";
    ss << "<CmdType>Catalog</CmdType>\r\n";
    ss << "<SN>" << sn << "</SN>\r\n";
    ss << "<DeviceID>" << device_sip_id << "</DeviceID>\r\n";
    ss << "<SumNum>" << 1 << "</SumNum>\r\n";
    ss << "<DeviceList Num=\"" << 1 << "\">\r\n";
    ss << "<Item>\r\n";
    ss << "<DeviceID>" << device_sip_id << "</DeviceID>\r\n";
    ss << "<Manufacturer>" << manufacture << "</Manufacturer>\r\n";
    ss << "<Status>ON</Status>\r\n";
    ss << "<Name>MediaProxy</Name>\r\n";
    ss << "<ParentID>" << server_sip_id << "</ParentID>\r\n";
    ss << "</Item>\r\n";
    ss << "</DeviceList>\r\n";
    ss << "</Response>\r\n";
    spdlog::debug("catalog response: \n{}", ss.str());
    auto request = create_msg();
    if (request != NULL) {
        osip_message_set_content_type(request, "Application/MANSCDP+xml");
        osip_message_set_body(request, ss.str().c_str(), strlen(ss.str().c_str()));
        send_request(request);
    }
}

void TdDevice::process_devicestatus_query(string sn) {
    stringstream ss;

    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char curtime[72] = {0};
    sprintf(curtime, "%d-%d-%dT%02d:%02d:%02d", (timeinfo->tm_year + 1900), (timeinfo->tm_mon + 1),
                        timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);    
    
    ss << "<?xml version=\"1.0\"?>\r\n";
    ss << "<Response>\r\n";
    ss << "<CmdType>DeviceStatus</CmdType>\r\n";
    ss << "<SN>" << get_sn() << "</SN>\r\n";
    ss << "<DeviceID>" << device_sip_id << "</DeviceID>\r\n";
    ss << "<Result>OK</Result>\r\n";
    ss << "<Online>ONLINE</Online>\r\n";
    ss << "<Status>OK</Status>\r\n";
    ss << "<DeviceTime>" << curtime << "</DeviceTime>\r\n";
    ss << "<Alarmstatus Num=\"0\">\r\n";
    ss << "</Alarmstatus>\r\n";
    ss << "<Encode>ON</Encode>\r\n";
    ss << "<Record>OFF</Record>\r\n";
    ss << "</Response>\r\n";

    spdlog::info("devicestatus response: \n{}", ss.str());
    auto request = create_msg();
    if (request != NULL) {
        osip_message_set_content_type(request, "Application/MANSCDP+xml");
        osip_message_set_body(request, ss.str().c_str(), strlen(ss.str().c_str()));
        send_request(request);
    }
}

void TdDevice::process_deviceinfo_query(string sn) {
    stringstream ss;

    ss << "<?xml version=\"1.0\"?>\r\n";
    ss <<    "<Response>\r\n";
    ss <<    "<CmdType>DeviceInfo</CmdType>\r\n";
    ss <<    "<SN>" << get_sn() << "</SN>\r\n";
    ss <<    "<DeviceID>" << device_sip_id << "</DeviceID>\r\n";
    ss <<    "<DeviceName>jiuli</DeviceName>";
    ss <<    "<Name>媒体代理服务</Name>\r\n" ;
    ss <<    "<Result>OK</Result>\r\n";
    ss <<    "<DeviceType>GMProxy</DeviceType>\r\n";
    ss <<    "<Manufacturer>TDWL</Manufacturer>\r\n";
    ss <<    "<Model>CH-QDLC-21-01020-MP</Model>\r\n";
    ss <<    "<Firmware>v1.0.2</Firmware>\r\n";
    ss <<    "<MaxCamera>1</MaxCamera>\r\n";
    ss <<    "<MaxAlarm>0</MaxAlarm>\r\n";
    ss <<    "</Response>\r\n";

    spdlog::debug("deviceinfo response: \n{}", ss.str());
    auto request = create_msg();
    if (request != NULL) {
        osip_message_set_content_type(request, "Application/MANSCDP+xml");
        osip_message_set_body(request, ss.str().c_str(), strlen(ss.str().c_str()));
        send_request(request);
    }
}

void TdDevice::process_devicecontrol_query(string sn) {

}

void TdDevice::heartbeat_task() {
	while (true) {
        if (is_register) {
            stringstream ss;
            ss << "<?xml version=\"1.0\"?>\r\n";
            ss << "<Notify>\r\n";
            ss << "<CmdType>Keepalive</CmdType>\r\n";
            ss << "<SN>" << get_sn() << "</SN>\r\n";
            ss << "<DeviceID>" << device_sip_id << "</DeviceID>\r\n";
            ss << "<Status>OK</Status>\r\n";
            ss << "</Notify>\r\n";

            osip_message_t* request = create_msg();
            if (request != NULL) {
                osip_message_set_content_type(request, "Application/MANSCDP+xml");
                osip_message_set_body(request, ss.str().c_str(), strlen(ss.str().c_str()));
                send_request(request);
                spdlog::debug("sent heartbeat");
            }
        }

		std::this_thread::sleep_for(std::chrono::seconds(60));
	}
}

osip_message_t * TdDevice::create_msg() {

    osip_message_t * request = nullptr;
    auto status = eXosip_message_build_request(sip_context, &request, "MESSAGE", to_sip.c_str(), from_sip.c_str(), nullptr);
    if (OSIP_SUCCESS != status) {
        spdlog::error("build request failed: {}", status);
    }

    return request;
}

void TdDevice::send_request(osip_message_t * request) {
    eXosip_lock(sip_context);
    eXosip_message_send_request(sip_context, request);
    eXosip_unlock(sip_context);
}

void TdDevice::send_response(shared_ptr<eXosip_event_t> evt, osip_message_t * msg) {
    eXosip_lock(sip_context);
    eXosip_message_send_answer(sip_context, evt->tid, 200, msg);
    eXosip_unlock(sip_context);
}

void TdDevice::send_response_ok(shared_ptr<eXosip_event_t> evt) {
    auto msg = evt->request;
    eXosip_message_build_answer(sip_context, evt->tid, 200, &msg);
    send_response(evt, msg);
}

void TdDevice::send_response_err(shared_ptr<eXosip_event_t> evt) {
    //auto msg = evt->request;
    //eXosip_message_build_answer(sip_context, evt->tid, 403, nullptr);
    //send_response(evt, msg);
    eXosip_message_send_answer(sip_context, evt->tid, 403, nullptr);
}

std::tuple<string, string> TdDevice::get_cmd(const char * body) {
    pugi::xml_document document;

    if (!document.load(body)) {
        spdlog::error("cannot parse the xml");
        return make_tuple("", "");
    }

    pugi::xml_node root_node = document.first_child();

    if (!root_node) {
        spdlog::error("cannot get root node of xml");
        return make_tuple("", "");
    }

    string root_name = root_node.name();
    if ("Query" != root_name) {
        spdlog::error("invalid query xml with root: {}", root_name);
        return make_tuple("", "");
    }

    auto cmd_node = root_node.child("CmdType");

    if (!cmd_node) {
        spdlog::error("cannot get the cmd type");
        return make_tuple("", "");
    }

    auto sn_node = root_node.child("SN");

    if (!sn_node) {
        spdlog::error("cannot get the SN");
        return make_tuple("", "");
    }

    string cmd = cmd_node.child_value();
    string sn = sn_node.child_value();

    return make_tuple(cmd, sn);
}

std::string TdDevice::getValueByNodeName(const char * body,std::string nodeName)
{
    pugi::xml_document document;

    if (!document.load(body)) {
        spdlog::error("cannot parse the xml");
        return "";
    }

    pugi::xml_node root_node = document.first_child();

    if (!root_node) {
        spdlog::error("cannot get root node of xml");
        return "";
    }

    string root_name = root_node.name();
    if ("Query" != root_name) {
        spdlog::error("invalid query xml with root: {}", root_name);
        return "";
    }
    auto dest_node = root_node.child(nodeName.c_str());

    if (!dest_node) {
        spdlog::error("cannot get the {}",nodeName);
        return "";
    }
    return dest_node.child_value();
}

std::string TdDevice::getValueByAttrKey(const char * sdp,std::string AttrKey)
{
    if(!sdp)
    {
        spdlog::error("sdp is null");
        return "";
    }
   const char* sataPtr = strstr(sdp,AttrKey.c_str());
   printf("sataPtr:%s\r\n",sataPtr);
   return "";
}

int TdDevice::createSession(std::string channle,std::string dest,int destPort,int basePort)
{
    return TdChanManager::getInstance()->createChannel(channle,dest,destPort,basePort);
}

int TdDevice::destorySession(std::string channle)
{
    return  TdChanManager::getInstance()->delChannel(channle);
}