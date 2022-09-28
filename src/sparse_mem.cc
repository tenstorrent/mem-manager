#include "sparse_mem.h"

sparse_mem::sparse_mem() {}

void sparse_mem::read(addr_t addr, sz_t size, datum_t* data) {

    struct gen {
        sparse_mem& s;
        addr_t addr;
        gen(sparse_mem& s, addr_t a) : s(s), addr(a) {}
        datum_t operator()() {
            auto it = s.mem_.find(addr);
            if (it == s.mem_.end()) {
                it = s.mem_.emplace(addr, s.uninitialized_read(addr, 1).at(0)).first;
            }
            addr++;
            return it->second;
        }
    };

    std::generate_n(data, size, gen(*this, addr));

}

void sparse_mem::write(addr_t addr, sz_t size, const datum_t* data) {

    for (sz_t s = 0; s < size; s++) {
        mem_[addr + s] = *data;
        data++;
    }

}

bool sparse_mem::check(addr_t addr, sz_t size, const datum_t* data) const {

    for (sz_t s = 0; s < size; s++) {
        auto it = mem_.find(addr++);
        if(it == mem_.end() || it->second != *data) {
            return false;
        }
        data++;
    }

    return true;

}
