/*
  Vulkan Device Enumeration
  by Rene Kjellerup (c) 2017
  <rk.katana.steel@gmail.com>

  this work is publish under GNU General Public License version 3
  or later. For licensening details see https://www.gnu.org/licenses/gpl.html

*/
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <vulkan/vk_sdk_platform.h>
#include <vulkan/vulkan.h>

struct GpuInfo
{
    VkPhysicalDevice g;
    VkPhysicalDeviceMemoryProperties mem;
    VkPhysicalDeviceProperties props;
    std::vector<VkQueueFamilyProperties> queue_props;
    uint32_t queue_fam_count;
};

struct layerProperties {
    VkLayerProperties prop;
    std::vector<VkExtensionProperties> extensions;
    const char *name;
};

struct VulkanLayers {
    std::vector<layerProperties> properties;
    std::vector<const char*> c_names;
    uint32_t count;
};

struct Info
{
    VkInstance inst;
    std::vector<GpuInfo> gpus_info;
    std::vector<VkExtensionProperties> ext;
    VulkanLayers layers;
};

void find_extensions(std::vector<VkExtensionProperties> &extensions, const char *name=nullptr)
{
    uint32_t count;
    vkEnumerateInstanceExtensionProperties(name, &count, nullptr);
    if ( count == 0 )
        return;
    extensions.resize(count);
    vkEnumerateInstanceExtensionProperties(name, &count, extensions.data());
}

void init_layers(VulkanLayers &layers)
{
    vkEnumerateInstanceLayerProperties(&layers.count, nullptr);
    if (layers.count == 0)
        return;
    std::vector<VkLayerProperties> vkprop(layers.count);
    vkEnumerateInstanceLayerProperties(&layers.count, vkprop.data());

    for( auto& p: vkprop)
    {
        layerProperties prop{};
        prop.prop = p;
        layers.properties.push_back(prop);
    }
    for ( auto& n: layers.properties) {
        n.name = n.prop.layerName;
        find_extensions(n.extensions, n.name);
        layers.c_names.push_back(n.prop.layerName);
    }
}

void init_instance(VkInstance &i, const VulkanLayers &layers)
{
    const char name[] = "katana_vk1";
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = nullptr;
    app_info.pApplicationName = name;
    app_info.applicationVersion = 1;
    app_info.pEngineName = name;
    app_info.engineVersion = 1;
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo inst_info = {};
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pNext = nullptr;
    inst_info.flags = 0;
    inst_info.pApplicationInfo = &app_info;
    inst_info.enabledLayerCount = layers.count;
    inst_info.ppEnabledLayerNames = (layers.count) ? layers.c_names.data(): nullptr;
    inst_info.enabledExtensionCount = 0;
    inst_info.ppEnabledExtensionNames = nullptr;

    VkResult res = vkCreateInstance(&inst_info, nullptr, &i);
}

void fillInGpus(Info &i, uint32_t count)
{
    if (count < 1)
        return;
    std::vector<VkPhysicalDevice> gpus;
    gpus.resize(count);
    vkEnumeratePhysicalDevices(i.inst, &count, gpus.data());
    for (auto& g : gpus)
    {
        GpuInfo ginfo{};
        ginfo.g = g;
        /* getting and saving available HW queues */
        i.gpus_info.push_back(ginfo);
    }
    for (auto &gpu: i.gpus_info)
    {
        vkGetPhysicalDeviceQueueFamilyProperties(gpu.g, &gpu.queue_fam_count, nullptr);
        gpu.queue_props.resize(gpu.queue_fam_count);
        vkGetPhysicalDeviceQueueFamilyProperties(gpu.g, &gpu.queue_fam_count, gpu.queue_props.data());

        vkGetPhysicalDeviceProperties(gpu.g, &gpu.props);
        vkGetPhysicalDeviceMemoryProperties(gpu.g, &gpu.mem);
    }
}

int main(int,char**)
{
    Info info{};
    find_extensions(info.ext);
    init_layers(info.layers);
    init_instance(info.inst, info.layers);

    uint32_t gpu_count=0;
    VkResult res = vkEnumeratePhysicalDevices(info.inst, &gpu_count, nullptr);
    std::cout << "found " << gpu_count << " vulkan enabled gpus\n";
    fillInGpus(info, gpu_count);
    for (auto& i : info.gpus_info)
    {
        std::cout << i.props.deviceName << " type: ";
        switch(i.props.deviceType){
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            std::cout << "other";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            std::cout << "igpu";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            std::cout << "gpu";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            std::cout << "vgpu";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            std::cout << "cpu";
            break;
        default:
            std::cout << "unknown";
            break;
        }
        std::cout << '\n';
        for (auto& qp : i.queue_props) {
            if (qp.queueFlags & VK_QUEUE_GRAPHICS_BIT)
              std::cout << "\tFound Graphics queue w/ " << qp.queueCount << " queue(s)\n";
            if (qp.queueFlags & VK_QUEUE_COMPUTE_BIT)
              std::cout << "\tFound Compute queue w/ " << qp.queueCount << " queue(s)\n";
            if (qp.queueFlags & VK_QUEUE_TRANSFER_BIT)
              std::cout << "\tFound Transfer queue w/ " << qp.queueCount << " queue(s)\n";
            if (qp.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
              std::cout << "\tFound Sparse Binding queue w/ " << qp.queueCount << " queue(s)\n";
        }
    }

    std::cout << "found " << info.layers.count << " Vulkan Layers\n";
    for(auto& layer: info.layers.properties)
    {
        std::cout << "\t" << layer.name << '\n';
        for(auto &e: layer.extensions)
            std::cout << "\t\t" << e.extensionName << '\n';
    }

    std::cout << "found " << info.ext.size() << " Vulkan extension on this platform\n";
    for(auto &e : info.ext)
        std::cout << "\t" << e.extensionName << '\n';
    vkDestroyInstance(info.inst, NULL);
    return 0;
}
