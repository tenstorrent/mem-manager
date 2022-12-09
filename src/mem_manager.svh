`define _MEM_MANAGER_RETURN_void
`define _MEM_MANAGER_RETURN_byte return

`define MEM_MANAGER_IMPORT_SIZED_FUNCTIONS(SIZE)         \
    `MEM_MANAGER_IMPORT_SIZED_FUNCTION(SIZE,read,output, void) \
    `MEM_MANAGER_IMPORT_SIZED_FUNCTION(SIZE,write,input, void) \
    `MEM_MANAGER_IMPORT_SIZED_FUNCTION(SIZE,check,input, byte)

`define MEM_MANAGER_IMPORT_SIZED_FUNCTION(SIZE,NAME,IO,RETURN)                                                                                           \
    import "DPI-C" function RETURN mem_manager_``NAME``_``SIZE (mm_t mm, addr_t addr, sz_t size, IO datum_t data[SIZE]);  \
    function RETURN NAME``_``SIZE(mm_t mm, addr_t addr, IO datum_t data[SIZE]);                                                       \
        `_MEM_MANAGER_RETURN_``RETURN mem_manager_``NAME``_``SIZE(mm, addr, SIZE, data);                                                                                   \
    endfunction                                                                                                                                   \
                                                                                                                                                  \
    function RETURN NAME``_``SIZE``_sized(mm_t mm, addr_t addr, sz_t size, IO datum_t data[SIZE]);                                    \
        check_size: assert (size <= SIZE) else $error("size %0d bigger than SIZE", size);                                                                     \
        if (size <= SIZE) begin                                                                                                                   \
            `_MEM_MANAGER_RETURN_``RETURN mem_manager_``NAME``_``SIZE(mm, addr, size, data);                                                                               \
        end                                                                                                                                       \
    endfunction

package mem_manager;

    typedef chandle          mm_t    ;
    typedef longint unsigned addr_t  ;
    typedef longint unsigned sz_t    ;
    typedef byte    unsigned datum_t ;

    import "DPI-C" mem_manager_create           = function chandle create            ();
    import "DPI-C" mem_manager_destroy          = function void    destroy           (mm_t mm);
    import "DPI-C" mem_manager_load_ELF         = function void    load_ELF          (mm_t mm, string filename);
    import "DPI-C" mem_manager_load_verilog_hex = function void    load_verilog_hex  (mm_t mm, string filename);

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

`undef _MEM_MANAGER_RETURN_void
`undef _MEM_MANAGER_RETURN_bit
`undef MEM_MANAGER_IMPORT_SIZED_FUNCTIONS
`undef MEM_MANAGER_IMPORT_SIZED_FUNCTION
