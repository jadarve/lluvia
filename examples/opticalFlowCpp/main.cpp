
#include <chrono>
#include <iostream>
#include <lluvia/core.h>
#include <opencv2/opencv.hpp>

constexpr const auto CONTAINER_NODE_CODE = R"(
local builder = ll.class(ll.ContainerNodeBuilder)

builder.name = 'webcam/HornSchunck'
builder.doc = [[
A calculator transforming computing optical flow using the Horn and Schunck algorithm.

]]

function builder.newDescriptor()

    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = builder.name

    local in_image = ll.PortDescriptor.new(0, 'in_image', ll.PortDirection.In, ll.PortType.ImageView)
    desc:addPort(in_image)

    local out_image = ll.PortDescriptor.new(1, 'out_image', ll.PortDirection.Out, ll.PortType.ImageView)
    desc:addPort(out_image)

    return desc
end


function builder.onNodeInit(node)

    ll.logd(node.descriptor.builderName, 'onNodeInit')

    local in_image = node:getPort('in_image')

    -- BGRA to Gray
    local BGRA2Gray = ll.createComputeNode('lluvia/color/BGRA2Gray')
    BGRA2Gray:bind('in_bgra', in_image)
    BGRA2Gray:init()
    node:bindNode('BGRA2Gray', BGRA2Gray)

    -- immediately run this node to convert the input BGRA to gray. This is necessary
    -- so that the Horn and Schunck node can be initialized with an actual image and
    -- not with zeros.
    ll.run(BGRA2Gray)

    local out_gray = BGRA2Gray:getPort('out_gray')

    -- Horn and Schunck
    local HornSchunck = ll.createContainerNode('lluvia/opticalflow/HornSchunck/HornSchunck')
    HornSchunck:setParameter('alpha', 0.05)
    HornSchunck:setParameter('iterations', 20)
    HornSchunck:setParameter('float_precision', ll.FloatPrecision.FP32)
    HornSchunck:setParameter('clear_history', 1)
    HornSchunck:bind('in_gray', out_gray)

    HornSchunck:init()
    node:bindNode('HornSchunck', HornSchunck)
    
    local out_flow = HornSchunck:getPort('out_flow')

    -- Flow to RGBA
    local Flow2RGBA = ll.createComputeNode('lluvia/viz/Flow2RGBA')
    Flow2RGBA:setParameter('max_flow', 16.0)
    Flow2RGBA:bind('in_flow', out_flow)
    Flow2RGBA:init()
    node:bindNode('Flow2RGBA', Flow2RGBA)

    local flowRGBA = Flow2RGBA:getPort('out_rgba')

    -- RGBA to BGRA
    local RGBA2BGRA = ll.createComputeNode('lluvia/color/RGBA2BGRA')
    RGBA2BGRA:bind('in_rgba', flowRGBA)
    RGBA2BGRA:init()
    node:bindNode('RGBA2BGRA', RGBA2BGRA)

    local out_bgra = RGBA2BGRA:getPort('out_bgra')
    
    -- bind the output
    node:bind('out_image', out_bgra)

    ll.logd(node.descriptor.builderName, 'onNodeInit: finish')

end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd(node.descriptor.builderName, 'onNodeRecord')

    local BGRA2Gray = node:getNode('BGRA2Gray')
    local HornSchunck = node:getNode('HornSchunck')
    local Flow2RGBA = node:getNode('Flow2RGBA')
    local RGBA2BGRA = node:getNode('RGBA2BGRA')

    BGRA2Gray:record(cmdBuffer)
    cmdBuffer:memoryBarrier()

    HornSchunck:record(cmdBuffer)
    -- no memory barrier here as HS container node puts a barrier at the end

    Flow2RGBA:record(cmdBuffer)
    cmdBuffer:memoryBarrier()

    RGBA2BGRA:record(cmdBuffer)
    cmdBuffer:memoryBarrier()

    ll.logd(node.descriptor.builderName, 'onNodeRecord: finish')
end


