#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <algorithm>
#include <tuple>
#include <vector>

#include "lluvia/core/memory/MemoryFreeSpaceManager.h"

using namespace ll;
using namespace ll::impl;

void checkMemory(const MemoryFreeSpaceManager& manager,
    const std::vector<uint64_t>&               offsetVectorExpected,
    const std::vector<uint64_t>&               sizeVectorExpected)
{

    auto offsetVector = manager.getOffsetVector();
    auto sizeVector   = manager.getSizeVector();

    auto offsetVectorEqual = std::equal(offsetVectorExpected.begin(), offsetVectorExpected.end(), offsetVector.begin());
    auto sizeVectorEqual   = std::equal(sizeVectorExpected.begin(), sizeVectorExpected.end(), sizeVector.begin());

    REQUIRE(offsetVectorEqual);
    REQUIRE(sizeVectorEqual);
}

void checkAllocation(bool boolExpected, bool boolReturned, const MemoryAllocationInfo& allocExpected,
    const MemoryAllocationInfo& allocReturned)
{

    REQUIRE(boolExpected == boolReturned);
    REQUIRE(allocExpected.offset == allocReturned.offset);
    REQUIRE(allocExpected.size == allocReturned.size);
    REQUIRE(allocExpected.leftPadding == allocReturned.leftPadding);
}

/**
 * No insertions
 */
TEST_CASE("NoInsertions", "test_MemoryFreeSpaceManager")
{

    auto size         = uint64_t {1024};
    auto offsetVector = std::vector<uint64_t> {0};
    auto sizeVector   = std::vector<uint64_t> {size};

    auto manager = MemoryFreeSpaceManager {size};

    checkMemory(manager, offsetVector, sizeVector);
}

/**
 * Allocate
 */
TEST_CASE("A", "test_MemoryFreeSpaceManager")
{

    auto size  = uint64_t {1024};
    auto sizeA = uint64_t {256};

    auto offsetVector = std::vector<uint64_t> {sizeA};
    auto sizeVector   = std::vector<uint64_t> {size - sizeA};

    auto manager = MemoryFreeSpaceManager {size};

    auto allocA = MemoryAllocationInfo {};
    auto boolA  = manager.allocate(sizeA, allocA);

    checkAllocation(true, boolA, allocA, MemoryAllocationInfo {0, sizeA, 0, 0});
    checkMemory(manager, offsetVector, sizeVector);
}

/**
 * Allocate + Release full size
 */
TEST_CASE("AR_fullSize", "test_MemoryFreeSpaceManager")
{

    auto size  = uint64_t {1024};
    auto sizeA = uint64_t {1024};

    auto offsetVector = std::vector<uint64_t> {sizeA};
    auto sizeVector   = std::vector<uint64_t> {size - sizeA};

    auto manager = MemoryFreeSpaceManager {size};

    auto allocA = MemoryAllocationInfo {};
    auto boolA  = manager.allocate(sizeA, allocA);

    checkAllocation(true, boolA, allocA, MemoryAllocationInfo {0, sizeA, 0, 0});
    checkMemory(manager, offsetVector, sizeVector);

    manager.release(allocA);

    offsetVector = std::vector<uint64_t> {0};
    sizeVector   = std::vector<uint64_t> {size};
    checkMemory(manager, offsetVector, sizeVector);
}

/**
 * Allocate + Allocate + Allocate
 */
TEST_CASE("AAA", "test_MemoryFreeSpaceManager")
{

    auto size  = uint64_t {1024};
    auto sizeA = uint64_t {256};
    auto sizeB = uint64_t {512};
    auto sizeC = uint64_t {128};

    auto manager = MemoryFreeSpaceManager {size};

    auto allocA = MemoryAllocationInfo {};
    auto allocB = MemoryAllocationInfo {};
    auto allocC = MemoryAllocationInfo {};

    auto boolA = manager.allocate(sizeA, allocA);
    auto boolB = manager.allocate(sizeB, allocB);
    auto boolC = manager.allocate(sizeC, allocC);

    checkAllocation(true, boolA, allocA, MemoryAllocationInfo {0, sizeA, 0, 0});
    checkAllocation(true, boolB, allocB, MemoryAllocationInfo {sizeA, sizeB, 0, 0});
    checkAllocation(true, boolC, allocC, MemoryAllocationInfo {sizeA + sizeB, sizeC, 0, 0});

    auto offsetVector = std::vector<uint64_t> {sizeA + sizeB + sizeC};
    auto sizeVector   = std::vector<uint64_t> {size - (sizeA + sizeB + sizeC)};
    checkMemory(manager, offsetVector, sizeVector);
}

