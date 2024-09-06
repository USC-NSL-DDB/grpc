#pragma once

#include <iostream>
#include <fstream>

#include "cereal/archives/json.hpp"
#include "cereal/types/string.hpp"
#include "ddb/backtrace.h"

// namespace DDB{
//     static inline void serialize(DDBCallerMeta& data) {
//         std::stringstream ss;
//         ss << data.caller_comm_ip << "," << 
//     }

//     template <class Archive>
//     static inline void serialize(Archive & ar, DDBCallerContext& data) {
//     }
    
//     template <class Archive>
//     static inline void serialize(DDBTraceMeta& data) {
//     }

//     static inline std::string serialize_to_str(const DDBTraceMeta& data) {
//     }

//     static inline DDBTraceMeta deserialize_from_str(const std::string& data) {
//         DDBTraceMeta meta;
//         std::istringstream is(data, std::ios::binary); 
//         cereal::JSONInputArchive archive(is);        
//         archive(cereal::make_nvp("data", data));                                  
//         return meta;           
//     }
// }

// namespace DDB
// {
// } // namespace DDB
