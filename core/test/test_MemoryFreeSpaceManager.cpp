
#include <algorithm>
#include <vector>
#include <tuple>

#include <gtest/gtest.h>

#include "lluvia/core/impl/MemoryFreeSpaceManager.h"


using namespace ll;
using namespace ll::impl;


void checkMemory(const MemoryFreeSpaceManager& manager,
    const std::vector<uint64_t>& offsetVectorExpected,
    const std::vector<uint64_t>& sizeVectorExpected) {

    auto offsetVector = manager.getOffsetVector();
    auto sizeVector = manager.getSizeVector();

    auto offsetVectorEqual = std::equal(offsetVectorExpected.begin(), offsetVectorExpected.end(), offsetVector.begin());
    auto sizeVectorEqual = std::equal(sizeVectorExpected.begin(), sizeVectorExpected.end(), sizeVector.begin());

    ASSERT_EQ(true, offsetVectorEqual);
    ASSERT_EQ(true, sizeVectorEqual);
}


void checkAllocation(bool boolExpected, bool boolReturned, const MemoryAllocationInfo& allocExpected,
    const MemoryAllocationInfo& allocReturned) {

    ASSERT_EQ(true, boolExpected == boolReturned);
    ASSERT_EQ(true, allocExpected.offset == allocReturned.offset);
    ASSERT_EQ(true, allocExpected.size == allocReturned.size);
}


TEST(MemoryHeapTest, NoInsertions) {

    auto size = uint64_t{1024};
    auto offsetVector = std::vector<uint64_t>{0};
    auto sizeVector = std::vector<uint64_t>{size};

    auto manager = MemoryFreeSpaceManager{size};

    checkMemory(manager, offsetVector, sizeVector);
}


/**
 * Allocate
 */
TEST(MemoryHeapTest, A) {

    auto size = uint64_t{1024};
    auto sizeA = uint64_t{256};

    auto offsetVector = std::vector<uint64_t>{sizeA};
    auto sizeVector = std::vector<uint64_t>{size - sizeA};

    auto manager = MemoryFreeSpaceManager{size};

    auto allocA = MemoryAllocationInfo{};
    auto boolA = manager.allocate(sizeA, allocA);
    
    checkAllocation(true, boolA, allocA, MemoryAllocationInfo{0, sizeA});
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

    auto manager = MemoryFreeSpaceManager{size};

    auto allocA = MemoryAllocationInfo{};
    auto allocB = MemoryAllocationInfo{};
    auto allocC = MemoryAllocationInfo{};

    auto boolA = manager.allocate(sizeA, allocA);
    auto boolB = manager.allocate(sizeB, allocB);
    auto boolC = manager.allocate(sizeC, allocC);

    checkAllocation(true, boolA, allocA, MemoryAllocationInfo{0, sizeA});
    checkAllocation(true, boolB, allocB, MemoryAllocationInfo{sizeA, sizeB});
    checkAllocation(true, boolC, allocC, MemoryAllocationInfo{sizeA + sizeB, sizeC});

    auto offsetVector = std::vector<uint64_t>{sizeA + sizeB + sizeC};
    auto sizeVector = std::vector<uint64_t>{size - (sizeA + sizeB + sizeC)};
    checkMemory(manager, offsetVector, sizeVector);
}


/**
 * Allocate + Allocate + Allocate + Release + Release + Release
 *
 * Release first allocated objects first.
 */
TEST(MemoryHeapTest, AAARRR_fifo) {

    auto size = uint64_t{1024};
    auto sizeA = uint64_t{256};
    auto sizeB = uint64_t{512};
    auto sizeC = uint64_t{128};

    auto manager = MemoryFreeSpaceManager{size};

    auto allocA = MemoryAllocationInfo{};
    auto allocB = MemoryAllocationInfo{};
    auto allocC = MemoryAllocationInfo{};

    auto boolA = manager.allocate(sizeA, allocA);
    auto boolB = manager.allocate(sizeB, allocB);
    auto boolC = manager.allocate(sizeC, allocC);

    checkAllocation(true, boolA, allocA, MemoryAllocationInfo{0, sizeA});
    checkAllocation(true, boolB, allocB, MemoryAllocationInfo{sizeA, sizeB});
    checkAllocation(true, boolC, allocC, MemoryAllocationInfo{sizeA + sizeB, sizeC});

    manager.release(allocA);
    manager.release(allocB);
    manager.release(allocC);

    auto offsetVector = std::vector<uint64_t>{0};
    auto sizeVector = std::vector<uint64_t>{size};
    checkMemory(manager, offsetVector, sizeVector);
}


