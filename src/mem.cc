#include "mem.h"
#include "util.h"
#include <cstdlib>
#include <cstdio>
#include <stdexcept>
#include <functional>
#include <fstream>
#include <limits>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <lz4frame.h>
#include <memory>

namespace {

    class fd_t {

        private:
            int fd;

        public:

            template <typename... Args>
                fd_t(const char* filename, Args&&... args) : fd(open(filename, std::forward<Args>(args)...)){
                    if (fd == -1) {
                        throw std::runtime_error("Couldn't open " + std::string(filename));
                    }
                }

            fd_t(fd_t&) = delete;
            fd_t& operator=(fd_t&) = delete;

            ~fd_t() {
                close(fd);
            }

            operator int() { return fd; }
    };

    class mmapped {

        private:

            void* p;
            size_t s;

        public:

            mmapped() : p(nullptr), s(0) {}

            template <typename... Args>
                mmapped(void* start, size_t size, Args&&... args) :
                    p(mmap(start, size, std::forward<Args>(args)...)),
                    s(size) {
                        if(p == MAP_FAILED) {
                            throw std::runtime_error("Couldn't mmap");
                        }
                    }

            mmapped& operator=(mmapped&& other) {
                std::swap(p, other.p);
                std::swap(s, other.s);
                return *this;
            }

            mmapped(mmapped&& other) {
                std::swap(p, other.p);
                std::swap(s, other.s);
            }

            mmapped& operator=(mmapped& other) = delete;
            mmapped (mmapped& other) = delete;

            ~mmapped() { if(p) munmap(p, s); }

            template <typename T>
                explicit operator T() { return static_cast<T>(p); }

    };

    class mmapped_file {

        private:

            mmapped mmapped_;
            size_t length_;

        public:

            mmapped_file(const std::string& filename) {

                fd_t fd(filename.c_str(), O_RDONLY);

                struct stat sbuf;
                if (fstat(fd, &sbuf) == -1) {
                    throw std::runtime_error("Couldn't fstat " + filename);
                }
                length_ = sbuf.st_size;

                mmapped_ = ::mmapped(0, length_, PROT_READ, MAP_SHARED, fd, 0);
            }

            mmapped& mmapped() {
                return mmapped_;
            }

            size_t length() {
                return length_;
            }
    };

}

void mem::load_ELF(const std::string& filename) {

    struct tmpfile {
        std::string tmp;
        tmpfile() {
            char mkstemp_template[] = "/tmp/tmpfileXXXXXX";
            int fd = mkstemp(mkstemp_template);
            if (fd == -1) {
                throw std::runtime_error("Failed to create temporary file.");
            }
            tmp = mkstemp_template;
            close(fd);
        }
        ~tmpfile() {
            std::remove(tmp.c_str());
        }
    } tmp;

    std::string cmd = std::string("objcopy -O verilog ") + filename + " " + tmp.tmp;
    util::exec_check(cmd.c_str());

    load_verilog_hex(tmp.tmp);
}

void mem::load_verilog_hex(const std::string& filename) {
    bool   addr_found = false;
    addr_t addr;
    data_t data;

    std::ifstream file(filename);

    std::string s;
    while (file >> s) {
        if (s[0] == '@') {
            if (addr_found) {
                write(addr, data);
            }
            data.clear();

            addr = std::strtoull(s.c_str() + 1, nullptr, 16);
            addr_found = true;
        } else {
            if (!addr_found) {
                throw std::runtime_error("Couldn't parse " + filename);
            }

            unsigned long long d = std::strtoull(s.c_str(), nullptr, 16);
            if (d > std::numeric_limits<datum_t>::max()) {
                throw std::runtime_error("Invalid value " + s);
            }

            data.push_back(d);
        }
    }

    if (addr_found) {
        write(addr, data);
    }
}

void mem::load_lz4(const std::string& filename, addr_t offset) {
    const size_t BLOCK_SIZE = 4*1024*1024;

    ::mmapped_file mmapped_file(filename);
    size_t src_size = mmapped_file.length();
    uint8_t* src = static_cast<uint8_t*>(mmapped_file.mmapped());

    const size_t dst_size = BLOCK_SIZE;
    auto dst_array = std::make_unique<uint8_t[]>(dst_size);
    uint8_t* dst = dst_array.get();

    LZ4F_dctx *dctx;
    LZ4F_errorCode_t ret = LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION);
    if (LZ4F_isError(ret)) {
        throw std::runtime_error("Couldn't initialize LZ4 context");
    }

    ssize_t addr = 0;
    while (src_size) {
        size_t src_bytes_read = src_size;
        size_t dst_bytes_written = dst_size;

        size_t ret = LZ4F_decompress(dctx, dst, &dst_bytes_written, src, &src_bytes_read, NULL);
        if (LZ4F_isError(ret)) {
            throw std::runtime_error("LZ4F_decompress failed");
        }
        write(addr+offset, dst_bytes_written, dst, &write_options_skip_zero);
        src = src + src_bytes_read;
        src_size = src_size - src_bytes_read;
        addr = addr + dst_bytes_written;
    }
}

mem::data_t mem::read(addr_t addr, sz_t size) {

    data_t data(size);
    read(addr, size, data.data());
    return data;

}

void mem::write(addr_t addr, const data_t& data, const write_options& options) {

    write(addr, data.size(), data.data(), &options);

}

bool mem::check(addr_t addr, const data_t& data, bool allow_unininitialized) const {

    return check(addr, data.size(), data.data(), allow_unininitialized);

}
