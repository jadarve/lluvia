
#include <algorithm>
#include <vector>
#include <tuple>

#include <gtest/gtest.h>

#include "lluvia/core/impl/MemoryFreeSpaceManager.h"


void checkMemory(const ll::impl::MemoryFreeSpaceManager& manager,
    const std::vector<uint64_t>& offsetVectorExpected,
    const std::vector<uint64_t>& sizeVectorExpected) {

    auto offsetVector = manager.getOffsetVector();
    auto sizeVector = manager.getSizeVector();

    auto offsetVectorEqual = std::equal(offsetVectorExpected.begin(), offsetVectorExpected.end(), offsetVector.begin());
    auto sizeVectorEqual = std::equal(sizeVectorExpected.begin(), sizeVectorExpected.end(), sizeVector.begin());

    ASSERT_EQ(true, offsetVectorEqual);
    ASSERT_EQ(true, sizeVectorEqual);
}


void checkAllocation(const std::tuple<bool, uint64_t>& expected, const std::tuple<bool, uint64_t>& returned) {

    ASSERT_EQ(true, std::get<0>(expected) == std::get<0>(returned));
    ASSERT_EQ(true, std::get<1>(expected) == std::get<1>(returned));
}


TEST(MemoryHeapTest, NoInsertions) {

    auto size = uint64_t{1024};
    auto offsetVector = std::vector<uint64_t>{0};
    auto sizeVector = std::vector<uint64_t>{size};

    auto manager = ll::impl::MemoryFreeSpaceManager{size};

    checkMemory(manager, offsetVector, sizeVector);
}


/**
 * Allocate
 */
TEST(MemoryHeapTest, A) {

    auto size = uint64_t{1024};
    auto offsetVector = std::vector<uint64_t>{512};
    auto sizeVector = std::vector<uint64_t>{512};

    auto manager = ll::impl::MemoryFreeSpaceManager{size};
    manager.allocate(512);

    checkMemory(manager, offsetVector, sizeVector);
}


/**
 * Allocate + Allocate + Allocate
 */
TEST(MemoryHeapTest, AAA) {

    auto size = uint64_t{1024};
    auto sizeA = uint64_t{256};
    auto sizeB = uint64_t{512};
    auto sizeC = uint64_t{128};

    auto manager = ll::impl::MemoryFreeSpaceManager{size};

    checkAllocation(manager.allocate(sizeA), std::make_tuple(true, 0));
    checkAllocation(manager.allocate(sizeB), std::make_tuple(true, sizeA));
    checkAllocation(manager.allocate(sizeC), std::make_tuple(true, sizeA + sizeB));

    auto offsetVector = std::vector<uint64_t>{sizeA + sizeB + sizeC};
    auto sizeVector = std::vector<uint64_t>{size - (sizeA + sizeB + sizeC)};
    checkMemory(manager, offsetVector, sizeVector);
}


/**
 * Allocate + Allocate + Release + Allocate
 */
TEST(MemoryHeapTest, AARA) {

    auto size = uint64_t{1024};
    auto sizeA = uint64_t{256};
    auto sizeB = uint64_t{512};
    auto sizeC = uint64_t{128};

    auto manager = ll::impl::MemoryFreeSpaceManager{size};

    checkAllocation(manager.allocate(sizeA), std::make_tuple(true, 0));         // alloc A
    checkAllocation(manager.allocate(sizeB), std::make_tuple(true, sizeA));     // alloc B
    manager.release(0, sizeA);                                                  // release A
    checkAllocation(manager.allocate(sizeC), std::make_tuple(true, 0));         // alloc C

    auto offsetVector = std::vector<uint64_t>{sizeC, sizeA + sizeB};
    auto sizeVector = std::vector<uint64_t>{sizeA - sizeC, size - (sizeA + sizeB)};
    checkMemory(manager, offsetVector, sizeVector);
}


int main(int argc, char **argv) {
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
