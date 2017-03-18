#pragma once

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define VK_USE_PLATFORM_WIN32_KHR
	#define NOMINMAX
#elif defined(__ANDROID__)
	// Include files for Android
	#include <unistd.h>
	#include <android/log.h>
	#include "vulkan_wrapper.h" // Include Vulkan_wrapper and dynamically load symbols.
#else //__ANDROID__
	#include <unistd.h>
	#include "vulkan/vk_sdk_platform.h"
#endif // _WIN32

#include <vulkan\vulkan.hpp>

#ifdef _WIN32
namespace lcn {
	namespace platform {
		namespace specifics
		{
			struct PlatformHandles
			{
				HINSTANCE hInstance;
				HWND hWnd;
			};
		};
	};
}; // lcn::platform::specifics
#endif

namespace lcn {
	namespace graphics {
		namespace helpers
		{
			typedef struct
			{
				VkLayerProperties properties;
				std::vector<VkExtensionProperties> extensions;
			} layer_properties;

			struct VkData
			{
				VkInstance instance;
				VkApplicationInfo app_info = {
					VK_STRUCTURE_TYPE_APPLICATION_INFO,
					nullptr,
					"LacunaGE",
					0,
					nullptr,
					0,
					VK_API_VERSION_1_0
				};
				VkInstanceCreateInfo instance_create_info = {
					VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
					nullptr,
					0,
					nullptr,
					0,
					nullptr,
					0,
					nullptr
				};
				VkDeviceCreateInfo device_info = {
					VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
					nullptr,
					0,
					1,
					nullptr,
					0,
					nullptr,
					0,
					nullptr,
					nullptr
				};
				std::vector<VkQueueFamilyProperties> queue_props;
				std::vector<VkPhysicalDevice> gpus;
				VkDevice device;

				std::vector<layer_properties> instance_layer_properties;
				std::vector<const char*> extension_names;

				VkCommandPool graphics_cmd_pool;
				VkCommandBuffer graphics_cmd_buf;

				uint32_t queue_family_count;
			};

			void init_instance_extension_names(struct VkData& info);
			VkResult init_global_layer_properties(struct VkData& info);
			VkResult init_global_extension_properties(layer_properties &layer_props);
		};
	};
}; // lcn::graphics::helpers