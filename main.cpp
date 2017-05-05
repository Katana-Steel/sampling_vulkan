#include <string>
#include <iostream>
#include <vector>
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

void init_instance(Info &i)
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
    inst_info.enabledLayerCount = 0;
    inst_info.ppEnabledLayerNames = nullptr;
    inst_info.enabledExtensionCount = 0;
    inst_info.ppEnabledExtensionNames = nullptr;

    VkResult res = vkCreateInstance(&inst_info, NULL, &i.inst);

}

void fillInGpus(Info &i, uint32_t count)
{
    if (count < 1)
        return;
    std::vector<VkPhysicalDevice> gpus;
    gpus.resize(count);
    vkEnumeratePhysicalDevices(i.inst, &count, gpus.data());
    for (VkPhysicalDevice g : gpus)
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
    init_instance(info);

    uint32_t gpu_count=0;
    VkResult res = vkEnumeratePhysicalDevices(info.inst, &gpu_count, nullptr);
    std::cout << "found " << gpu_count << " vulkan enabled gpus\n";
    fillInGpus(info, gpu_count);
    for (GpuInfo i : info.gpus_info)
    {
        std::cout << i.props.deviceName << "\n";
        for (auto qp : i.queue_props) {
            if (qp.queueFlags & VK_QUEUE_GRAPHICS_BIT)
              std::cout << " Found Graphics queue\n";
            if (qp.queueFlags & VK_QUEUE_COMPUTE_BIT)
              std::cout << " Found Compute queue\n";
            if (qp.queueFlags & VK_QUEUE_TRANSFER_BIT)
              std::cout << " Found Transfer queue\n";
            if (qp.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
              std::cout << " Found Sparse Binding queue\n";
        }
    }

    

    vkDestroyInstance(info.inst, NULL);
    return 0;
}