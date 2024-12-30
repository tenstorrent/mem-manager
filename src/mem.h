#pragma once

#include <cinttypes>
#include <vector>
#include <string>
#include <functional>

class mem { 

    public:

        typedef std::uint64_t        addr_t ;
        typedef std::uint64_t        sz_t   ;
        typedef std::uint8_t         datum_t;
        typedef std::vector<datum_t> data_t ;

    private:

        static data_t uninitialized_cb_default(addr_t, sz_t size) { return data_t(size); }

        std::function<data_t(addr_t, sz_t)> uninitialized_cb_ = uninitialized_cb_default;

    public:

        struct write_options {
            bool skip_zero;
        };
        static constexpr write_options write_options_default  {.skip_zero = false};
        static constexpr write_options write_options_skip_zero{.skip_zero = true};

        virtual bool read (addr_t addr, sz_t size,       datum_t* data                                                   )       =0;
        //TODO byte-enabled version
        virtual void write(addr_t addr, sz_t size, const datum_t* data, const write_options* opt = &write_options_default)       =0;
        virtual bool check(addr_t addr, sz_t size, const datum_t* data, bool allow_uninitialized = false                 ) const =0;

        // C++ versions
        data_t read (addr_t addr, sz_t size);
        void   write(addr_t addr, const data_t& data, const write_options& opt = write_options_default);
        bool   check(addr_t addr, const data_t& data, bool allow_unininitialized = false ) const;

        void   load_ELF(const std::string& filename);
        void   load_verilog_hex(const std::string& filename);
        void   load_lz4(const std::string& filename, addr_t offset = 0);

        void   uninitialized_read_data_cb(decltype(uninitialized_cb_) cb) { uninitialized_cb_ = cb; }

        virtual ~mem() {}

    protected:

        data_t uninitialized_read(addr_t addr, sz_t size) const { return uninitialized_cb_(addr, size); }

};
