#include "service_registry.h"
#include "service.h"

namespace wjp{

void service_registry::unregister_service(str name)
{
    service_lookup.erase(name);
}

// overwrite value if key exists
void service_registry::register_service(std::shared_ptr<service> service_)
{
    service_lookup[name]=service_;
}

std::shared_ptr<service> service_registry::get_service(str name)
{
    if(service_lookup.count(name)){
        return service_lookup[name];
    }
}

void service_registry::start_all()
{
    for(auto it = service_lookup.begin();it!=service_lookup.end();it++){
        it->second->start();
    }
}

}