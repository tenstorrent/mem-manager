#include <cinttypes>
#include <vector>
#include <string>

class mem { 

    public:

        typedef std::uint64_t        addr_t ;
        typedef std::uint64_t        sz_t   ;
        typedef std::uint8_t         datum_t;
        typedef std::vector<datum_t> data_t ;

        virtual data_t read (addr_t addr, sz_t size)  =0;
        //TODO byte-enabled version
        virtual void   write(addr_t addr, const data_t& data)       =0;
        virtual bool   check(addr_t addr, const data_t& data) const =0;

        // C versions
        virtual void read (addr_t addr, sz_t size,       datum_t* data)       =0;
        virtual void write(addr_t addr, sz_t size, const datum_t* data)       =0;
        virtual bool check(addr_t addr, sz_t size, const datum_t* data) const =0;

        void   load_ELF(const std::string& filename);
        void   load_verilog_hex(const std::string& filename);

        virtual ~mem() {}

};
