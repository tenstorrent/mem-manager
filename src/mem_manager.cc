#include "mem_manager.h"

mem_manager::mem_manager() {}

extern "C" {

    using addr_t  = mem_manager::addr_t ;
    using sz_t    = mem_manager::sz_t   ;
    using datum_t = mem_manager::datum_t;
    using data_t  = mem_manager::data_t ;

    mem_manager* mem_manager_create() {

        return new mem_manager();

    };

    void mem_manager_destroy         (mem_manager* mm) {

        delete mm;

    }

    void mem_manager_read            (mem_manager* mm, addr_t addr, sz_t size, datum_t* data) {

        mm->read(addr, size, data);

    }

    void mem_manager_write           (mem_manager* mm, addr_t addr, sz_t size, datum_t* data) {

        mm->write(addr, size, data);

    }

    void mem_manager_check           (mem_manager* mm, addr_t addr, sz_t size, datum_t* data) {

        mm->check(addr, size, data);

    }

    void mem_manager_load_ELF        (mem_manager* mm, const char* filename) {

        mm->load_ELF(filename);

    }

    void mem_manager_load_verilog_hex(mem_manager* mm, const char* filename) {

        mm->load_verilog_hex(filename);

    }
}
