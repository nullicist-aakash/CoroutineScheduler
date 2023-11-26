#include "network_import.h"

using namespace std;

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

string get_err_str()
{
#if WINDOWS
    DWORD error = ::GetLastError();
    return system_category().message(error);
#elif UNIX
    return strerror(errno);
#endif
    return {};
}