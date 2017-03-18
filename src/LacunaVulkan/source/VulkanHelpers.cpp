#pragma once

#include "VulkanHelpers.h"

namespace lcn {
	namespace graphics {
		namespace helpers
		{
			void init_instance_extension_names(struct VkData& info)
			{
				info.extension_names.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef __ANDROID__
				info.extension_names.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(_WIN32)
				info.extension_names.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
				info.extension_names.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
			}

			VkResult init_global_layer_properties(struct VkData& info)
			{
				uint32_t instance_layer_count;
				VkLayerProperties *vk_props = NULL;
				VkResult res;
#ifdef __ANDROID__
				// This place is the first place for samples to use Vulkan APIs.
				// Here, we are going to open Vulkan.so on the device and retrieve function pointers using
				// vulkan_wrapper helper.
				if (!InitVulkan())
				{
					LOGE("Failied initializing Vulkan APIs!");
					return VK_ERROR_INITIALIZATION_FAILED;
				}
				LOGI("Loaded Vulkan APIs.");
#endif

				/*
				* It's possible, though very rare, that the number of
				* instance layers could change. For example, installing something
				* could include new layers that the loader would pick up
				* between the initial query for the count and the
				* request for VkLayerProperties. The loader indicates that
				* by returning a VK_INCOMPLETE status and will update the
				* the count parameter.
				* The count parameter will be updated with the number of
				* entries loaded into the data pointer - in case the number
				* of layers went down or is smaller than the size given.
				*/
				do
				{
					res = vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
					if (res)
						return res;

					if (instance_layer_count == 0)
					{
						return VK_SUCCESS;
					}

					vk_props = (VkLayerProperties *)realloc(
						vk_props, instance_layer_count * sizeof(VkLayerProperties));

					res =
						vkEnumerateInstanceLayerProperties(&instance_layer_count, vk_props);
				} while (res == VK_INCOMPLETE);

				/*
				* Now gather the extension list for each instance layer.
				*/
				for (uint32_t i = 0; i < instance_layer_count; i++)
				{
					layer_properties layer_props;
					layer_props.properties = vk_props[i];
					res = init_global_extension_properties(layer_props);
					if (res)
						return res;
					info.instance_layer_properties.push_back(layer_props);
				}
				free(vk_props);

				return res;
			}

			VkResult init_global_extension_properties(layer_properties &layer_props)
			{
				VkExtensionProperties *instance_extensions;
				uint32_t instance_extension_count;
				VkResult res;
				char *layer_name = NULL;

				layer_name = layer_props.properties.layerName;

				do
				{
					res = vkEnumerateInstanceExtensionProperties(
						layer_name, &instance_extension_count, NULL);
					if (res)
						return res;

					if (instance_extension_count == 0)
					{
						return VK_SUCCESS;
					}

					layer_props.extensions.resize(instance_extension_count);
					instance_extensions = layer_props.extensions.data();
					res = vkEnumerateInstanceExtensionProperties(
						layer_name, &instance_extension_count, instance_extensions);
				} while (res == VK_INCOMPLETE);

				return res;
			}
		};
	};
};