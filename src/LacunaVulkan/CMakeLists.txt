message(STATUS "########## LacunaVulkan ##########")

file(GLOB_RECURSE LCN_VULKAN_FILES relative
	"source/*.cpp"
	"source/*.hpp"
	"source/*.c"
	"source/*.h"
	"include/*.hpp"
	"include/*.h"
)

foreach(SRC_FILE ${LCN_VULKAN_FILES})
  # get the path of the file relative to ${DIRECTORY}
  file(RELATIVE_PATH SRC_GROUP "${CMAKE_SOURCE_DIR}" ${SRC_FILE})
  string(REGEX REPLACE "src/LacunaDX12/" "" SRC_GROUP ${SRC_GROUP})
  # Extract the folder, ie remove the filename part
  string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRC_GROUP ${SRC_GROUP})
  # Source_group expects \\ (double antislash), not / (slash)
  string(REPLACE / \\\\ SRC_GROUP ${SRC_GROUP})
  source_group(${SRC_GROUP} FILES ${SRC_FILE})
endforeach()

set(LCN_VULKAN_SOURCES ${LCN_VULKAN_SOURCES} ${LCN_VULKAN_FILES})

include_directories("include")
include_directories("${Vulkan_INCLUDE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/src/LacunaCore/includes")
include_directories("${CMAKE_SOURCE_DIR}/src/LacunaCore/interfaces")

include_directories(SYSTEM "${LCN_GLM_INCLUDES}")
include_directories(SYSTEM "${LCN_ASSIMP_INCLUDES}")

link_directories("${Vulkan_LIBRARY}")

add_library(LacunaVulkan ${LCN_VULKAN_SOURCES})

target_link_libraries(LacunaVulkan ${Vulkan_LIBRARIES})