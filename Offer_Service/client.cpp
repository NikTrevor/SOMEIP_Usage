
#include <iostream>
#include <vsomeip/vsomeip.hpp>


#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678

std::shared_ptr< vsomeip::application > app;


void on_availability(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available){
    if (_is_available == true && _service == static_cast<vsomeip::service_t>(SAMPLE_SERVICE_ID) && _instance == static_cast<vsomeip::service_t>(SAMPLE_INSTANCE_ID)) {
        std::cout << "Service " <<std::hex << _service << ":" << _instance << " is AVAILABLE" <<std::endl;
    }
} 


int main() {

    app = vsomeip::runtime::get()->create_application("Client");
    app->init();
    app->request_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
    app->register_availability_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, on_availability);    
    app->start();
}