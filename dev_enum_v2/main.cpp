#include <vulkan/vulkan.hpp>
#include <vector>
#include <iostream>
#include <string>


struct GpuInfo
{
    vk::PhysicalDevice g;
    vk::PhysicalDeviceMemoryProperties mem;
    vk::PhysicalDeviceProperties props;
    std::vector<vk::QueueFamilyProperties> queue_props;
};

struct layerProperties {
    vk::LayerProperties prop;
    std::vector<vk::ExtensionProperties> extensions;
    std::string name;
};

struct VulkanLayers {
    std::vector<layerProperties> properties;
    std::vector<std::string> c_names;
};

struct Info
{
    vk::UniqueInstance inst;
    std::vector<GpuInfo> gpus_info;
    std::vector<vk::ExtensionProperties> ext;
    VulkanLayers layers;
};

void init_layers(VulkanLayers &layers)
{
    auto vkprop = vk::enumerateInstanceLayerProperties();
    if (vkprop.size() == 0)
        return;
    for( auto& p: vkprop)
    {
        layerProperties prop{};
        prop.prop = p;
        prop.name = std::string{p.layerName};
        prop.extensions = vk::enumerateInstanceExtensionProperties(prop.name);
        layers.properties.push_back(prop);
        layers.c_names.push_back(prop.name);
    }
}

auto fill_info(void) -> vk::UniqueInstance
{
    vk::ApplicationInfo appInfo{"devEnum", 1, "vk++Tut", 1, VK_API_VERSION_1_1};
    vk::InstanceCreateInfo createInfo{{}, &appInfo};
    return vk::createInstanceUnique(createInfo);
}

void printGpu(const GpuInfo &gpu)
{
    std::cout << gpu.props.deviceName << " type: '";
    std::cout << "(0x" << std::hex << (uintptr_t)(&gpu.g) << std::dec << ")' ";
    switch(gpu.props.deviceType){
        case vk::PhysicalDeviceType::eOther:
            std::cout << "other";
            break;
        case vk::PhysicalDeviceType::eIntegratedGpu:
            std::cout << "igpu";
            break;
        case vk::PhysicalDeviceType::eDiscreteGpu:
            std::cout << "gpu";
            break;
        case vk::PhysicalDeviceType::eVirtualGpu:
            std::cout << "vgpu";
            break;
        case vk::PhysicalDeviceType::eCpu:
            std::cout << "cpu";
            break;
        default:
            std::cout << "unknown";
            break;
    }
    std::cout << '\n';
    for (auto& qp : gpu.queue_props) {
        if (qp.queueFlags & vk::QueueFlagBits::eGraphics)
            std::cout << "\tFound Graphics queue w/ " << qp.queueCount << " queue(s)\n";
        if (qp.queueFlags & vk::QueueFlagBits::eCompute)
            std::cout << "\tFound Compute queue w/ " << qp.queueCount << " queue(s)\n";
        if (qp.queueFlags & vk::QueueFlagBits::eTransfer)
            std::cout << "\tFound Transfer queue w/ " << qp.queueCount << " queue(s)\n";
        if (qp.queueFlags & vk::QueueFlagBits::eSparseBinding)
            std::cout << "\tFound Sparse Binding queue w/ " << qp.queueCount << " queue(s)\n";
        if (qp.queueFlags & vk::QueueFlagBits::eProtected)
            std::cout << "\tFound Protected queue w/ " << qp.queueCount << " queue(s)\n";
    }
}

void
fillGpus(const vk::UniqueInstance &i)
{
    for (auto &g : i->enumeratePhysicalDevices())
    {
        GpuInfo info{};
        info.g = g;
        info.queue_props = g.getQueueFamilyProperties();
        info.props = g.getProperties();
        info.mem = g.getMemoryProperties();
        printGpu(info);
    }
}
int main(int p1, char **p2)
{
    Info info;
    try {
        info.ext = vk::enumerateInstanceExtensionProperties();
        init_layers(info.layers);
        info.inst = fill_info();
        fillGpus(info.inst);
    }
    catch(vk::SystemError err)
    {
        std::cout << err.what() << '\n';
        return -1;
    }
    std::cout << "found " << info.layers.properties.size() << " Vulkan Layers\n";
    for(auto& layer: info.layers.properties)
    {
        std::cout << "\t" << layer.name << '\n';
        for(auto &e: layer.extensions)
            std::cout << "\t\t" << e.extensionName << '\n';
    }

    std::cout << "found " << info.ext.size() << " Vulkan extension on this platform\n";
    for(auto &e : info.ext)
        std::cout << "\t" << e.extensionName << '\n';
    return 0;
}
