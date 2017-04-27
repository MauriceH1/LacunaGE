#include "LacunaVRenderer.h"
#include "VulkanHelpers.h"
#include <iostream>

using namespace lcn::graphics;

LacunaVRenderer::LacunaVRenderer()
    : Renderer()
{
    m_Data = new helpers::VkData;
    m_Data->instance_create_info.pApplicationInfo = &m_Data->app_info;
}

LacunaVRenderer::~LacunaVRenderer()
{

}

bool LacunaVRenderer::Initialize(const lcn::platform::specifics::PlatformHandles* a_Handles)
{
    helpers::init_global_layer_properties(*m_Data);
    helpers::init_instance_extension_names(*m_Data);

    VkResult res = vkCreateInstance(&m_Data->instance_create_info, NULL, &m_Data->instance);
    if(res == VK_ERROR_INCOMPATIBLE_DRIVER)
    {
        std::cout << "Cannot find a compatible Vulkan Driver!\n";
        return false;
    }
    else if(res)
    {
        std::cout << "unknown error: " << res << "\n";
        return false;
    }
    std::cout << "VULKAN INSTANCE ALIVE\n";

    uint32_t gpu_count = 0;
    res = vkEnumeratePhysicalDevices(m_Data->instance, &gpu_count, nullptr);
    m_Data->gpus.resize(gpu_count);
    res = vkEnumeratePhysicalDevices(m_Data->instance, &gpu_count, m_Data->gpus.data());

    VkDeviceQueueCreateInfo queue_info = {};
    vkGetPhysicalDeviceQueueFamilyProperties(m_Data->gpus[0], &m_Data->queue_family_count, NULL);
    assert(m_Data->queue_family_count >= 1);

    m_Data->queue_props.resize(m_Data->queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(m_Data->gpus[0], &m_Data->queue_family_count, m_Data->queue_props.data());
    assert(m_Data->queue_family_count >= 1);

    bool found = false;
    for(unsigned int i = 0; i < m_Data->queue_family_count; i++)
    {
        if(m_Data->queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queue_info.queueFamilyIndex = i;
            found = true;
            break;
        }
    }
    assert(found);
    assert(m_Data->queue_family_count >= 1);

    float queue_priorities[1] = {0.0};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    queue_info.pNext = nullptr;
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = queue_priorities;

    m_Data->device_info.pQueueCreateInfos = &queue_info;

    res = vkCreateDevice(m_Data->gpus[0], &m_Data->device_info, nullptr, &m_Data->device);
    assert(res == VK_SUCCESS);
    std::cout << "VULKAN DEVICE CREATED\n";

    VkCommandPoolCreateInfo cmd_pool_info = {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        nullptr,
        0,
        queue_info.queueFamilyIndex
    };
    res = vkCreateCommandPool(m_Data->device, &cmd_pool_info, nullptr, &m_Data->graphics_cmd_pool);
    assert(res == VK_SUCCESS);

    VkCommandBufferAllocateInfo g_cmd_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        nullptr,
        m_Data->graphics_cmd_pool,
        VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        1
    };

    res = vkAllocateCommandBuffers(m_Data->device, &g_cmd_info, &m_Data->graphics_cmd_buf);
    assert(res == VK_SUCCESS);
    std::cout << "VULKAN COMMAND BUFFERS ALLOCATED\n";

#ifdef _WIN32
    //VkWin32SurfaceCreateInfoKHR createInfo = {};
    //createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    //createInfo.pNext = NULL;
    //createInfo.hinstance = info.connection;
    //createInfo.hwnd = info.window;
    //res = vkCreateWin32SurfaceKHR(info.inst, &createInfo, NULL, &info.surface);
#endif

    return 0;
}

void LacunaVRenderer::Render(lcn::object::Entity* a_RootEntity)
{
    
}

void LacunaVRenderer::Cleanup()
{
    VkCommandBuffer cmd_bufs[1] = { m_Data->graphics_cmd_buf };
    vkFreeCommandBuffers(m_Data->device, m_Data->graphics_cmd_pool, 1, cmd_bufs);
    vkDestroyCommandPool(m_Data->device, m_Data->graphics_cmd_pool, nullptr);
    vkDestroyDevice(m_Data->device, nullptr);
    vkDestroyInstance(m_Data->instance, NULL);
}