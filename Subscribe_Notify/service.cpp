#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <thread>
#include <condition_variable>
#include <chrono>


#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_METHOD_ID 0x0421
#define SAMPLE_EVENTGROUP_ID 300
#define SAMPLE_EVENTGROUP_EXTRA_ID 100
#define SAMPLE_EVENT_ID 0x8001
#define SAMPLE_EVENT_EXTRA_ID 0x8002
#define SAMPLE_SERVICE_EXTRA_ID 0x4321

std::mutex mutex;
std::condition_variable condition;
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


void on_availability(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available){
    if(_is_available == true && _service == static_cast<vsomeip::service_t>(SAMPLE_SERVICE_EXTRA_ID)
        && _instance == static_cast<vsomeip::service_t>(SAMPLE_INSTANCE_ID)) {
        std::cout << "Service " <<std::hex << _service << ":" << _instance << " is AVAILABLE" <<std::endl;
    }
    condition.notify_one();
}


void subscribe(){
    std::unique_lock<std::mutex> its_lock(mutex);
    condition.wait(its_lock);

    std::cout << "SUBSCRIBE" << std::endl;

    std::set<vsomeip::eventgroup_t> its_group;
    its_group.insert(SAMPLE_EVENTGROUP_EXTRA_ID);
    app->request_event(SAMPLE_SERVICE_EXTRA_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_EXTRA_ID, its_group);
    app->subscribe(SAMPLE_SERVICE_EXTRA_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENTGROUP_EXTRA_ID);
}


void message_hanlder (const std::shared_ptr<vsomeip::message> & _message){

    std::cout << "Message received: " <<std::endl;

    std::shared_ptr<vsomeip::payload> payload = _message->get_payload();   
    vsomeip::byte_t * data = payload->get_data();

    std::vector< vsomeip::byte_t > data_stream; 

    for (vsomeip::length_t i=0; i != payload->get_length(); ++i){
        data_stream.push_back(*data);
        data++;
    }

    for (const auto i : data_stream){
        std::cout << std::hex << static_cast<int>(i) << ' ';
    }
    std::cout << std::endl;
}


int main() {

    app = vsomeip::runtime::get()->create_application("Service");
    app->init();
    app->offer_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
    app->request_service(SAMPLE_SERVICE_EXTRA_ID, SAMPLE_INSTANCE_ID);
    app->register_availability_handler(SAMPLE_SERVICE_EXTRA_ID, SAMPLE_INSTANCE_ID, on_availability);
    app->register_message_handler(vsomeip::ANY_SERVICE, vsomeip::ANY_INSTANCE, vsomeip::ANY_METHOD,message_hanlder);

    std::thread request_th{[&]
       {
            while(1){                
                subscribe();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
       }
    };

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
