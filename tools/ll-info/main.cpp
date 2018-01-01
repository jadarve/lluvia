
#include "lluvia/core.h"

#include <cstdlib>
#include <iostream>
#include <sstream>      // stringbuf
#include <string>


using namespace std;


std::string getLayerProperties() {

    auto buffer = std::ostringstream {};
    buffer << "************************************************************\n";
    buffer << "                   LAYER PROPERTIES\n";
    buffer << "************************************************************\n";

    auto layerProperties = ll::Session::getVulkanInstanceLayerProperties();
    

    for (const auto& layer : layerProperties) {
        
        buffer << "Name:                   " << layer.layerName << "\n";
        buffer << "Description:            " << layer.description << "\n";
        buffer << "Spec version:           " << layer.specVersion << "\n";
        buffer << "implementation version: " << layer.implementationVersion << "\n\n";
    }


    return buffer.str();
}

std::string getPhysicalDevicePropertiesString(const vk::PhysicalDeviceMemoryProperties& memProperties) {

    auto compareFlagBit = [](const auto& flag, const auto& value) {
        return ((flag & value) == value);
    };

    auto buffer = std::ostringstream {};

    buffer << "************************************************************\n";
    buffer << "           PHYSICAL DEVICE MEMORY PROPERTIES\n";
    buffer << "************************************************************\n";
    buffer << "Heap count:          " << memProperties.memoryHeapCount << "\n";
    buffer << "Memory type count:   " << memProperties.memoryTypeCount << "\n\n";


    for(auto i = 0u; i < memProperties.memoryHeapCount; i ++) {
        vk::MemoryHeap heap = memProperties.memoryHeaps[i];

        buffer << "Heap: " << i << "\n";
        buffer << "    Size:    " << heap.size << "\n";
        buffer << "    Flags:" << "\n";
        buffer << "        Device local:    " << ((heap.flags & vk::MemoryHeapFlagBits::eDeviceLocal) == vk::MemoryHeapFlagBits::eDeviceLocal) << "\n";
        buffer << "        Multi instance:  " << ((heap.flags & vk::MemoryHeapFlagBits::eMultiInstanceKHX) == vk::MemoryHeapFlagBits::eMultiInstanceKHX) << "\n";
        buffer << "\n";
    }

    for(auto i = 0u; i <  memProperties.memoryTypeCount; i ++) {
        vk::MemoryType type = memProperties.memoryTypes[i];
        vk::MemoryPropertyFlags flags = type.propertyFlags;

        buffer << "Type: " << i << "\n";
        buffer << "    Heap index: " << type.heapIndex << "\n";
        buffer << "    Flags" << "\n";
        buffer << "        Device local:      " << compareFlagBit(flags, vk::MemoryPropertyFlagBits::eDeviceLocal) << "\n";
        buffer << "        Host visible:      " << compareFlagBit(flags, vk::MemoryPropertyFlagBits::eHostVisible) << "\n";
        buffer << "        Host coherent:     " << compareFlagBit(flags, vk::MemoryPropertyFlagBits::eHostCoherent) << "\n";
        buffer << "        Host cached:       " << compareFlagBit(flags, vk::MemoryPropertyFlagBits::eHostCached) << "\n";
        buffer << "        Lazily allocated:  " << compareFlagBit(flags, vk::MemoryPropertyFlagBits::eLazilyAllocated) << "\n";
        buffer << "\n";
    }

    return buffer.str();
}


int main() {

    auto session = ll::Session::create();
    cout << getLayerProperties();
    cout << getPhysicalDevicePropertiesString(session->getPhysicalDeviceMemoryProperties());

    return EXIT_SUCCESS;
}
