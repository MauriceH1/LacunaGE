add_subdirectory(LacunaPlatform)

message(STATUS "Trying to find Vulkan")
find_package(Vulkan)

IF(Vulkan_FOUND)
	message(STATUS "Vulkan found! Adding Vulkan renderer...")
	add_subdirectory(LacunaVulkan)
ENDIF()

IF(${CMAKE_SYSTEM_VERSION} VERSION_GREATER "10.0")
	message(STATUS "Windows 10 found! Adding DX12 renderer...")
	add_subdirectory(LacunaDX12Shaders)
	add_subdirectory(LacunaDX12)
ENDIF()

add_subdirectory(LacunaCore)