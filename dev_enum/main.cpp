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

struct Info
{
    VkInstance inst;
    std::vector<GpuInfo> gpus_info;
};

struct layerProperties {
    VkLayerProperties prop;
    std::vector<VkExtensionProperties> extentions;
};

struct VulkanLayers {
    std::vector<layerProperties> properties;
    std::vector<const char*> c_names;
    uint32_t count;
};

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
    for ( auto& n: layers.properties)
        layers.c_names.push_back(n.prop.layerName);
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
        vkGetPhysicalDeviceQueueFamilyProperties(g, &ginfo.queue_fam_count, nullptr);
        ginfo.queue_props.resize(ginfo.queue_fam_count);
        vkGetPhysicalDeviceQueueFamilyProperties(g, &ginfo.queue_fam_count, ginfo.queue_props.data());

        vkGetPhysicalDeviceProperties(g, &ginfo.props);
        vkGetPhysicalDeviceMemoryProperties(g, &ginfo.mem);
        i.gpus_info.push_back(ginfo);
    }
}

int main(int,char**)
{
    Info info{};
    VulkanLayers layers{};
    init_layers(layers);
    init_instance(info.inst, layers);

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

    std::cout << "found " << layers.count << " Vulkan Layers\n";
    for(auto& layer: layers.c_names)
    {
        std::cout << "\t" << layer << '\n';
    }

    vkDestroyInstance(info.inst, NULL);
    return 0;
}
