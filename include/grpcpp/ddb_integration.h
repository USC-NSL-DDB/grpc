#ifndef DDB_INTEGRATION_H
#define DDB_INTEGRATION_H

#include <iostream>

#define DEFINE_DDB_META
#include <ddb/common.h>
#include <ddb/basic.h>
#include <ddb/service_reporter.h>

namespace 
{
    struct Initializer {
        DDBServiceReporter reporter;

        Initializer() {
            populate_ddb_metadata("ens1f1");

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
        }

        ~Initializer() {
            int ret_val = service_reporter_deinit(&reporter);
            if (ret_val)
                std::cerr << "failed to deinit service reporter" << std::endl;
        }
    };

    inline Initializer global_init;
} 

#endif  // DDB_INTEGRATION_H