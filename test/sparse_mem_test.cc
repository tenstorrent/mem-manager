#include <gtest/gtest.h>
#include <cinttypes>
#include <numeric>
#include "mem_manager.h"
#include "tools/cpp/runfiles/runfiles.h"

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

TEST(Mem, ElfLoading) {

    using bazel::tools::cpp::runfiles::Runfiles;
    std::string error;
    std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest(&error));

    EXPECT_TRUE(runfiles);

    std::string elf = runfiles->Rlocation("__main__/test/arith.riscv");

    EXPECT_NE(elf, "");

    mem_manager mm;
    mm.load_ELF(elf);

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
