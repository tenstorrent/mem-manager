#include "mem_manager.h"

mem_manager::mem_manager(const opts& o) : sparse_mem(o.page_size) {}

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

    std::uint8_t mem_manager_read            (mem_manager* mm, addr_t addr, sz_t size, datum_t* data) {

        return mm->read(addr, size, data);

    }

    void mem_manager_write           (mem_manager* mm, addr_t addr, sz_t size, datum_t* data) {

        mm->write(addr, size, data);

    }

    std::uint8_t mem_manager_check           (mem_manager* mm, addr_t addr, sz_t size, datum_t* data) {

        return mm->check(addr, size, data);

    }

    void mem_manager_load_ELF        (mem_manager* mm, const char* filename) {

        mm->load_ELF(filename);

    }

    void mem_manager_load_verilog_hex(mem_manager* mm, const char* filename) {

        mm->load_verilog_hex(filename);

    }

    void mem_manager_load_lz4(mem_manager* mm, const char* filename) {

        mm->load_lz4(filename);

    }


#define ALIAS_NAME_SIZED(NAME, SIZE, RETURN)                                                   \
    RETURN mem_manager_##NAME##_##SIZE (mem_manager* mm, addr_t addr, sz_t size, datum_t* data) { \
        return mem_manager_##NAME(mm, addr, size, data);                                      \
    }

#define ALIAS_SIZED(SIZE)         \
    ALIAS_NAME_SIZED(read, SIZE, std::uint8_t)  \
    ALIAS_NAME_SIZED(write, SIZE, void) \
    ALIAS_NAME_SIZED(check, SIZE, std::uint8_t)

ALIAS_SIZED(1)
ALIAS_SIZED(2)
ALIAS_SIZED(4)
ALIAS_SIZED(8)
ALIAS_SIZED(16)
ALIAS_SIZED(32)
ALIAS_SIZED(64)
ALIAS_SIZED(128)
ALIAS_SIZED(256)
ALIAS_SIZED(512)

}
