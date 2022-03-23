#include "sparse_mem.h"

sparse_mem::sparse_mem() {}

sparse_mem::data_t sparse_mem::read(addr_t addr, sz_t size) {

    data_t data(size);
    readt(addr, size, std::begin(data));
    return data;

}

void sparse_mem::read(addr_t addr, sz_t size, datum_t* data) {

    readt(addr, size, data);

}

void sparse_mem::write(addr_t addr, const data_t& data) {

    writet(addr, data.size(), std::cbegin(data));

}

void sparse_mem::write(addr_t addr, sz_t size, const datum_t* data) {

    writet(addr, size, data);

}

bool sparse_mem::check(addr_t addr, const data_t& data) const {

    return checkt(addr, data.size(), std::cbegin(data));

}

bool sparse_mem::check(addr_t addr, sz_t size, const datum_t* data) const {

    return checkt(addr, size, data);

}
