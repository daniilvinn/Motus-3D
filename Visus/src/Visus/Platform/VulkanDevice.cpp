#include "VulkanDevice.h"
#include <Visus/Platform/VulkanGraphicsContext.h>

#include <Visus/Core/Logger.h>

#include <vector>

namespace Motus3D
{
    // Vulkan Physical Device
    // ======================
    Ref<VulkanPhysicalDevice> VulkanPhysicalDevice::SelectDevice()
    {
        return CreateRef<VulkanPhysicalDevice>();
    }

    VulkanPhysicalDevice::VulkanPhysicalDevice()
    {
        //
        // TODO: check physical device's swapchain support when selecting device
        //

        auto vkinstance = VulkanGraphicsContext::GetVulkanContext()->GetInstance();

        // Enumerating all physical devices in system
        uint32_t physicalDeviceCount;
        vkEnumeratePhysicalDevices(vkinstance, &physicalDeviceCount, nullptr);
        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(vkinstance, &physicalDeviceCount, physicalDevices.data());

        for(auto& device : physicalDevices){
            VkPhysicalDeviceProperties physicalDeviceProperies;
            vkGetPhysicalDeviceProperties(device, &physicalDeviceProperies);
            if( physicalDeviceProperies.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
            {
				m_PhysicalDevice = device;
                VISUS_TRACE("Selected GPU: {}", physicalDeviceProperies.deviceName);
                break;
            }
        }

        if(m_PhysicalDevice == VK_NULL_HANDLE)
        {
            VISUS_TRACE("No discrete GPU was found.");
            m_PhysicalDevice = physicalDevices.front();
        }

        // Creating queue create infos
        FindQueueFamilies();

        VISUS_TRACE("Graphics queue index: {}", m_QueueFamilyIndices.graphics);
        VISUS_TRACE("Compute queue index: {}", m_QueueFamilyIndices.compute);
        VISUS_TRACE("Transfer queue index: {}", m_QueueFamilyIndices.transfer);

        {
            // Graphics queue
            VkDeviceQueueCreateInfo graphics_queue = {};
            graphics_queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            graphics_queue.queueFamilyIndex = m_QueueFamilyIndices.graphics;
            graphics_queue.queueCount = 1;
            graphics_queue.pQueuePriorities = &m_DefaultQueuePriority;
            m_QueueCreateInfos.push_back(graphics_queue);
        }
        {
            // Compute queue
            VkDeviceQueueCreateInfo compute_queue = {};
            compute_queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            compute_queue.queueFamilyIndex = m_QueueFamilyIndices.compute;
            compute_queue.queueCount = 1;
            compute_queue.pQueuePriorities = &m_DefaultQueuePriority;
            m_QueueCreateInfos.push_back(compute_queue);
        }

    }

    VulkanPhysicalDevice::~VulkanPhysicalDevice()
    {
        
    }

    bool VulkanPhysicalDevice::IsExtensionSupported(std::string ext) const
    {
        uint32_t count;
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> supportedExtensions(count);
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &count, supportedExtensions.data());

        for(auto& supportedExt : supportedExtensions)
        {
            if (strcmp(supportedExt.extensionName, ext.c_str())) return true;
        }
        return false;
    }

    void VulkanPhysicalDevice::FindQueueFamilies()
    {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_family_properties(count);
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, queue_family_properties.data());

        uint32_t index = 0;
        for (const auto& family : queue_family_properties) {
            if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                m_QueueFamilyIndices.graphics = index;
            }
            else if (family.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                m_QueueFamilyIndices.compute = index;
            }
            else if (family.queueFlags & VK_QUEUE_TRANSFER_BIT) {
                m_QueueFamilyIndices.transfer = index;
            }
            index++;
        }
    }

    // Vulkan Device
    // =============

    VulkanDevice::VulkanDevice(Ref<VulkanPhysicalDevice> physicalDevice, VkPhysicalDeviceFeatures enabledFeatures)
    {
        m_PhysicalDevice = physicalDevice;

        std::vector<const char*> enabledExtensions;
        // Physical device was previously checked on surface compatibility
        // therefore, selected device will 100% support surface and swapchain,
        // so no extension support checking needed
        enabledExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        enabledExtensions.push_back(VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME);
        enabledExtensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);

        VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature_enable_struct = {};
        dynamic_rendering_feature_enable_struct.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
        dynamic_rendering_feature_enable_struct.dynamicRendering = VK_TRUE;

        VkPhysicalDeviceIndexTypeUint8FeaturesEXT uint8_index_feature_enable_struct = {};
        uint8_index_feature_enable_struct.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT;
        uint8_index_feature_enable_struct.indexTypeUint8 = VK_TRUE;
        uint8_index_feature_enable_struct.pNext = &dynamic_rendering_feature_enable_struct;

        VkDeviceCreateInfo device_create_info = {};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pNext = &uint8_index_feature_enable_struct;
        device_create_info.enabledExtensionCount = enabledExtensions.size();
        device_create_info.ppEnabledExtensionNames = enabledExtensions.data();
        device_create_info.queueCreateInfoCount = m_PhysicalDevice->m_QueueCreateInfos.size();
        device_create_info.pQueueCreateInfos = m_PhysicalDevice->m_QueueCreateInfos.data();
        // Since all device layers are considered as deprecated and became instance layers, we will not use them.
        device_create_info.pEnabledFeatures = &enabledFeatures;

        VK_CHECK_RESULT(vkCreateDevice(m_PhysicalDevice->GetHandle(), &device_create_info, nullptr, &m_Device));

        vkGetDeviceQueue(m_Device, m_PhysicalDevice->GetQueueFamilyIndices().graphics, 0, &m_GraphicsTransferQueue);
        vkGetDeviceQueue(m_Device, m_PhysicalDevice->GetQueueFamilyIndices().compute, 0, &m_ComputeQueue);

        // Creating default cmd pool and allocating single cmd buffer.
        VkCommandPoolCreateInfo cmd_pool_create_info = {};
        cmd_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmd_pool_create_info.queueFamilyIndex = m_PhysicalDevice->GetQueueFamilyIndices().graphics;
        
        VK_CHECK_RESULT(vkCreateCommandPool(m_Device, &cmd_pool_create_info, nullptr, &m_DefaultCmdPool));

        VkCommandBufferAllocateInfo cmd_buffer_allocate_info = {};
        cmd_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmd_buffer_allocate_info.commandPool = m_DefaultCmdPool;
        cmd_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmd_buffer_allocate_info.commandBufferCount = 1;

        VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device, &cmd_buffer_allocate_info, &m_CommandBuffer));

    }

    VulkanDevice::~VulkanDevice()
    {
    }

    void VulkanDevice::Destroy()
    {
        vkDeviceWaitIdle(m_Device);
        vkDestroyCommandPool(m_Device, m_DefaultCmdPool, nullptr);
        vkDestroyDevice(m_Device, nullptr);
        VISUS_TRACE("Logical device destroyed");
    }
}
