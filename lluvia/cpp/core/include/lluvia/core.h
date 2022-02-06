/**
@file       core.h
@brief      Lluvia single header for including core classes.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_H_
#define LLUVIA_CORE_H_

#include "core/memory/Memory.h"
#include "core/memory/MemoryPropertyFlags.h"
#include "core/memory/MemoryAllocationInfo.h"

#include "core/buffer/Buffer.h"
#include "core/buffer/BufferUsageFlags.h"

#include "core/device/DeviceDescriptor.h"
#include "core/device/DeviceType.h"

#include "core/image/Image.h"
#include "core/image/ImageAddressMode.h"
#include "core/image/ImageAxis.h"
#include "core/image/ImageDescriptor.h"
#include "core/image/ImageFilterMode.h"
#include "core/image/ImageLayout.h"
#include "core/image/ImageTiling.h"
#include "core/image/ImageUsageFlags.h"
#include "core/image/ImageView.h"
#include "core/image/ImageViewDescriptor.h"

#include "core/node/NodeState.h"

#include "core/CommandBuffer.h"
#include "core/ComputeNode.h"
#include "core/ComputeNodeDescriptor.h"
#include "core/ContainerNode.h"
#include "core/ContainerNodeDescriptor.h"
#include "core/Duration.h"
#include "core/error.h"
#include "core/Interpreter.h"
#include "core/Node.h"
#include "core/NodeBuilderDescriptor.h"
#include "core/Parameter.h"
#include "core/Program.h"
#include "core/PushConstants.h"
#include "core/Session.h"
#include "core/types.h"
#include "core/utils.h"

#endif // LLUVIA_CORE_H_