/**
 * Allocate + Allocate + Allocate + Release + Release + Release
 *
 * Release first allocated objects first.
 */
TEST_CASE("AAARRR_fifo", "test_MemoryFreeSpaceManager")
{

    auto size  = uint64_t {1024};
    auto sizeA = uint64_t {256};
    auto sizeB = uint64_t {512};
    auto sizeC = uint64_t {128};

    auto manager = MemoryFreeSpaceManager {size};

    auto allocA = MemoryAllocationInfo {};
    auto allocB = MemoryAllocationInfo {};
    auto allocC = MemoryAllocationInfo {};

    auto boolA = manager.allocate(sizeA, allocA);
    auto boolB = manager.allocate(sizeB, allocB);
    auto boolC = manager.allocate(sizeC, allocC);

    checkAllocation(true, boolA, allocA, MemoryAllocationInfo {0, sizeA, 0, 0});
    checkAllocation(true, boolB, allocB, MemoryAllocationInfo {sizeA, sizeB, 0, 0});
    checkAllocation(true, boolC, allocC, MemoryAllocationInfo {sizeA + sizeB, sizeC, 0, 0});

    manager.release(allocA);
    manager.release(allocB);
    manager.release(allocC);

    auto offsetVector = std::vector<uint64_t> {0};
    auto sizeVector   = std::vector<uint64_t> {size};
    checkMemory(manager, offsetVector, sizeVector);
}

/**
 * Allocate + Allocate + Allocate + Release + Release + Release
 *
 * Release last allocated objects first.
 */
TEST_CASE("AAARRR_lifo", "test_MemoryFreeSpaceManager")
{

    auto size  = uint64_t {1024};
    auto sizeA = uint64_t {256};
    auto sizeB = uint64_t {512};
    auto sizeC = uint64_t {128};

    auto manager = MemoryFreeSpaceManager {size};

    auto allocA = MemoryAllocationInfo {};
    auto allocB = MemoryAllocationInfo {};
    auto allocC = MemoryAllocationInfo {};

    auto boolA = manager.allocate(sizeA, allocA);
    auto boolB = manager.allocate(sizeB, allocB);
    auto boolC = manager.allocate(sizeC, allocC);

    checkAllocation(true, boolA, allocA, MemoryAllocationInfo {0, sizeA, 0, 0});
    checkAllocation(true, boolB, allocB, MemoryAllocationInfo {sizeA, sizeB, 0, 0});
    checkAllocation(true, boolC, allocC, MemoryAllocationInfo {sizeA + sizeB, sizeC, 0, 0});

    manager.release(allocC);
    manager.release(allocB);
    manager.release(allocA);

    auto offsetVector = std::vector<uint64_t> {0};
    auto sizeVector   = std::vector<uint64_t> {size};
    checkMemory(manager, offsetVector, sizeVector);
}

/**
 * Allocate + Allocate + Allocate + Release + Release + Release
 *
 * Release in order 1, 3, 2 to check simultaneous lower and upper merge
 */
TEST_CASE("AAARRR_simultaneousMerge", "test_MemoryFreeSpaceManager")
{

    auto size  = uint64_t {1024};
    auto sizeA = uint64_t {256};
    auto sizeB = uint64_t {512};
    auto sizeC = uint64_t {128};

    auto manager = MemoryFreeSpaceManager {size};

    auto allocA = MemoryAllocationInfo {};
    auto allocB = MemoryAllocationInfo {};
    auto allocC = MemoryAllocationInfo {};

    auto boolA = manager.allocate(sizeA, allocA);
    auto boolB = manager.allocate(sizeB, allocB);
    auto boolC = manager.allocate(sizeC, allocC);

    checkAllocation(true, boolA, allocA, MemoryAllocationInfo {0, sizeA, 0, 0});
    checkAllocation(true, boolB, allocB, MemoryAllocationInfo {sizeA, sizeB, 0, 0});
    checkAllocation(true, boolC, allocC, MemoryAllocationInfo {sizeA + sizeB, sizeC, 0, 0});

    manager.release(allocA);
    manager.release(allocC);
    manager.release(allocB);

    auto offsetVector = std::vector<uint64_t> {0};
    auto sizeVector   = std::vector<uint64_t> {size};
    checkMemory(manager, offsetVector, sizeVector);
}

