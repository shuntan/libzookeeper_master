# Writed by shuntan on 2017/4/13

cmake_minimum_required(VERSION 2.8.11)
project(zookeeper_helper)

# 让make时显示编译命令
set(CMAKE_VERBOSE_MAKEFILE OFF)

# 定义颜色值，message()时可用到
if (NOT WIN32)
    string(ASCII 27 Esc)
    set(ColourReset "${Esc}[m")
    set(ColourBold  "${Esc}[1m")
    set(Red         "${Esc}[31m")
    set(Green       "${Esc}[32m")
    set(Yellow      "${Esc}[33m")
    set(Blue        "${Esc}[34m")
    set(Magenta     "${Esc}[35m")
    set(Cyan        "${Esc}[36m")
    set(White       "${Esc}[37m")
    set(BoldRed     "${Esc}[1;31m")
    set(BoldGreen   "${Esc}[1;32m")
    set(BoldYellow  "${Esc}[1;33m")
    set(BoldBlue    "${Esc}[1;34m")
    set(BoldMagenta "${Esc}[1;35m")
    set(BoldCyan    "${Esc}[1;36m")
    set(BoldWhite   "${Esc}[1;37m")
endif ()

# 编译参数
# 启用__STDC_FORMAT_MACROS是为了可以使用inttypes.h中的PRId64等
# 启用__STDC_LIMIT_MACROS是为了可以使用stdint.h中的__UINT64_C和INT32_MIN等
add_definitions("-Wall -fPIC -pthread -D_GNU_SOURCE -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -Wno-deprecated")

# 当前目录和其父目录加入到include搜索中
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

# 第三方库存放位置
# 建议所用到的第三方库放到THIRDPARTY_HOME指定的目录
if (EXISTS /usr/local/thirdparty)
    set(THIRDPARTY_HOME /usr/local/thirdparty)
else ()
    set(THIRDPARTY_HOME /usr/local)
endif ()

# 检查THIRDPARTY_HOME指定的目录是否存在
if (NOT EXISTS ${THIRDPARTY_HOME})
    message("${Yellow}directory[${THIRDPARTY_HOME}] not exist${ColourReset}")
    return ()
endif ()

# 检查zookeeper官方库安装目录
if (EXISTS ${THIRDPARTY_HOME}/zookeeper/lib/libzookeeper_mt.a)    
    add_definitions("-DHAVE_ZOOKEEPER=1")
    message("${Red}ZOOKEEPER found${ColourReset}")
else ()    
    message("${Yellow}ZOOKEEPER not found${ColourReset}")
endif ()

# 设置zookeeper官方库安装目录
set(ZOOKEEPER_HOME ${THIRDPARTY_HOME}/zookeeper)

# 设置依赖库路径
#zookeeper库
include_directories(${ZOOKEEPER_HOME}/include)
link_directories(${ZOOKEEPER_HOME}/lib)

# 链接需要的系统库
link_libraries(dl pthread rt z)

link_directories(src-cpp)
set(COMMON_DEPEND_LIB libzookeeper.a libzookeeper_mt.a)

add_subdirectory(src-cpp)
add_subdirectory(examples)
add_subdirectory(tests)
