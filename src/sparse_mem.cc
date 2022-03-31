#include "sparse_mem.h"

sparse_mem::sparse_mem() {}

void sparse_mem::read(addr_t addr, sz_t size, datum_t* data) {

    struct gen {
        decltype(mem_)& mem;
        addr_t addr;
        gen(decltype(mem)& m, addr_t a) : mem(m), addr(a) {}
        datum_t operator()() {
            return mem[addr++];
        }
    };

    std::generate_n(data, size, gen(mem_, addr));

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
