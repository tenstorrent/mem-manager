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

void mem::load_ELF(const std::string& filename) {

    struct tmpfile {
        std::string tmp = std::tmpnam(nullptr);
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

void *mem::mmap_file(const std::string& filename, size_t& length) {
    int fd;
    struct stat sbuf;
    void *src;

    fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error("Couldn't open " + filename);
    }

    if (fstat(fd, &sbuf) == -1) {
        throw std::runtime_error("Couldn't fstat " + filename);
    }
    length = sbuf.st_size;

    src = mmap(0, length, PROT_READ, MAP_SHARED, fd, 0);
    if (src == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("Couldn't mmap " + filename);
    }

    close(fd);

    return src;
}

void mem::process(uint8_t *data, ssize_t addr, size_t length) {
    while (length--) {
        if (*data) {
            write(addr, 1, data);
        }
        addr++;
        data++;
    }
}

#define BLOCK_SIZE (4*1024*1024)

void mem::load_lz4(const std::string& filename) {
    LZ4F_dctx *dctx;
    LZ4F_errorCode_t ret;
    uint8_t *src;
    uint8_t *dst;
    size_t src_size;
    size_t dst_size;
    ssize_t addr = 0;

    src = (uint8_t *)mmap_file(filename, src_size);

    dst_size = BLOCK_SIZE;
    dst = (uint8_t *)malloc(dst_size);

    ret = LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION);
    if (LZ4F_isError(ret)) {
        throw std::runtime_error("Couldn't initialize LZ4 context");
    }

    while (src_size) {
        size_t src_bytes_read = src_size;
        size_t dst_bytes_written = dst_size;

        size_t ret = LZ4F_decompress(dctx, dst, &dst_bytes_written, src, &src_bytes_read, NULL);
        if (LZ4F_isError(ret)) {
            throw std::runtime_error("LZ4F_decompress failed");
        }
        process(dst, addr, dst_bytes_written);
        src = src + src_bytes_read;
        src_size = src_size - src_bytes_read;
        addr = addr + dst_bytes_written;
    }

    munmap(src, src_size);
    free(dst);
}

mem::data_t mem::read(addr_t addr, sz_t size) {

    data_t data(size);
    read(addr, size, data.data());
    return data;

}

void mem::write(addr_t addr, const data_t& data) {

    write(addr, data.size(), data.data());

}

bool mem::check(addr_t addr, const data_t& data, bool allow_unininitialized) const {

    return check(addr, data.size(), data.data(), allow_unininitialized);

}
