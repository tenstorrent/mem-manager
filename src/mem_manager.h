#pragma once

#include "sparse_mem.h"

extern "C" {

    using addr_t  = mem::addr_t ;
    using sz_t    = mem::sz_t   ;
    using datum_t = mem::datum_t;
    using data_t  = mem::data_t ;

    class mem_manager;

    mem_manager* mem_manager_create();
    void mem_manager_destroy         (mem_manager* mm);
    void mem_manager_read            (mem_manager* mm, addr_t addr, sz_t size, datum_t* data);
    void mem_manager_write           (mem_manager* mm, addr_t addr, sz_t size, datum_t* data);
    std::uint8_t mem_manager_check   (mem_manager* mm, addr_t addr, sz_t size, datum_t* data);
    void mem_manager_load_ELF        (mem_manager* mm, const char* filename);
    void mem_manager_load_verilog_hex(mem_manager* mm, const char* filename);
}

class mem_manager : public sparse_mem {

    public:

        mem_manager();

        // TODO
        //addr_t alloc(/* size, addr constraints */);

};
