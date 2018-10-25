#pragma once

#include "qaio.h"

namespace wjp{
class service;
class service_registry{
protected:
    // @todo add limits to total nr of threads
    service_registry(){}
public:
    // global single instance
    static service_registry& me() 
    {
        static service_registry instance_;
        return instance_;
    }
    void unregister_service(str name);
    // overwrite value if key exists
    void register_service(std::shared_ptr<service> service_);
    std::shared_ptr<service> get_service(str name);
    void start_all();
private:
    std::unordered_map<std::string, std::shared_ptr<service>> service_lookup;
};


}