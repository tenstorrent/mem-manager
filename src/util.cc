#include "util.h"

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <tuple>

// https://stackoverflow.com/a/478960
std::tuple<int, std::string> util::exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    int code = -1;
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    code = pclose(pipe);
    return std::make_tuple(code, result);
}

std::string util::exec_check(const char* cmd) {
    int exit;
    std::string result;
    std::tie(exit, result) = exec(cmd);

    if (WEXITSTATUS(exit)) {
        throw std::runtime_error(std::string("`") + cmd + "` exited with " + std::to_string(exit));
    }

    return result;
}
