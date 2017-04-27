message(STATUS "########## LacunaDX12 ##########")

file(GLOB_RECURSE LCN_DX12_FILES relative
	"source/*.cpp"
	"source/*.hpp"
	"source/*.c"
	"source/*.h"
	"include/*.hpp"
	"include/*.h"
)

foreach(SRC_FILE ${LCN_DX12_FILES})
  # get the path of the file relative to ${DIRECTORY}
  file(RELATIVE_PATH SRC_GROUP "${CMAKE_SOURCE_DIR}" ${SRC_FILE})
  string(REGEX REPLACE "src/LacunaDX12/" "" SRC_GROUP ${SRC_GROUP})
  # Extract the folder, ie remove the filename part
  string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRC_GROUP ${SRC_GROUP})
  # Source_group expects \\ (double antislash), not / (slash)
  string(REPLACE / \\\\ SRC_GROUP ${SRC_GROUP})
  source_group(${SRC_GROUP} FILES ${SRC_FILE})
  message(STATUS "  ${SRC_GROUP}\\ File::${SRC_FILE}")
endforeach()

set(LCN_DX12_SOURCES ${LCN_DX12_SOURCES} ${LCN_DX12_FILES})

include_directories("include")
include_directories("${CMAKE_SOURCE_DIR}/src/LacunaCore/includes")
include_directories("${CMAKE_SOURCE_DIR}/src/LacunaCore/interfaces")
include_directories("${CMAKE_SOURCE_DIR}/thirdparty/include")

include_directories(SYSTEM "${LCN_GLM_INCLUDES}")
include_directories(SYSTEM "${LCN_ASSIMP_INCLUDES}")

add_library(LacunaDX12 ${LCN_DX12_SOURCES})

target_link_libraries(LacunaDX12 "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")
add_dependencies(LacunaDX12 LacunaDX12Shaders)















# file(GLOB_RECURSE LCN_DX12_SOURCES relative "source/*.cpp")
# file(GLOB_RECURSE LCN_DX12_INCLUDES relative "source/*.hpp" "source/*.h"
# 										"include/*.hpp" "include/*.h")
# file(GLOB_RECURSE LCN_DX12_SHADERS relative "shaders/*.hlsl")
# 
# set(LCN_DX12_FILES ${LCN_DX12_FILES} ${LCN_DX12_SOURCES})
# set(LCN_DX12_FILES ${LCN_DX12_FILES} ${LCN_DX12_INCLUDES})
# set(LCN_DX12_FILES ${LCN_DX12_FILES} ${LCN_DX12_SHADERS})
# 
# message(STATUS "Files:")
# foreach(SRCFILE ${LCN_DX12_FILES})
# 	message(STATUS "    ${SRCFILE}")
# endforeach()
# 
# include_directories("include")
# include_directories("${CMAKE_SOURCE_DIR}/src/LacunaCore/interfaces")
# include_directories("${CMAKE_SOURCE_DIR}/thirdparty/include")
# 
# add_library(LacunaDX12 ${LCN_DX12_FILES})
# 
# target_link_libraries(LacunaDX12 "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")