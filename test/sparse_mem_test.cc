#include <gtest/gtest.h>
#include <cinttypes>
#include <numeric>
#include "mem_manager.h"
#include "tools/cpp/runfiles/runfiles.h"
#include "Vmem_manager_test.h"

TEST(Mem, WritingReading) {

    mem_manager mm;
    std::unordered_map<std::uint64_t, std::vector<std::uint8_t>> written;

    for (int i = 0; i < 10; i++) {
        std::uint64_t addr = std::uint64_t(1) << i;
        std::vector<std::uint8_t> data(i);
        std::iota(data.begin(), data.end(), addr);
        mm.write(addr, data);
        written[addr] = data;
    }

    for (const auto& it : written) {
        EXPECT_EQ(it.second, mm.read(it.first, it.second.size()));
    }

}

TEST(Mem, ReadingUninitialized) {

    mem_manager mm;
    mm.uninitialized_read_data_cb(
        [](mem_manager::addr_t addr, mem_manager::sz_t size) {
            mem_manager::data_t data(size);
            struct gen {
                mem_manager::addr_t addr;
                gen(addr_t addr) : addr(addr) {};
                datum_t operator()() {
                    datum_t d[] = {0xde, 0xad, 0xbe, 0xef};
                    return d[addr++ % (sizeof(d)/sizeof(d[0]))];
                }
            };
            std::generate(data.begin(), data.end(), gen(addr));
            return data;
        }
    );
    mm.write(0x3, mem_manager::data_t{{0xca, 0xfe}});

    mem_manager::data_t expected{{0xfe, 0xad, 0xbe}};
    EXPECT_EQ(expected, mm.read(0x4, 3));

}

TEST(Mem, SVBindings) {

    Vmem_manager_test top;
    int argc = 1;
    const char* argv[] = {"test"};
    Verilated::commandArgs(argc, argv);

    while (!Verilated::gotFinish()) {
        top.eval();
    }

}

TEST(Mem, CBindings) {

    mem_manager* mm = mem_manager_create();

    std::unordered_map<std::uint64_t, std::vector<std::uint8_t>> written;

    for (int i = 0; i < 10; i++) {
        std::uint64_t addr = std::uint64_t(1) << i;
        std::vector<std::uint8_t> data(i);
        std::iota(data.begin(), data.end(), addr);
        mem_manager_write(mm, addr, data.size(), &data[0]);
        written[addr] = data;
    }

    for (const auto& it : written) {
        std::uint8_t* data = new std::uint8_t[it.second.size()];
        mem_manager_read(mm, it.first, it.second.size(), data);

        for (size_t s = 0; s < it.second.size(); s++) {
            EXPECT_EQ(it.second[s], data[s]);
        }

        delete data;
    }

    mem_manager_destroy(mm);

}

static std::string get_runfile(const char* s) {

    using bazel::tools::cpp::runfiles::Runfiles;
    std::string error;
    std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest(&error));

    EXPECT_TRUE(runfiles);

    std::string file = runfiles->Rlocation(s);

    EXPECT_NE(file, "");

    return file;
}

static void check_mem(mem_manager& mm) {
    mem_manager::data_t data{{
0x03, 0x2d, 0x81, 0x06, 0x83, 0x2d, 0xc1, 0x06, 0x03, 0x2e, 0x01, 0x07, 0x83, 0x2e, 0x41, 0x07, 0x03, 0x2f, 0x81, 0x07, 0x83, 0x2f, 0xc1, 0x07, 0x13, 0x01, 0xc1, 0x07, 0x73, 0x00, 0x20, 0x30, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }};
    EXPECT_TRUE(mm.check(0x80000000 + 0x13c0, data));
    EXPECT_EQ(mm.read(0x80000000 + 0x13c0, data.size()), data);
    EXPECT_FALSE(mm.check(0x80000000 + 0x147c, {0}));
    EXPECT_TRUE(mm.check(0x80000000 + 0x147c -1, {0}));
    EXPECT_EQ(mm.read(0x80000000 + 0x147c, 1), mem_manager::data_t{0});
    EXPECT_TRUE(mm.check(0x80000000 + 0x147c, {0}));
}

TEST(Mem, ElfLoading) {
    mem_manager mm;
    mm.load_ELF(get_runfile("__main__/test/arith.riscv"));
    check_mem(mm);
}

TEST(Mem, VerilogHexLoading) {
    mem_manager mm;
    mm.load_verilog_hex(get_runfile("__main__/test/arith.hex"));
    check_mem(mm);
}
