cmake_minimum_required(VERSION 3.7)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
#add_definitions(-D_UNICODE -DUNICODE)

set(LCN_ASSIMP_INCLUDES ${LCN_ASSIMP_INCLUDES} "${CMAKE_SOURCE_DIR}/thirdparty/assimp-3.3.1/include/")
set(LCN_STB_INCLUDES ${LCN_STB_INCLUDES} "${CMAKE_SOURCE_DIR}/thirdparty/stb")
set(LCN_GLM_INCLUDES ${LCN_GLM_INCLUDES} "${CMAKE_SOURCE_DIR}/thirdparty/glm-0.9.8.4/")
set(LCN_MC_INCLUDES ${LCN_MC_INCLUDES} "${CMAKE_SOURCE_DIR}/thirdparty/moodycamel/")

project(LacunaGE)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(LCN_BUILDNAME "x64")
else()
	message(FATAL_ERROR "32-bit compilers are not supported, please select a 64-bit compiler instead.")
endif()

message(STATUS "64-bit compiler found! Moving on...")

# set output folders, shared for each project and remain unchanged
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/int/${CMAKE_CFG_INTDIR}/Lacuna)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/int/${CMAKE_CFG_INTDIR}/Lacuna)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/int/${CMAKE_CFG_INTDIR}/Lacuna)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/int/${CMAKE_CFG_INTDIR}/Lacuna)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/${CMAKE_CFG_INTDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/${CMAKE_CFG_INTDIR})

# set compiler specific flags (eg: optimizations, shared by all projects)
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	add_compile_options("/MP")
ENDIF()

add_subdirectory("thirdparty/assimp-3.3.1")
#add_subdirectory("thirdparty/glm-0.9.8.4")

set_target_properties(assimp PROPERTIES FOLDER "Thirdparty")
# set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "Thirdparty")
set_target_properties(zlibstatic PROPERTIES FOLDER "Thirdparty")
set_target_properties(uninstall PROPERTIES FOLDER "Thirdparty")

add_compile_options("-std:c++latest")

add_subdirectory(assets)
add_subdirectory(src)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LacunaCore)