/**
 * Allocate + Allocate + Release + Allocate
 */
TEST_CASE("AARA", "test_MemoryFreeSpaceManager")
{

    auto size  = uint64_t {1024};
    auto sizeA = uint64_t {256};
    auto sizeB = uint64_t {512};
    auto sizeC = uint64_t {128};

    auto manager = MemoryFreeSpaceManager {size};

    auto allocA = MemoryAllocationInfo {};
    auto allocB = MemoryAllocationInfo {};
    auto allocC = MemoryAllocationInfo {};

    auto boolA = manager.allocate(sizeA, allocA);
    checkAllocation(true, boolA, MemoryAllocationInfo {0, sizeA, 0, 0}, allocA);

    auto boolB = manager.allocate(sizeB, allocB);
    checkAllocation(true, boolB, MemoryAllocationInfo {sizeA, sizeB, 0, 0}, allocB);

    manager.release(allocA);

    auto boolC = manager.allocate(sizeC, allocC);
    checkAllocation(true, boolC, MemoryAllocationInfo {0, sizeC, 0, 0}, allocC);

    auto offsetVector = std::vector<uint64_t> {sizeC, sizeA + sizeB};
    auto sizeVector   = std::vector<uint64_t> {sizeA - sizeC, size - (sizeA + sizeB)};
    checkMemory(manager, offsetVector, sizeVector);
}

/**
 * Allocate + Allocate + Release + Allocate + Release
 *
 * Test lower bound merge
 */
TEST_CASE("AARAR", "test_MemoryFreeSpaceManager")
{

    auto size  = uint64_t {1024};
    auto sizeA = uint64_t {256};
    auto sizeB = uint64_t {512};
    auto sizeC = uint64_t {128};

    auto manager = MemoryFreeSpaceManager {size};

    auto allocA = MemoryAllocationInfo {};
    auto allocB = MemoryAllocationInfo {};
    auto allocC = MemoryAllocationInfo {};

    auto boolA = manager.allocate(sizeA, allocA);
    checkAllocation(true, boolA, MemoryAllocationInfo {0, sizeA, 0, 0}, allocA);

    auto boolB = manager.allocate(sizeB, allocB);
    checkAllocation(true, boolB, MemoryAllocationInfo {sizeA, sizeB, 0, 0}, allocB);

    manager.release(allocA);

    auto boolC = manager.allocate(sizeC, allocC);
    checkAllocation(true, boolC, MemoryAllocationInfo {0, sizeC, 0, 0}, allocC);

    manager.release(allocB);

    auto offsetVector = std::vector<uint64_t> {sizeC};
    auto sizeVector   = std::vector<uint64_t> {size - sizeC};
    checkMemory(manager, offsetVector, sizeVector);
}

TEST_CASE("offset_A", "test_MemoryFreeSpaceManager")
{

    auto size  = uint64_t {1024};
    auto sizeA = uint64_t {10};

    auto alignment = 0x08u;

    auto manager = MemoryFreeSpaceManager(size);

    auto allocA = MemoryAllocationInfo {};

    auto boolA = manager.allocate(sizeA, alignment, allocA);
    checkAllocation(true, boolA, MemoryAllocationInfo {0, sizeA, 0, 0}, allocA);

    auto offsetVector = std::vector<uint64_t> {sizeA};
    auto sizeVector   = std::vector<uint64_t> {size - sizeA};
    checkMemory(manager, offsetVector, sizeVector);
}

