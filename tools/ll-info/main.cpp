
#include "lluvia/core.h"

#include <cstdlib>
#include <iostream>
#include <sstream>      // stringbuf
#include <string>


using namespace std;

std::string getPhysicalDevicePropertiesString(const vk::PhysicalDeviceMemoryProperties& memProperties) {

    auto compareFlagBit = [](const auto& flag, const auto& value) {
        return ((flag & value) == value);
    };

    auto buffer = std::ostringstream {};

    buffer << "************************************************************\n";
    buffer << "           PHYSICAL DEVICE MEMORY PROPERTIES\n";
    buffer << "************************************************************\n";
    buffer << "Heap count:\t\t" << memProperties.memoryHeapCount << "\n";
    buffer << "Memory type count:\t" << memProperties.memoryTypeCount << "\n\n";


    for(int i = 0; i < memProperties.memoryHeapCount; i ++) {
        vk::MemoryHeap heap = memProperties.memoryHeaps[i];

        buffer << "Heap: " << i << "\n";
        buffer << "\tSize:\t" << heap.size << "\n";
        buffer << "\tFlags:" << "\n";
        buffer << "\t\tDevice local:\t" << ((heap.flags & vk::MemoryHeapFlagBits::eDeviceLocal) == vk::MemoryHeapFlagBits::eDeviceLocal) << "\n";
        buffer << "\t\tMulti instance:\t" << ((heap.flags & vk::MemoryHeapFlagBits::eMultiInstanceKHX) == vk::MemoryHeapFlagBits::eMultiInstanceKHX) << "\n";
        buffer << "\n";
    }

    for(int i = 0; i <  memProperties.memoryTypeCount; i ++) {
        vk::MemoryType type = memProperties.memoryTypes[i];
        vk::MemoryPropertyFlags flags = type.propertyFlags;

        buffer << "Type: " << i << "\n";
        buffer << "\tHeap index: " << type.heapIndex << "\n";
        buffer << "\tFlags" << "\n";
        buffer << "\t\tDevice local:\t\t" << compareFlagBit(flags, vk::MemoryPropertyFlagBits::eDeviceLocal) << "\n";
        buffer << "\t\tHost visible:\t\t" << compareFlagBit(flags, vk::MemoryPropertyFlagBits::eHostVisible) << "\n";
        buffer << "\t\tHost coherent:\t\t" << compareFlagBit(flags, vk::MemoryPropertyFlagBits::eHostCoherent) << "\n";
        buffer << "\t\tHost cached:\t\t" << compareFlagBit(flags, vk::MemoryPropertyFlagBits::eHostCached) << "\n";
        buffer << "\t\tLazily allocated:\t" << compareFlagBit(flags, vk::MemoryPropertyFlagBits::eLazilyAllocated) << "\n";
        buffer << "\n";
    }

    return buffer.str();
}


int main() {

    auto session = ll::Session::create();
    cout << getPhysicalDevicePropertiesString(session->getPhysicalDeviceMemoryProperties());

    return EXIT_SUCCESS;
}