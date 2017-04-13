# Writed by shuntan on 2017/4/13

cmake_minimum_required(VERSION 2.8.11)
project(zookeeper_helper)

# 当前目录和其父目录加入到include搜索中
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

# 设置zookeeper官方库安装目录
set(ZOOKEEPER_HOME /usr/local/thirdparty/zookeeper)

# 编译参数
# 启用__STDC_FORMAT_MACROS是为了可以使用inttypes.h中的PRId64等
# 启用__STDC_LIMIT_MACROS是为了可以使用stdint.h中的__UINT64_C和INT32_MIN等
add_definitions("-Wall -fPIC -pthread -D_GNU_SOURCE -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -Wno-deprecated")

# 设置依赖库路径
#zookeeper库
include_directories(${ZOOKEEPER_HOME}/include)
link_directories(${ZOOKEEPER_HOME}/lib)

# 链接需要的系统库
link_libraries(dl pthread rt z)

link_directories(cpp)
set(COMMON_DEPEND_LIB libzookeeper.a libzookeeper_mt.a)

add_subdirectory(cpp)
add_subdirectory(examples)
add_subdirectory(tests)
