cmake_minimum_required(VERSION 3.20)
project(v8_with_libuv)

set(CMAKE_CXX_STANDARD 14)

include_directories(deps/libuv/include)
include_directories(deps/v8/include)
include_directories(deps/v8/include/platform)
include_directories(deps/v8)

link_directories(deps/libuv/.libs/)
link_libraries(uv)
link_directories(deps/v8/out/x64.release/obj)
link_libraries(v8_monolith)

aux_source_directory(src DIR_SRCS)

add_executable(demo ${DIR_SRCS} src/common.h src/env.h)
