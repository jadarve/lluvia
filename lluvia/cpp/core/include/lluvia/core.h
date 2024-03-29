/**
@file       core.h
@brief      Lluvia single header for including core classes.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_H_
#define LLUVIA_CORE_H_

#include "core/memory/Memory.h"
#include "core/memory/MemoryAllocationInfo.h"
#include "core/memory/MemoryPropertyFlags.h"

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

#include "core/node/ComputeNode.h"
#include "core/node/ComputeNodeDescriptor.h"
#include "core/node/ContainerNode.h"
#include "core/node/ContainerNodeDescriptor.h"
#include "core/node/Node.h"
#include "core/node/NodeBuilderDescriptor.h"
#include "core/node/NodeState.h"
#include "core/node/NodeType.h"
#include "core/node/Parameter.h"
#include "core/node/ParameterType.h"
#include "core/node/PortDescriptor.h"
#include "core/node/PortDirection.h"
#include "core/node/PortType.h"
#include "core/node/PushConstants.h"

#include "core/CommandBuffer.h"
#include "core/Duration.h"
#include "core/FloatPrecision.h"
#include "core/Interpreter.h"
#include "core/Program.h"
#include "core/Session.h"
#include "core/error.h"
#include "core/types.h"
#include "core/utils.h"

#endif // LLUVIA_CORE_H_
