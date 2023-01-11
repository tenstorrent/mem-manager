#pragma once

#include "mem.h"
#include <unordered_map>
#include <algorithm>

class sparse_mem : public mem {

    public:

        sparse_mem();

        using mem::read;
        using mem::write;
        using mem::check;

        bool read (addr_t addr, sz_t size,       datum_t* data)       override;
        void write(addr_t addr, sz_t size, const datum_t* data)       override;
        bool check(addr_t addr, sz_t size, const datum_t* data) const override;

    private:

        // FIXME
        std::unordered_map<addr_t, datum_t> mem_;

};
