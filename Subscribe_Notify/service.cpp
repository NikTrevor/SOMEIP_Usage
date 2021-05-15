#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <thread>

#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_METHOD_ID 0x0421
#define SAMPLE_EVENTGROUP_ID 300
#define SAMPLE_EVENT_ID 0x8001

std::shared_ptr< vsomeip::application > app;

void notify(){
    std::cout << "NOTIFY" <<std::endl;
    std::shared_ptr<vsomeip::payload> payload;
    const vsomeip::byte_t its_data[] = { 0x10, 0x20, 0x30, 0x40, 0x50 };
    payload = vsomeip::runtime::get()->create_payload();
    payload->set_data(its_data, sizeof(its_data));

    std::set<vsomeip::eventgroup_t> its_groups;
    its_groups.insert(SAMPLE_EVENTGROUP_ID);
    app->offer_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, its_groups);
    app->notify(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, payload);
}

int main() {

    app = vsomeip::runtime::get()->create_application("Service");
    app->init();
    app->offer_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);


    std::thread notify_th{[&]
       {
            while(1){                
                notify();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                
            }
       }
    };
            
    app->start();

}