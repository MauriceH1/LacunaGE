message(STATUS "########## LacunaPlatform ##########")

file(GLOB_RECURSE LCN_PLATFORM_FILES relative
	"source/*.cpp"
	"source/*.hpp"
	"source/*.c"
	"source/*.h"
	"include/*.hpp"
	"include/*.h"
)

foreach(SRC_FILE ${LCN_PLATFORM_FILES})
  # get the path of the file relative to ${DIRECTORY}
  file(RELATIVE_PATH SRC_GROUP "${CMAKE_SOURCE_DIR}" ${SRC_FILE})
  string(REGEX REPLACE "src/LacunaPlatform/" "" SRC_GROUP ${SRC_GROUP})
  # Extract the folder, ie remove the filename part
  string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRC_GROUP ${SRC_GROUP})
  # Source_group expects \\ (double antislash), not / (slash)
  string(REPLACE / \\\\ SRC_GROUP ${SRC_GROUP})
  source_group(${SRC_GROUP} FILES ${SRC_FILE})
  message(STATUS "  ${SRC_GROUP}\\ File::${SRC_FILE}")
endforeach()

set(LCN_PLATFORM_SOURCES ${LCN_PLATFORM_SOURCES} ${LCN_PLATFORM_FILES})

include_directories("include")

include_directories(SYSTEM "${LCN_GLM_INCLUDES}")
include_directories(SYSTEM "${LCN_ASSIMP_INCLUDES}")

add_library(LacunaPlatform ${LCN_PLATFORM_SOURCES})

# file(GLOB_RECURSE LCN_PLATFORM_SOURCES relative "source/*.cpp")
# file(GLOB_RECURSE LCN_PLATFORM_INCLUDES relative "source/*.hpp" "source/*.h"
# 										"include/*.hpp" "include/*.h")
# 
# set(LCN_PLATFORM_FILES ${LCN_PLATFORM_FILES} ${LCN_PLATFORM_SOURCES})
# set(LCN_PLATFORM_FILES ${LCN_PLATFORM_FILES} ${LCN_PLATFORM_INCLUDES})
# 
# message(STATUS "Files:")
# foreach(SRCFILE ${LCN_PLATFORM_FILES})
# 	message(STATUS "    ${SRCFILE}")
# endforeach()
# 
# include_directories("include")
# include_directories("${CMAKE_SOURCE_DIR}/thirdparty/include")
# 
# add_library(LacunaPlatform ${LCN_PLATFORM_FILES})