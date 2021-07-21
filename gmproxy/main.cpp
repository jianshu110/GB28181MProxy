#include "inc/TdDevice.h"
int main(){
    
    char logo[] = "\n\
  ____ __  __ ____\n\
 / ___|  \\/  |  _ \\ _ __ _____  ___   _\n\
| |  _| |\\/| | |_) | '__/ _ \\ \\/ / | | |\n\
| |_| | |  | |  __/| | | (_) >  <| |_| |\n\
 \\____|_|  |_|_|   |_|  \\___/_/\\_\\__,  |\n\
                                  |___/\n\n";
    printf("%s",logo);
    auto device = shared_ptr<TdDevice>(
        new TdDevice("34020000002000000001", "192.168.11.195", 15060, 
            "31011500991320000046", "admin", "admin123", 5060, "TDWL")
        );
    device->start();
    return 0;
}