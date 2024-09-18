#pragma once

#include <iostream>

#include <string>
#include <cstdint>
#include <sched.h>
#include <unistd.h>
#include <functional>

#include "ddb/common.hpp"

/// @brief  Added magic number for testing DDBTraceMeta 
#define T_META_MATIC 12345ULL
// constexpr static uint64_t tMetaMagic = 12345;

// typedef struct {
//   uint32_t caller_comm_ip;
//   pid_t pid;
// } __attribute__((packed)) DDBCallerMeta;

// typedef struct {
//   uintptr_t rip;
//   uintptr_t rsp;
//   uintptr_t rbp;
// } __attribute__((packed)) DDBCallerContext;

// /// @brief  Added data structure for backtrace
// typedef struct {
//   uint64_t magic;
//   DDBCallerMeta meta;
//   DDBCallerContext ctx;
// } __attribute__((packed)) DDBTraceMeta;

namespace DDB {
struct DDBCallerMeta {
  uint32_t caller_comm_ip = 0;
  pid_t pid = 0;
};

// struct DDBCallerContext {
//   uintptr_t rip = 0;
//   uintptr_t rsp = 0;
//   uintptr_t rbp = 0;
// };

struct DDBCallerContext {
  uintptr_t pc = 0;  // Program Counter
  uintptr_t sp = 0;  // Stack Pointer
  uintptr_t fp = 0;  // Frame Pointer
  #ifdef __aarch64__
  uintptr_t lr = 0;  // Link Register (only on ARM64)
  #endif
};

/// @brief  Added data structure for backtrace
struct DDBTraceMeta {
  uint64_t magic = 0;
  DDBCallerMeta meta;
  DDBCallerContext ctx;
};

static inline __attribute__((always_inline)) void get_context(DDBCallerContext* ctx) { 
  // void *rsp;
  // void *rbp;

  // // Fetch the current stack pointer (RSP)
  // asm volatile ("mov %%rsp, %0" : "=r" (rsp));

  // // Fetch the current base pointer (RBP)
  // asm volatile ("mov %%rbp, %0" : "=r" (rbp));

  // uintptr_t _rsp = (uintptr_t) rsp;
  // uintptr_t _rip = (uintptr_t) __builtin_return_address(0); // Approximation to get RIP
  // uintptr_t _rbp = (uintptr_t) rbp;

  void* sp;
#if defined(__x86_64__)
  asm volatile("mov %%rsp, %0" : "=r" (sp));
#elif defined(__aarch64__)
  asm volatile("mov %0, sp" : "=r" (sp));
#else
  #error "Unsupported architecture"
#endif

  ctx->sp = (uintptr_t) sp;
  ctx->pc = (uintptr_t) __builtin_return_address(0);
  ctx->fp = (uintptr_t) __builtin_frame_address(0);

#ifdef __aarch64__
  // For Link Register on ARM64, we still need inline assembly
  void *lr;
  asm volatile ("mov %0, x30" : "=r" (lr));
  ctx->lr = (uintptr_t)lr;
#endif

  // std::cout << "rsp = " << _rsp << ", rip = " << _rip << ", rbp = " << _rbp << std::endl;
  // std::cout << "sp = " << ctx->sp << ", pc = " << ctx->pc << ", fp = " << ctx->fp << std::endl;
}

static inline __attribute__((always_inline)) void __get_caller_meta(DDBCallerMeta* meta) {
  meta->caller_comm_ip = ddb_meta.comm_ip;
  meta->pid = getpid();
}

static inline __attribute__((always_inline)) void get_trace_meta(DDBTraceMeta* trace_meta) {
  trace_meta->magic = T_META_MATIC;
  __get_caller_meta(&trace_meta->meta);
  get_context(&trace_meta->ctx);
}

namespace Backtrace {
  template<typename RT = void, class RPCCallable>
  __attribute__((noinline))
  static RT extraction(std::function<DDBTraceMeta()> extractor, RPCCallable&& rpc_callable) {
    __attribute__((used)) DDBTraceMeta meta;
    if (extractor) {
      meta = extractor();
    }
    if constexpr (!std::is_void_v<RT>) {
      return rpc_callable();
    } else {
      rpc_callable();
    }
  }
} // namespace Backtrace
} // namespace DDB
