#pragma once

#include <iostream>
#include <fstream>

#include "cereal/archives/binary.hpp"
#include "cereal/types/string.hpp"
#include "ddb/backtrace.h"

namespace cereal {
    template <class Archive>
    inline void serialize(Archive & ar, DDBCallerMeta& data) {
        ar(data.caller_comm_ip);
        ar(data.pid);
    }

    template <class Archive>
    inline void serialize(Archive & ar, DDBCallerContext& data) {
        ar(data.rbp);
        ar(data.rip);
        ar(data.rsp);
    }
    
    template <class Archive>
    inline void serialize(Archive & ar, DDBTraceMeta& data) {
        ar(data.magic);
        ar(data.meta);
        ar(data.ctx);
    }
}

namespace DDB
{
    static inline std::string serialize(const DDBTraceMeta& data) {
        std::ostringstream os(std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        archive(data);
        return os.str();
    }

    static inline DDBTraceMeta deserialize(const std::string& data) {
        DDBTraceMeta meta;
        std::istringstream is(data, std::ios::binary); 
        cereal::BinaryInputArchive archive(is);        
        archive(meta);                                 
        return meta;           
    }
} // namespace DDB

