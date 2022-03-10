#include <unordered_map>
#include "mem.h"

class sparse_mem : public mem {

    public:

        sparse_mem();

        data_t read (addr_t addr, sz_t size) override;
        void   write(addr_t addr, const data_t& data) override;
        bool   check(addr_t addr, const data_t& data) override;

    private:

        // FIXME
        std::unordered_map<addr_t, datum_t> mem_;
};
