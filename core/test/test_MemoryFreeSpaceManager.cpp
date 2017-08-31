
#include <vector>
#include <algorithm>

#include <gtest/gtest.h>

#include "lluvia/core/impl/MemoryFreeSpaceManager.h"


void compareResults(const ll::impl::MemoryFreeSpaceManager& manager,
    const std::vector<uint64_t>& offsetVectorExpected,
    const std::vector<uint64_t>& sizeVectorExpected) {

    auto offsetVector = manager.getOffsetVector();
    auto sizeVector = manager.getSizeVector();

    auto offsetVectorEqual = std::equal(offsetVectorExpected.begin(), offsetVectorExpected.end(), offsetVector.begin());
    auto sizeVectorEqual = std::equal(sizeVectorExpected.begin(), sizeVectorExpected.end(), sizeVector.begin());

    ASSERT_EQ(true, offsetVectorEqual);
    ASSERT_EQ(true, sizeVectorEqual);
}

TEST(MemoryHeapTest, NoInsertions) {

    auto size = uint64_t{1024};
    auto offsetVector = std::vector<uint64_t>{0};
    auto sizeVector = std::vector<uint64_t>{size};

    auto manager = ll::impl::MemoryFreeSpaceManager{size};

    compareResults(manager, offsetVector, sizeVector);
}


TEST(MemoryHeapTest, OneInsertion) {

    auto size = uint64_t{1024};
    auto offsetVector = std::vector<uint64_t>{512};
    auto sizeVector = std::vector<uint64_t>{512};

    auto manager = ll::impl::MemoryFreeSpaceManager{size};
    manager.allocate(512);

    compareResults(manager, offsetVector, sizeVector);
}

int main(int argc, char **argv) {
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}