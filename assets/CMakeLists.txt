message(STATUS "########## LacunaAssets ##########")

set(COPYCOMMAND robocopy ${CMAKE_CURRENT_LIST_DIR}/ ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/assets/ /S /XF CMakeLists.txt /IS \nSet errorlevel=0)
message(STATUS ${COPYCOMMAND})
add_custom_target(LacunaAssets COMMAND ${COPYCOMMAND})