#writer by shuntan@Tencent.com on 2017-02-28

link_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(example example.cpp)
target_link_libraries(example ${COMMON_DEPEND_LIB})    

