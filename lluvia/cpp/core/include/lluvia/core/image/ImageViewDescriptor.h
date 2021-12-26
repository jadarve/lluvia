/**
@file       ImageViewDescriptor.h
@brief      ImageViewDescriptor class and related enumerations and methods.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IMAGE_IMAGE_VIEW_DESCRIPTOR_H_
#define LLUVIA_CORE_IMAGE_IMAGE_VIEW_DESCRIPTOR_H_

#include "lluvia/core/image/ImageAddressMode.h"
#include "lluvia/core/image/ImageAxis.h"
#include "lluvia/core/image/ImageFilterMode.h"

#include <array>
#include <cstdint>
#include <tuple>
#include <string>

#include "lluvia/core/vulkan/vulkan.hpp"


namespace ll {

class ImageView;


/**
@brief      Descriptor for ll::ImageView objects.

The following code creates a descriptor for a sampled image using nearest
filtering and unnormalized coordinates. The repeat address mode is used
for pixels outside the image boundaries.

@code
    auto imgViewDesc = ll::ImageViewDescriptor {}
                        .setNormalizedCoordinates(false)
                        .setIsSampled(true)
                        .setAddressMode(ll::ImageAddressMode::Repeat)
                        .setFilterMode(ll::ImageFilterMode::Nearest);
@endcode
*/
class ImageViewDescriptor {

public:
    ImageViewDescriptor()                                                   = default;
    ImageViewDescriptor(const ImageViewDescriptor& descriptor)              = default;
    ImageViewDescriptor(ImageViewDescriptor&& descriptor)                   = default;

    ImageViewDescriptor(const ll::ImageAddressMode addressmode,
                        const ll::ImageFilterMode filterMode,
                        const bool normalizedCoordinates,
                        const bool isSampled);

    ~ImageViewDescriptor()                                                  = default;
    
    ImageViewDescriptor& operator = (const ImageViewDescriptor& descriptor) = default;
    ImageViewDescriptor& operator = (ImageViewDescriptor&& descriptor)      = default;


    /**
    @brief      Sets the filtering mode.
    
    @param[in]  filterMode  The filter mode
    
    @return     A reference to this object.
    */
    ImageViewDescriptor& setFilterMode(ll::ImageFilterMode filterMode) noexcept;


    /**
    @brief      Gets the filter mode.
    
    @return     The filter mode.
    */
    ll::ImageFilterMode getFilterMode() const noexcept;


    /**
    @brief      Sets the address mode to all image axes.
    
    @param[in]  addressMode  The address mode
    
    @return     A reference to this object.
    */
    ImageViewDescriptor& setAddressMode(ll::ImageAddressMode addressMode) noexcept;


    /**
    @brief      Sets the address mode to a given image axis.
    
    @param[in]  axis           The axis
    @param[in]  addressMode   The address mode
    
    @return     A reference to this object.
    */
    ImageViewDescriptor& setAddressMode(ll::ImageAxis axis, ll::ImageAddressMode addressMode) noexcept;


    /**
    @brief      Gets the address mode for the U axis.
    
    @return     The address mode.
    */
    ll::ImageAddressMode getAddressModeU() const noexcept;


    /**
    @brief      Gets the address mode for the V axis.
    
    @return     The address mode.
    */
    ll::ImageAddressMode getAddressModeV() const noexcept;


    /**
    @brief      Gets the address mode for the W axis.
    
    @return     The address mode.
    */
    ll::ImageAddressMode getAddressModeW() const noexcept;


    /**
    @brief      Sets whether or not the image view will use normalized coordinates.
    
    @param[in]  normalizedCoordinates  The normalized coordinates.
    
    @return     A reference to this object.
    */
    ImageViewDescriptor& setNormalizedCoordinates(bool normalizedCoordinates) noexcept;


    /**
    @brief      Determines if normalized coordinates are used for this image view.
    
    @return     True if normalized coordinates, False otherwise.
    */
    bool isNormalizedCoordinates() const noexcept;


    /**
    @brief      Sets whether or not the image view will be sampled.
    
    @param[in]  isSampled  Indicates if sampled
    
    @return     A reference to this object.
    */
    ImageViewDescriptor& setIsSampled(bool isSampled) noexcept;


    /**
    @brief      Determines if the image view is sampled.
    
    @return     True if sampled, False otherwise.
    */
    bool isSampled() const noexcept;


    /**
    @brief      Return the Vulkan sampler creation info filled from this object.
    
    See @VULKAN_DOC#VkSamplerCreateInfo

    @return     The Vulkan sampler create info.
    */
    vk::SamplerCreateInfo getVkSamplerCreateInfo() const noexcept;


private:
    ll::ImageFilterMode m_filterMode {ll::ImageFilterMode::Nearest};

    /**
     * Address mode for U, V, W axes
     */
    std::array<ll::ImageAddressMode, 3> m_addressMode {{ll::ImageAddressMode::Repeat,
                                                        ll::ImageAddressMode::Repeat,
                                                        ll::ImageAddressMode::Repeat}};

    bool m_normalizedCoordinates {false};
    bool m_isSampled             {false};

};


} // namespace ll

#endif /* LLUVIA_CORE_IMAGE_IMAGE_VIEW_DESCRIPTOR_H_ */
