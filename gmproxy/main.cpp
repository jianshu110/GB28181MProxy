#include "inc/TdRtp.h"
#include "inc/TdRedis.h"
#include "inc/TdDevice.h"
#include <unistd.h>
// TdRtp gRtp ;
int main(){
    // if(gRtp.setUp("192.168.11.126",30002,30004)<0)
    // {
    //     printf("setUp is error\n");
    //     return -1 ;
    // }
    // gRtp.start(1);
    // rs.subcrsiberConnect();
    auto device = shared_ptr<TdDevice>(
        new TdDevice("34020000002000000001", "192.168.11.195", 15060, 
            "31011500991320000046", "admin", "admin123", 5060, "TDWL")
        );
    device->start();
    return 0;
}