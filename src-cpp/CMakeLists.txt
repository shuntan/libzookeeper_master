#writer by shuntan@Tencent.com on 2017-02-28

# 生成 Zookeeper helper 库
link_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(ZOOKEEPER_SRC zookeeper_helper.cpp)
add_library(zookeeper STATIC ${ZOOKEEPER_SRC})

# CMAKE_INSTALL_PREFIX
install(
        TARGETS zookeeper
        DESTINATION lib
       )