/**
 * Allocate + Allocate + Allocate + Release + Release + Release
 *
 * Release last allocated objects first.
 */
TEST(MemoryHeapTest, AAARRR_lifo) {

    auto size = uint64_t{1024};
    auto sizeA = uint64_t{256};
    auto sizeB = uint64_t{512};
    auto sizeC = uint64_t{128};

    auto manager = MemoryFreeSpaceManager{size};

    auto allocA = MemoryAllocationInfo{};
    auto allocB = MemoryAllocationInfo{};
    auto allocC = MemoryAllocationInfo{};

    auto boolA = manager.allocate(sizeA, allocA);
    auto boolB = manager.allocate(sizeB, allocB);
    auto boolC = manager.allocate(sizeC, allocC);

    checkAllocation(true, boolA, allocA, MemoryAllocationInfo{0, sizeA});
    checkAllocation(true, boolB, allocB, MemoryAllocationInfo{sizeA, sizeB});
    checkAllocation(true, boolC, allocC, MemoryAllocationInfo{sizeA + sizeB, sizeC});

    manager.release(allocC);
    manager.release(allocB);
    manager.release(allocA);

    auto offsetVector = std::vector<uint64_t>{0};
    auto sizeVector = std::vector<uint64_t>{size};
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

    auto manager = MemoryFreeSpaceManager{size};

    auto allocA = MemoryAllocationInfo{};
    auto allocB = MemoryAllocationInfo{};
    auto allocC = MemoryAllocationInfo{};

    auto boolA = manager.allocate(sizeA, allocA);
    checkAllocation(true, boolA, MemoryAllocationInfo{0, sizeA}, allocA);

    auto boolB = manager.allocate(sizeB, allocB);
    checkAllocation(true, boolB, MemoryAllocationInfo{sizeA, sizeB}, allocB);

    manager.release(allocA);

    auto boolC = manager.allocate(sizeC, allocC);
    checkAllocation(true, boolC, MemoryAllocationInfo{0, sizeC}, allocC);


    auto offsetVector = std::vector<uint64_t>{sizeC, sizeA + sizeB};
    auto sizeVector = std::vector<uint64_t>{sizeA - sizeC, size - (sizeA + sizeB)};
    checkMemory(manager, offsetVector, sizeVector);
}


/**
 * Allocate + Allocate + Release + Allocate + Release
 *
 * Test lower bound merge
 */
TEST(MemoryHeapTest, AARAR) {

    auto size = uint64_t{1024};
    auto sizeA = uint64_t{256};
    auto sizeB = uint64_t{512};
    auto sizeC = uint64_t{128};

    auto manager = MemoryFreeSpaceManager{size};

    auto allocA = MemoryAllocationInfo{};
    auto allocB = MemoryAllocationInfo{};
    auto allocC = MemoryAllocationInfo{};

    auto boolA = manager.allocate(sizeA, allocA);
    checkAllocation(true, boolA, MemoryAllocationInfo{0, sizeA}, allocA);

    auto boolB = manager.allocate(sizeB, allocB);
    checkAllocation(true, boolB, MemoryAllocationInfo{sizeA, sizeB}, allocB);

    manager.release(allocA);

    auto boolC = manager.allocate(sizeC, allocC);
    checkAllocation(true, boolC, MemoryAllocationInfo{0, sizeC}, allocC);

    manager.release(allocB);

    auto offsetVector = std::vector<uint64_t>{sizeC};
    auto sizeVector = std::vector<uint64_t>{size - sizeC};
    checkMemory(manager, offsetVector, sizeVector);
}


int main(int argc, char **argv) {
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
