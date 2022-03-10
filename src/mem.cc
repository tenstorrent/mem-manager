#include "mem.h"
#include "util.h"
#include <cstdlib>
#include <cstdio>
#include <stdexcept>
#include <functional>
#include <fstream>
#include <limits>

void mem::load_ELF(const std::string& filename) {

    struct tmpfile {
        std::string tmp = std::tmpnam(nullptr);
        ~tmpfile() {
            std::remove(tmp.c_str());
        }
    } tmp;

    std::string cmd = std::string("objcopy -O verilog ") + filename + " " + tmp.tmp;
    util::exec_check(cmd.c_str());

    load_verilog_hex(tmp.tmp);
}

void mem::load_verilog_hex(const std::string& filename) {
    bool   addr_found = false;
    addr_t addr;
    data_t data;

    std::ifstream file(filename);

    std::string s;
    while (file >> s) {
        if (s[0] == '@') {
            if (addr_found) {
                write(addr, data);
            }
            data.clear();

            addr = std::strtoull(s.c_str() + 1, nullptr, 16);
            addr_found = true;
        } else {
            if (!addr_found) {
                std::runtime_error("Couldn't parse " + filename);
            }

            unsigned long long d = std::strtoull(s.c_str(), nullptr, 16);
            if (d > std::numeric_limits<datum_t>::max()) {
                std::runtime_error("Invalid value " + s);
            }

            data.push_back(d);
        }
    }

    if (addr_found) {
        write(addr, data);
    }
}
