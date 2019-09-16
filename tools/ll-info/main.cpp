
#include "lluvia/core.h"

#include "json/json.hpp"

#include <iostream>


using namespace std;

using json = nlohmann::json;


void getLayerProperties(json& jsonParent) {

    // placeholder
    jsonParent["layers"] = nullptr;

    auto layerProperties = ll::Session::getVulkanInstanceLayerProperties();
    

    for (const auto& layer : layerProperties) {

        auto jsonLayer = json {};
        jsonLayer["name"] = layer.layerName;
        jsonLayer["description"] = layer.description;
        jsonLayer["spec_version"] = layer.specVersion;
        jsonLayer["impl_version"] = layer.implementationVersion;

        jsonParent["layers"].push_back(jsonLayer);
    }
}


void getPhysicalDeviceMemoryProperties(const vk::PhysicalDeviceMemoryProperties& memProperties, json& jsonParent) {

    auto compareFlagBit = [](const auto& flag, const auto& value) {
        return ((flag & value) == value);
    };

    auto jsonMemProp = json {};

    auto jsonHeaps = json {};
    for(auto i = 0u; i < memProperties.memoryHeapCount; i ++) {
        vk::MemoryHeap heap = memProperties.memoryHeaps[i];

        auto jsonHeap = json {};
        jsonHeap["size"] = heap.size;
        jsonHeap["flags"] = {
                {"device_local", ((heap.flags & vk::MemoryHeapFlagBits::eDeviceLocal) == vk::MemoryHeapFlagBits::eDeviceLocal)},
                {"multi_instance", ((heap.flags & vk::MemoryHeapFlagBits::eMultiInstanceKHR) == vk::MemoryHeapFlagBits::eMultiInstanceKHR)}
            };
        jsonHeaps.push_back(jsonHeap);
    }
    jsonMemProp["heaps"] = jsonHeaps;

    auto jsonMemories = json {};
    for(auto i = 0u; i <  memProperties.memoryTypeCount; i ++) {
        vk::MemoryType type = memProperties.memoryTypes[i];
        vk::MemoryPropertyFlags flags = type.propertyFlags;

        auto jsonMem = json {};
        jsonMem["heap_index"] = type.heapIndex;
        jsonMem["flags"] = {
                {"device_local"    , compareFlagBit(flags, vk::MemoryPropertyFlagBits::eDeviceLocal)},
                {"host_visible"    , compareFlagBit(flags, vk::MemoryPropertyFlagBits::eHostVisible)},
                {"host_coherent"   , compareFlagBit(flags, vk::MemoryPropertyFlagBits::eHostCoherent)},
                {"host_cached"     , compareFlagBit(flags, vk::MemoryPropertyFlagBits::eHostCached)},
                {"lazily_allocated", compareFlagBit(flags, vk::MemoryPropertyFlagBits::eLazilyAllocated)}
            };

        jsonMemories.push_back(jsonMem);
    }
    jsonMemProp["memories"] = jsonMemories;

    jsonParent["memory_properties"] = jsonMemProp;
}


void getCompatibleImageFormats(__attribute__((unused)) std::shared_ptr<ll::Session>& session, json& jsonParent) {

    auto formats = json {};

    // constexpr const auto channelCountValues = std::array<ll::ChannelCount, 4> {{
    //     ll::ChannelCount::C1,
    //     ll::ChannelCount::C2,
    //     ll::ChannelCount::C3,
    //     ll::ChannelCount::C4
    // }};

    // for (const auto c : channelCountValues) {

    //     auto channelFormats = json {};
    //     channelFormats["channels"] = static_cast<int32_t>(c);


    //     for (auto& cType : ll::impl::ChannelTypeStrings) {

    //         if (session->isImageFormatSupported(c, std::get<1>(cType))) {
    //             channelFormats["formats"].push_back(std::get<0>(cType));
    //         }

    //     }

    //     formats.push_back(channelFormats);
    // }

    jsonParent["image_formats"] = formats;
}


int main() {

    auto j = json {};

    auto session = ll::Session::create();

    getLayerProperties(j);
    getPhysicalDeviceMemoryProperties(session->getPhysicalDeviceMemoryProperties(), j);
    getCompatibleImageFormats(session, j);

    cout << std::setw(2) << j << std::endl;

    return EXIT_SUCCESS;
}
