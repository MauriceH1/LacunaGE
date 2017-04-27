message(STATUS "########## LacunaCore ##########")

file(GLOB_RECURSE ALL_FILES relative "*.cpp" "*.hpp" "*.h" "*.c" "*.inl")

foreach(SRC_FILE ${ALL_FILES})
  # get the path of the file relative to ${DIRECTORY}
  file(RELATIVE_PATH SRC_GROUP "${CMAKE_SOURCE_DIR}" ${SRC_FILE})
  string(REGEX REPLACE "src/LacunaCore/" "" SRC_GROUP ${SRC_GROUP})
  # Extract the folder, ie remove the filename part
  string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRC_GROUP ${SRC_GROUP})
  # Source_group expects \\ (double antislash), not / (slash)
  string(REPLACE / \\\\ SRC_GROUP ${SRC_GROUP})
  source_group(${SRC_GROUP} FILES ${SRC_FILE})
endforeach()

set(LCN_SOURCE_FILES ${LCN_SOURCE_FILES} ${ALL_FILES})

add_executable(LacunaCore ${LCN_SOURCE_FILES})

include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/src/LacunaPlatform/include")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/src/LacunaCore/interfaces")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/src/LacunaCore/includes")

include_directories(SYSTEM "${LCN_GLM_INCLUDES}")
include_directories(SYSTEM "${LCN_ASSIMP_INCLUDES}")

target_link_libraries(LacunaCore LacunaPlatform)
target_link_libraries(LacunaCore assimp )

IF(Vulkan_FOUND)
	include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/src/LacunaVulkan/include")
	target_link_libraries(LacunaCore LacunaVulkan)
ENDIF()

IF(${CMAKE_SYSTEM_VERSION} VERSION_GREATER "10.0")
	include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/src/LacunaDX12/include")
	target_link_libraries(LacunaCore LacunaDX12)
ENDIF()

add_dependencies(LacunaCore LacunaAssets)





#file(GLOB_RECURSE LCN_CORE_FILES relative "*.cpp" "*.hpp" "*.h")
#
#message(STATUS "Files:")
#foreach(SRCFILE ${LCN_CORE_FILES})
#	message(STATUS "    ${SRCFILE}")
#endforeach()
#
##add include directories
#message(STATUS "Adding include directories")
#include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/src/LacunaPlatform/include")
#include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/src/LacunaCore/interfaces")
#include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/src/LacunaCore/includes")
#
##add library directories
#message(STATUS "Adding library directories")
#link_directories("${CMAKE_SOURCE_DIR}/lib/${CMAKE_CFG_INTDIR}/Lacuna")
#
#add_executable(LacunaCore ${LCN_CORE_FILES})
#
## target_link_libraries(LacunaCore debug "Assimp-d.lib")
## target_link_libraries(LacunaCore optimized "Assimp.lib")
#target_link_libraries(LacunaCore LacunaPlatform)
#target_link_libraries(LacunaCore assimp )
#
#IF(Vulkan_FOUND)
#	include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/src/LacunaVulkan/include")
#	target_link_libraries(LacunaCore LacunaVulkan)
#ENDIF()
#
#IF(${CMAKE_SYSTEM_VERSION} VERSION_GREATER "10.0")
#	include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/src/LacunaDX12/include")
#	target_link_libraries(LacunaCore LacunaDX12)
#ENDIF()