#include <string>

namespace util {
    std::tuple<int, std::string> exec(const char* cmd);
    std::string exec_check(const char* cmd);
};
