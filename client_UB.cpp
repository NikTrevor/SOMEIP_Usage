
#include <iostream>
#include <iomanip>
#include <sstream>
#include <condition_variable>
#include <thread>

#include <vsomeip/vsomeip.hpp>
#include <chrono>

#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_METHOD_ID 0x0421
#define SAMPLE_EVENTGROUP_ID 300
#define SAMPLE_EVENT_ID 0x8001

std::mutex mutex;
std::condition_variable condition;
std::shared_ptr< vsomeip::application > app;
vsomeip::byte_t counter = 0;

void on_availability(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available){
    if (_is_available == true && _service == static_cast<vsomeip::service_t>(SAMPLE_SERVICE_ID) && _instance == static_cast<vsomeip::service_t>(SAMPLE_INSTANCE_ID)) {
        std::cout << "Service " <<std::hex << _service << ":" << _instance << " is AVAILABLE" <<std::endl;
    }
    condition.notify_one();

} 


void run() {
    std::unique_lock<std::mutex> its_lock(mutex);
    condition.wait(its_lock);
    std::cout << "RUN" << std::endl;    


    std::shared_ptr< vsomeip::message > request;
    request = vsomeip::runtime::get()->create_request();
    request->set_service(SAMPLE_SERVICE_ID);
    request->set_instance(SAMPLE_INSTANCE_ID);
    request->set_method(SAMPLE_METHOD_ID);

    std::shared_ptr< vsomeip::payload > its_payload = vsomeip::runtime::get()->create_payload();
    std::vector< vsomeip::byte_t > its_payload_data;
    for (vsomeip::byte_t i=0; i<10; i++) {
        its_payload_data.push_back(counter);
        counter++;
    }
    its_payload->set_data(its_payload_data);
    request->set_payload(its_payload);

    app->send(request);
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

void on_message(const std::shared_ptr<vsomeip::message> &_response) {
    std::stringstream its_message;
    its_message << "CLIENT: received a notification for event ["
            << std::setw(4) << std::setfill('0') << std::hex
            << _response->get_service() << "."
            << std::setw(4) << std::setfill('0') << std::hex
            << _response->get_instance() << "."
            << std::setw(4) << std::setfill('0') << std::hex
            << _response->get_method() << "] to Client/Session ["
            << std::setw(4) << std::setfill('0') << std::hex
            << _response->get_client() << "/"
            << std::setw(4) << std::setfill('0') << std::hex
            << _response->get_session()
            << "] = ";
    std::shared_ptr<vsomeip::payload> its_payload = _response->get_payload();
    its_message << "(" << std::dec << its_payload->get_length() << ") ";
    for (uint32_t i = 0; i < its_payload->get_length(); ++i)
        its_message << std::hex << std::setw(2) << std::setfill('0')
            << (int) its_payload->get_data()[i] << " ";
    std::cout << its_message.str() << std::endl;
}

void subscribe(){
    std::unique_lock<std::mutex> its_lock(mutex);
    condition.wait(its_lock);

    std::cout << "SUBSCRIBE" <<std::endl;

    std::set<vsomeip::eventgroup_t> its_groups;
    its_groups.insert(SAMPLE_EVENTGROUP_ID);
    app->request_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, its_groups);
    app->subscribe(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENTGROUP_ID);

}

int main() {

    app = vsomeip::runtime::get()->create_application("Client");
    app->init();
    app->register_availability_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, on_availability);
    app->request_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
    //app->register_message_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID, message_hanlder);
    app->register_message_handler(vsomeip::ANY_SERVICE, vsomeip::ANY_INSTANCE, vsomeip::ANY_METHOD, message_hanlder);
    std::thread request_th{[&]
       {
            while(1){
                //run();
                subscribe();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
       }
    };
    app->start();
}
