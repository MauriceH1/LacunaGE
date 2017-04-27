message(STATUS "########## LacunaDX12Shaders ##########")

file(GLOB_RECURSE LCN_DX12_FILES relative "*.hlsl" "*.cpp")

foreach(SRC_FILE ${LCN_DX12_FILES})
  # get the path of the file relative to ${DIRECTORY}
  file(RELATIVE_PATH SRC_GROUP "${CMAKE_SOURCE_DIR}" ${SRC_FILE})
  string(REGEX REPLACE "src/LacunaDX12Shaders/" "" SRC_GROUP ${SRC_GROUP})
  # Extract the folder, ie remove the filename part
  string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRC_GROUP ${SRC_GROUP})
  # Source_group expects \\ (double antislash), not / (slash)
  string(REPLACE / \\\\ SRC_GROUP ${SRC_GROUP})
  source_group(${SRC_GROUP} FILES ${SRC_FILE})
  message(STATUS "  ${SRC_GROUP}\\ File::${SRC_FILE}")
endforeach()

set(LCN_DX12_SHADERS ${LCN_DX12_SHADERS} ${LCN_DX12_FILES})

set(COPYCOMMAND robocopy ${CMAKE_CURRENT_LIST_DIR}/ ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/shaders/ /S /XF CMakeLists.txt /IS \nSet errorlevel=0)
message(STATUS ${COPYCOMMAND})
add_custom_target(LacunaDX12Shaders SOURCES ${LCN_DX12_SHADERS} COMMAND ${COPYCOMMAND})
set_source_files_properties(${LCN_DX12_SHADERS} PROPERTIES VS_TOOL_OVERRIDE none)