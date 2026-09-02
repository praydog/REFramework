#pragma once

#include <cstdint>
#include <safetyhook/context.hpp>
#include <bddisasm.h>

namespace disasm_utils {

template <typename T = uint64_t>
T get_register_value(safetyhook::Context& context, int reg) {
    switch (reg) {
    case NDR_RAX: return (T)context.rax;
    case NDR_RCX: return (T)context.rcx;
    case NDR_RDX: return (T)context.rdx;
    case NDR_RBX: return (T)context.rbx;
    case NDR_RSP: return (T)context.rsp;
    case NDR_RBP: return (T)context.rbp;
    case NDR_RSI: return (T)context.rsi;
    case NDR_RDI: return (T)context.rdi;
    case NDR_R8:  return (T)context.r8;
    case NDR_R9:  return (T)context.r9;
    case NDR_R10: return (T)context.r10;
    case NDR_R11: return (T)context.r11;
    case NDR_R12: return (T)context.r12;
    case NDR_R13: return (T)context.r13;
    case NDR_R14: return (T)context.r14;
    case NDR_R15: return (T)context.r15;
    default: return (T)0;
    }
}

template <typename T>
void set_register_value(safetyhook::Context& context, int reg, T value) {
    switch (reg) {
    case NDR_RAX: context.rax = (uint64_t)value; break;
    case NDR_RCX: context.rcx = (uint64_t)value; break;
    case NDR_RDX: context.rdx = (uint64_t)value; break;
    case NDR_RBX: context.rbx = (uint64_t)value; break;
    case NDR_RSP: context.rsp = (uint64_t)value; break;
    case NDR_RBP: context.rbp = (uint64_t)value; break;
    case NDR_RSI: context.rsi = (uint64_t)value; break;
    case NDR_RDI: context.rdi = (uint64_t)value; break;
    case NDR_R8:  context.r8 = (uint64_t)value; break;
    case NDR_R9:  context.r9 = (uint64_t)value; break;
    case NDR_R10: context.r10 = (uint64_t)value; break;
    case NDR_R11: context.r11 = (uint64_t)value; break;
    case NDR_R12: context.r12 = (uint64_t)value; break;
    case NDR_R13: context.r13 = (uint64_t)value; break;
    case NDR_R14: context.r14 = (uint64_t)value; break;
    case NDR_R15: context.r15 = (uint64_t)value; break;
    }
}

inline const char* register_name(int reg) {
    switch (reg) {
    case NDR_RAX: return "RAX";
    case NDR_RCX: return "RCX";
    case NDR_RDX: return "RDX";
    case NDR_RBX: return "RBX";
    case NDR_RSP: return "RSP";
    case NDR_RBP: return "RBP";
    case NDR_RSI: return "RSI";
    case NDR_RDI: return "RDI";
    case NDR_R8:  return "R8";
    case NDR_R9:  return "R9";
    case NDR_R10: return "R10";
    case NDR_R11: return "R11";
    case NDR_R12: return "R12";
    case NDR_R13: return "R13";
    case NDR_R14: return "R14";
    case NDR_R15: return "R15";
    default: return "UNKNOWN";
    }
}

} // namespace disasm_utils
