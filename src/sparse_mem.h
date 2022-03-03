#include <cinttypes>
#include <vector>
#include <unordered_map>

class sparse_mem {

    public:

        typedef std::uint64_t        addr_t ;
        typedef std::uint64_t        sz_t   ;
        typedef std::uint8_t         datum_t;
        typedef std::vector<datum_t> data_t ;

        sparse_mem();

        data_t read (addr_t addr, sz_t size);
        //TODO byte-enabled version
        void   write(addr_t addr, const data_t& data);
        bool   check(addr_t addr, const data_t& data);

    private:

        // FIXME
        std::unordered_map<addr_t, datum_t> mem_;
};