TEST_CASE("offset_AA", "test_MemoryFreeSpaceManager")
{

    auto size  = uint64_t {1024};
    auto sizeA = uint64_t {10};
    auto sizeB = uint64_t {20};

    auto alignment = 0x08u;
    auto offsetB   = 16u;

    auto manager = MemoryFreeSpaceManager(size);

    auto allocA = MemoryAllocationInfo {};
    auto allocB = MemoryAllocationInfo {};

    auto boolA = manager.allocate(sizeA, alignment, allocA);
    checkAllocation(true, boolA, MemoryAllocationInfo {0, sizeA, 0, 0}, allocA);

    auto boolB = manager.allocate(sizeB, alignment, allocB);
    checkAllocation(true, boolB, MemoryAllocationInfo {offsetB, sizeB, 6u, 0}, allocB);

    auto offsetVector = std::vector<uint64_t> {offsetB + sizeB};
    auto sizeVector   = std::vector<uint64_t> {size - (sizeA + 6 + sizeB)};
    checkMemory(manager, offsetVector, sizeVector);
}

TEST_CASE("offset_AAR", "test_MemoryFreeSpaceManager")
{

    auto size  = uint64_t {1024};
    auto sizeA = uint64_t {10};
    auto sizeB = uint64_t {20};

    auto alignment = 0x08u;
    auto offsetB   = 16u;

    auto manager = MemoryFreeSpaceManager(size);

    auto allocA = MemoryAllocationInfo {};
    auto allocB = MemoryAllocationInfo {};

    auto boolA = manager.allocate(sizeA, alignment, allocA);
    checkAllocation(true, boolA, MemoryAllocationInfo {0, sizeA, 0, 0}, allocA);

    auto boolB = manager.allocate(sizeB, alignment, allocB);
    checkAllocation(true, boolB, MemoryAllocationInfo {offsetB, sizeB, 6u, 0}, allocB);

    manager.release(allocB);

    auto offsetVector = std::vector<uint64_t> {sizeA};
    auto sizeVector   = std::vector<uint64_t> {size - sizeA};
    checkMemory(manager, offsetVector, sizeVector);
}

TEST_CASE("offset_AAA", "test_MemoryFreeSpaceManager")
{

    auto size  = uint64_t {1024};
    auto sizeA = uint64_t {10};
    auto sizeB = uint64_t {20};
    auto sizeC = uint64_t {30};

    auto alignment = 0x08u;
    auto offsetB   = 16u;
    auto offsetC   = 40u;

    auto paddingA = 0u;
    auto paddingB = 6u;
    auto paddingC = 4u;

    auto manager = MemoryFreeSpaceManager(size);

    auto allocA = MemoryAllocationInfo {};
    auto allocB = MemoryAllocationInfo {};
    auto allocC = MemoryAllocationInfo {};

    auto boolA = manager.allocate(sizeA, alignment, allocA);
    checkAllocation(true, boolA, MemoryAllocationInfo {0, sizeA, paddingA, 0}, allocA);

    auto boolB = manager.allocate(sizeB, alignment, allocB);
    checkAllocation(true, boolB, MemoryAllocationInfo {offsetB, sizeB, paddingB, 0}, allocB);

    auto boolC = manager.allocate(sizeC, alignment, allocC);
    checkAllocation(true, boolC, MemoryAllocationInfo {offsetC, sizeC, paddingC, 0}, allocC);

    auto offsetVector = std::vector<uint64_t> {offsetC + sizeC};
    auto sizeVector   = std::vector<uint64_t> {size - (sizeA + paddingB + sizeB + paddingC + sizeC)};
    checkMemory(manager, offsetVector, sizeVector);
}

/**
 * Allocate + Allocate + Allocate + Release + Release + Release
 *
 * Release first allocated objects first.
 */
