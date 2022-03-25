`define MEM_MANAGER_IMPORT_SIZED_FUNCTIONS(SIZE)         \
    `MEM_MANAGER_IMPORT_SIZED_FUNCTION(SIZE,read,output) \
    `MEM_MANAGER_IMPORT_SIZED_FUNCTION(SIZE,write,input) \
    `MEM_MANAGER_IMPORT_SIZED_FUNCTION(SIZE,check,input)

`define MEM_MANAGER_IMPORT_SIZED_FUNCTION(SIZE,NAME,IO)                                                                                           \
    import "DPI-C" mem_manager_``NAME = function void wrap_mem_manager_``NAME``_``SIZE (mm_t mm, addr_t addr, sz_t size, IO datum_t data[SIZE]);  \
    function void mem_manager_``NAME``_``SIZE(mm_t mm, addr_t addr, IO datum_t data[SIZE]);                                                       \
        wrap_mem_manager_``NAME``_``SIZE(mm, addr, SIZE, data);                                                                                   \
    endfunction                                                                                                                                   \
                                                                                                                                                  \
    function void mem_manager_``NAME``_``SIZE``_sized(mm_t mm, addr_t addr, sz_t size, IO datum_t data[SIZE]);                                    \
        assert (size <= SIZE) else $error("size %0d bigger than SIZE", size);                                                                     \
        if (size <= SIZE) begin                                                                                                                   \
            wrap_mem_manager_``NAME``_``SIZE(mm, addr, SIZE, data);                                                                               \
        end                                                                                                                                       \
    endfunction

package mem_manager;

    typedef chandle          mm_t    ;
    typedef longint unsigned addr_t  ;
    typedef longint unsigned sz_t    ;
    typedef byte    unsigned datum_t ;

    import "DPI-C" function chandle mem_manager_create            ();
    import "DPI-C" function void    mem_manager_destroy           (mm_t mm);
    import "DPI-C" function void    mem_manager_load_ELF          (mm_t mm, string filename);
    import "DPI-C" function void    mem_manager_load_verilog_hex  (mm_t mm, string filename);

    `MEM_MANAGER_IMPORT_SIZED_FUNCTIONS(1)
    `MEM_MANAGER_IMPORT_SIZED_FUNCTIONS(2)
    `MEM_MANAGER_IMPORT_SIZED_FUNCTIONS(4)
    `MEM_MANAGER_IMPORT_SIZED_FUNCTIONS(8)
    `MEM_MANAGER_IMPORT_SIZED_FUNCTIONS(16)
    `MEM_MANAGER_IMPORT_SIZED_FUNCTIONS(32)
    `MEM_MANAGER_IMPORT_SIZED_FUNCTIONS(64)
    `MEM_MANAGER_IMPORT_SIZED_FUNCTIONS(128)
    `MEM_MANAGER_IMPORT_SIZED_FUNCTIONS(256)
    `MEM_MANAGER_IMPORT_SIZED_FUNCTIONS(512)

endpackage

`undef MEM_MANAGER_IMPORT_SIZED_FUNCTIONS
`undef MEM_MANAGER_IMPORT_SIZED_FUNCTION
