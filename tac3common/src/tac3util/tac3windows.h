#pragma once
#include <string>
#include <time.h>

namespace Tac
{
  time_t GetLastModified(
    const std::string & filepath,
    std::string & errors);

}
