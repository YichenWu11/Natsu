#pragma once

#include "natsu_runtime/core/log/log_system.h"
#include "natsu_runtime/function/global/global_context.h"

#define LOG_TRACE(...) g_runtime_global_context.m_logger_system->getLogger()->trace("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_INFO(...) g_runtime_global_context.m_logger_system->getLogger()->info("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_WARN(...) g_runtime_global_context.m_logger_system->getLogger()->warn("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_ERROR(...) g_runtime_global_context.m_logger_system->getLogger()->error("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_FATAL(...) g_runtime_global_context.m_logger_system->getLogger()->fatal("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)
#define LOG_CRITICAL(...) g_runtime_global_context.m_logger_system->getLogger()->critical("[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__)

#ifdef NDEBUG
#define ASSERT(statement, msg)
#else
#define ASSERT(statement, msg) \
    if (!(statement)) { \
        LOG_ERROR("{0}", msg); \
        __debugbreak(); \
    }
#endif

#define BIT(x) (1 << x)

// https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html
#define NATSU_XSTR(s) NATSU_STR(s)
#define NATSU_STR(s) #s

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
