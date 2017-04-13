#writer by shuntan@Tencent.com on 2017-02-28

link_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(configure_test configure_test.cpp)
target_link_libraries(configure_test ${COMMON_DEPEND_LIB})  