ll.registerNodeBuilder(builder)

)";

int main(int argc, char** argv)
{

    if (argc != 2) {
        std::cout << "missing library path, use 'opticalFlowCpp <path-to-library>'" << std::endl;
        return EXIT_FAILURE;
    }

    auto libraryPath = std::string {argv[1]};

    ///////////////////////////////////////////////////////////////////////////
    // Enumerate available

    auto availableDevices = ll::Session::getAvailableDevices();
    auto selectedDevice   = availableDevices[0];

    std::cout << "------------------------------------------------------------" << std::endl;
    for (auto& device : ll::Session::getAvailableDevices()) {
        std::cout << "ID: " << device.id << " type: " << ll::deviceTypeToString(std::forward<ll::DeviceType>(device.deviceType)) << " name: " << device.name << std::endl;

        if (device.deviceType == ll::DeviceType::DiscreteGPU) {
            selectedDevice = device;
        }
    }
    std::cout << "------------------------------------------------------------" << std::endl;

    ///////////////////////////////////////////////////////////////////////////
    // Create lluvia session and load the node library

    auto sessionDescriptor = ll::SessionDescriptor {}
                                 .setDeviceDescriptor(selectedDevice)
                                 .enableDebug(true);

    auto session = std::shared_ptr<ll::Session> {ll::Session::create(sessionDescriptor)};
    session->loadLibrary(libraryPath);

    // load the script for the optical flow container node
    session->script(CONTAINER_NODE_CODE);

    auto device = session->getDeviceDescriptor();
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "Using device ID: " << device.id << " type: " << ll::deviceTypeToString(std::forward<ll::DeviceType>(device.deviceType)) << " name: " << device.name << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    // enumerate all available node descriptors
    for (const auto& nodeDesc : session->getNodeBuilderDescriptors()) {
        std::cout << nodeDesc.name << std::endl;
    }

    // memory to create staging buffers in and out of the GPU
    auto hostMemory = session->getHostMemory();

    // memory to create buffers in the GPU
    auto deviceMemory = session->getDeviceMemory();

    /////////////////////////////////////////////
    // Nodes
    auto opticalFlowNode = session->createContainerNode("webcam/HornSchunck");

    ///////////////////////////////////////////////////////////////////////////
    // Open camera
    cv::VideoCapture cap;
    cap.open(0); // open the first camera available

    if (!cap.isOpened()) {
        std::cerr << "couldn't open capture." << std::endl;
        return EXIT_FAILURE;
    }

    // OpenCV images to store the input and output frames
    cv::Mat inputFrame;
    cv::Mat inputFrameBGRA;

    // read the first frame in order to get the size
    cap >> inputFrame;
    if (inputFrame.empty()) {
        std::cerr << "couldn't read frame." << std::endl;
        return EXIT_FAILURE;
    }

    // convert to BGRA the first time to obtain the size of the image buffer needed to allocate the staging buffer
    cv::cvtColor(inputFrame, inputFrameBGRA, cv::COLOR_BGR2BGRA);

    auto inputStagingBuffer = hostMemory->createBuffer(inputFrameBGRA.step * inputFrameBGRA.rows);

    // recreate the inputFrameBGRA image using the staging buffer as memory storage
    auto inputStagingBufferMapped = inputStagingBuffer->map<uint8_t>();
    inputFrameBGRA                = cv::Mat(cv::Size(inputFrameBGRA.cols, inputFrameBGRA.rows), CV_8UC4, inputStagingBufferMapped.get(), inputFrameBGRA.step);

    // as the inputFrameBGRA was recreated, we need to convert the inputFrame to BGRA again
    cv::cvtColor(inputFrame, inputFrameBGRA, cv::COLOR_BGR2BGRA);

    // CV image to receive the colored optical flow and display on screen
    auto outputStagingBuffer       = hostMemory->createBuffer(inputFrameBGRA.step * inputFrameBGRA.rows);
    auto outputStagingBufferMapped = outputStagingBuffer->map<uint8_t>();
    auto outputFlowColorBGRA       = cv::Mat(cv::Size(inputFrameBGRA.cols, inputFrameBGRA.rows), CV_8UC4, outputStagingBufferMapped.get(), inputFrameBGRA.step);

    // input image
    auto inputImageDesc = ll::ImageDescriptor {}
                              .setWidth(inputFrameBGRA.cols)
                              .setHeight(inputFrameBGRA.rows)
                              .setDepth(1)
                              .setChannelCount(ll::ChannelCount::C4)
                              .setChannelType(ll::ChannelType::Uint8)
                              .setUsageFlags(ll::ImageUsageFlagBits::Storage
                                             | ll::ImageUsageFlagBits::Sampled
                                             | ll::ImageUsageFlagBits::TransferDst);

    auto deviceInputImage     = ll::createAndInitImage(session, deviceMemory, inputImageDesc, ll::ImageLayout::General);
    auto deviceInputImageView = deviceInputImage->createImageView(ll::ImageViewDescriptor {ll::ImageAddressMode::ClampToEdge,
        ll::ImageFilterMode::Nearest,
        false,   // normalized coordinates
        false}); // is sampled

    ///////////////////////////////////////////////////////////////////////////
    // Node initialization
    opticalFlowNode->bind("in_image", deviceInputImageView);

    // Copy the initial image from the staging buffer to deviceInputImage in order to initialize
    // the optical flow node with actual data
    auto copyImgCmdBuffer = session->createCommandBuffer();
    copyImgCmdBuffer->begin();
    copyImgCmdBuffer->changeImageLayout(*deviceInputImage, ll::ImageLayout::TransferDstOptimal);
    copyImgCmdBuffer->copyBufferToImage(*inputStagingBuffer, *deviceInputImage);
    copyImgCmdBuffer->changeImageLayout(*deviceInputImage, ll::ImageLayout::General);
    copyImgCmdBuffer->end();
    session->run(*copyImgCmdBuffer);

    opticalFlowNode->init();

    auto deviceOutputImageView = std::static_pointer_cast<ll::ImageView>(opticalFlowNode->getPort("out_image"));
    auto deviceOutputImage     = deviceOutputImageView->getImage();

    ///////////////////////////////////////////////////////////////////////////
    // Duration to record node runtime
    auto duration = session->createDuration();

    ///////////////////////////////////////////////////////////////////////////
    // Command buffer
    auto cmdBuffer = session->createCommandBuffer();
    cmdBuffer->begin();
    cmdBuffer->changeImageLayout(*deviceInputImage, ll::ImageLayout::TransferDstOptimal);
    cmdBuffer->copyBufferToImage(*inputStagingBuffer, *deviceInputImage);
    cmdBuffer->changeImageLayout(*deviceInputImage, ll::ImageLayout::General);

    cmdBuffer->durationStart(*duration);
    cmdBuffer->run(*opticalFlowNode);
    cmdBuffer->durationEnd(*duration);

    cmdBuffer->changeImageLayout(*deviceOutputImage, ll::ImageLayout::TransferSrcOptimal);
    cmdBuffer->copyImageToBuffer(*deviceOutputImage, *outputStagingBuffer);
    cmdBuffer->changeImageLayout(*deviceOutputImage, ll::ImageLayout::General);
    cmdBuffer->end();

    for (;;) {

        cap >> inputFrame;

        // convert to BGRA
        cv::cvtColor(inputFrame, inputFrameBGRA, cv::COLOR_BGR2BGRA);

        if (inputFrame.empty()) {
            break;
        }

        session->run(*cmdBuffer);

        // display the runtime of the node
        auto runtime_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(duration->getDuration()).count();
        cv::putText(inputFrame, std::to_string(runtime_ms) + " ms", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);

        cv::imshow("input image", inputFrame);
        cv::imshow("optical flow", outputFlowColorBGRA);

        if (cv::waitKey(30) >= 0)

            break;
    }

    return EXIT_SUCCESS;
}