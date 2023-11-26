#include "network_import.h"

consteval bool is_windows()
{
#if WINDOWS
    return true;
#else
    return false;
#endif
}

consteval bool is_unix()
{
#if UNIX
    return true;
#else
    return false;
#endif
}

std::string get_err_str()
{
#if WINDOWS
    DWORD error = ::GetLastError();
    return std::system_category().message(error);
#elif UNIX
    return std::strerror(errno);
#endif
    return {};
}