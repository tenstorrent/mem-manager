#include "mem.h"
#include <unordered_map>
#include <algorithm>

class sparse_mem : public mem {

    public:

        sparse_mem();

        data_t read (addr_t addr, sz_t size)                override;
        void   write(addr_t addr, const data_t& data)       override;
        bool   check(addr_t addr, const data_t& data) const override;

        void read (addr_t addr, sz_t size,       datum_t* data)       override;
        void write(addr_t addr, sz_t size, const datum_t* data)       override;
        bool check(addr_t addr, sz_t size, const datum_t* data) const override;

    private:

        // FIXME
        std::unordered_map<addr_t, datum_t> mem_;

        struct gen {
            decltype(mem_)& mem;
            addr_t addr;
            gen(decltype(mem)& m, addr_t a) : mem(m), addr(a) {}
            datum_t operator()() {
                return mem[addr++];
            }
        };

        template <typename ForwardIt>
            void readt (addr_t addr, sz_t size, ForwardIt datum) {
                std::generate_n(datum, size, gen(mem_, addr));
            }

        template <typename ForwardIt>
            void writet (addr_t addr, sz_t size, ForwardIt datum) {

                for (sz_t s = 0; s < size; s++) {
                    mem_[addr + s] = *datum;
                    datum++;
                }

            }

        template <typename ForwardIt>
            bool checkt (addr_t addr, sz_t size, ForwardIt datum) const {

                for (sz_t s = 0; s < size; s++) {
                    auto it = mem_.find(addr++);
                    if(it == mem_.end() || it->second != *datum) {
                        return false;
                    }
                    datum++;
                }

                return true;
            }
};
