#include "sparse_mem.h"
#include <algorithm>

sparse_mem::sparse_mem() {}

sparse_mem::data_t sparse_mem::read(addr_t addr, sz_t size) {

    data_t data(size);

    struct gen {
        decltype(mem_) mem;
        addr_t addr;
        datum_t operator()() {
            return mem[addr++];
        }
    };

    std::generate(data.begin(), data.end(), gen{mem_, addr});

    return data;
}

void sparse_mem::write(addr_t addr, const data_t& data) {

    for (const auto& datum : data) {
        mem_[addr++] = datum;
    }

}

bool sparse_mem::check(addr_t addr, const data_t& data) {

    for (const auto& datum : data) {
        auto it = mem_.find(addr++);
        if(it == mem_.end() || it->second != datum) {
            return false;
        }
    }

    return true;
}
