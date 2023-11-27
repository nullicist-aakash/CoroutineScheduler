#include "network_import.h"

using namespace std;

string get_err_str()
{
#ifdef WINDOWS
    DWORD error = ::GetLastError();
    return system_category().message(error);
#elif defined(UNIX)
    return strerror(errno);
#endif
    return {};
}