TEST_CASE("offset_AAARRR_fifo", "test_MemoryFreeSpaceManager")
{

    auto size  = uint64_t {1024};
    auto sizeA = uint64_t {10};
    auto sizeB = uint64_t {20};
    auto sizeC = uint64_t {30};

    auto alignment = 0x08u;
    auto offsetB   = 16u;
    auto offsetC   = 40u;

    auto paddingA = 0u;
    auto paddingB = 6u;
    auto paddingC = 4u;

    auto manager = MemoryFreeSpaceManager(size);

    auto allocA = MemoryAllocationInfo {};
    auto allocB = MemoryAllocationInfo {};
    auto allocC = MemoryAllocationInfo {};

    auto boolA = manager.allocate(sizeA, alignment, allocA);
    checkAllocation(true, boolA, MemoryAllocationInfo {0, sizeA, paddingA, 0}, allocA);

    auto boolB = manager.allocate(sizeB, alignment, allocB);
    checkAllocation(true, boolB, MemoryAllocationInfo {offsetB, sizeB, paddingB, 0}, allocB);

    auto boolC = manager.allocate(sizeC, alignment, allocC);
    checkAllocation(true, boolC, MemoryAllocationInfo {offsetC, sizeC, paddingC, 0}, allocC);

    manager.release(allocA);
    manager.release(allocB);
    manager.release(allocC);

    auto offsetVector = std::vector<uint64_t> {0};
    auto sizeVector   = std::vector<uint64_t> {size};
    checkMemory(manager, offsetVector, sizeVector);
}

/**
 * Allocate + Allocate + Allocate + Release + Release + Release
 *
 * Release last allocated objects first.
 */
TEST_CASE("offset_AAARRR_lifo", "test_MemoryFreeSpaceManager")
{

    auto size  = uint64_t {1024};
    auto sizeA = uint64_t {10};
    auto sizeB = uint64_t {20};
    auto sizeC = uint64_t {30};

    auto alignment = 0x08u;
    auto offsetB   = 16u;
    auto offsetC   = 40u;

    auto paddingA = 0u;
    auto paddingB = 6u;
    auto paddingC = 4u;

    auto manager = MemoryFreeSpaceManager(size);

    auto allocA = MemoryAllocationInfo {};
    auto allocB = MemoryAllocationInfo {};
    auto allocC = MemoryAllocationInfo {};

    auto boolA = manager.allocate(sizeA, alignment, allocA);
    checkAllocation(true, boolA, MemoryAllocationInfo {0, sizeA, paddingA, 0}, allocA);

    auto boolB = manager.allocate(sizeB, alignment, allocB);
    checkAllocation(true, boolB, MemoryAllocationInfo {offsetB, sizeB, paddingB, 0}, allocB);

    auto boolC = manager.allocate(sizeC, alignment, allocC);
    checkAllocation(true, boolC, MemoryAllocationInfo {offsetC, sizeC, paddingC, 0}, allocC);

    manager.release(allocC);
    manager.release(allocB);
    manager.release(allocA);

    auto offsetVector = std::vector<uint64_t> {0};
    auto sizeVector   = std::vector<uint64_t> {size};
    checkMemory(manager, offsetVector, sizeVector);
}

/**
 * Allocate + Allocate + Release + Allocate + Release
 *
 * Test lower bound merge
 */
TEST_CASE("offset_AARAR", "test_MemoryFreeSpaceManager")
{

    auto size  = uint64_t {1024};
    auto sizeA = uint64_t {30};
    auto sizeB = uint64_t {20};
    auto sizeC = uint64_t {10};

    auto paddingA = 0u;
    auto paddingB = 2u;
    auto paddingC = 0u;

    auto alignment = 0x08u;
    auto offsetB   = sizeA + paddingB;
    auto offsetC   = 0u;

    auto manager = MemoryFreeSpaceManager {size};

    auto allocA = MemoryAllocationInfo {};
    auto allocB = MemoryAllocationInfo {};
    auto allocC = MemoryAllocationInfo {};

    auto boolA = manager.allocate(sizeA, alignment, allocA);
    checkAllocation(true, boolA, MemoryAllocationInfo {0, sizeA, paddingA, 0}, allocA);

    auto boolB = manager.allocate(sizeB, alignment, allocB);
    checkAllocation(true, boolB, MemoryAllocationInfo {offsetB, sizeB, paddingB, 0}, allocB);

    manager.release(allocA);

    auto boolC = manager.allocate(sizeC, alignment, allocC);
    checkAllocation(true, boolC, MemoryAllocationInfo {offsetC, sizeC, paddingC, 0}, allocC);

    manager.release(allocB);

    auto offsetVector = std::vector<uint64_t> {sizeC};
    auto sizeVector   = std::vector<uint64_t> {size - sizeC};
    checkMemory(manager, offsetVector, sizeVector);
}
