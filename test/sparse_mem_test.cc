#include <gtest/gtest.h>
#include <cinttypes>
#include <numeric>
#include "mem_manager.h"

// Demonstrate some basic assertions.
TEST(SparseMem, BasicAssertions) {

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
