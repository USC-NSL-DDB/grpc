#ifndef DDB_INTEGRATION_H
#define DDB_INTEGRATION_H

#include <iostream>
#include <string>

#define DEFINE_DDB_META
#include <ddb/common.h>
#include <ddb/basic.h>
#include <ddb/service_reporter.h>

namespace DDB
{
    class DDBConnector {
     public:

        inline void deinit_discovery() {
            int ret_val = service_reporter_deinit(&reporter);
            if (ret_val)
                std::cerr << "failed to deinit service reporter" << std::endl;
        }

        inline void deinit() {
            if (this->discovery)
                this->deinit_discovery();
        }

        inline void init_discovery() {
            auto service = ServiceInfo {
                .ip = ddb_meta.comm_ip,
                .tag = (char*)"proc",
                .pid = getpid()
            };

            if (service_reporter_init(&reporter) != 0) {
                std::cerr << "failed to initialize service reporter" << std::endl;
            } else {
                if (report_service(&reporter, &service) != 0) {
                    std::cerr << "failed to report new service" << std::endl;
                }
            }
            this->discovery = true;
        }

        inline void init(const std::string& iface, bool enable_discovery = true) {
            populate_ddb_metadata(iface.c_str());
            if (enable_discovery)
                this->init_discovery();
            this->discovery = enable_discovery;
        }

        DDBConnector() = default;
        ~DDBConnector() {
            this->deinit();
        }

     private:
        DDBServiceReporter reporter;
        bool discovery;
    }
    
} // namespace DDB


namespace 
{
    struct Initializer {

        Initializer() {

        }

        ~Initializer() {
        }
    };

    inline Initializer global_init;
} 

#endif  // DDB_INTEGRATION_H