cmake_minimum_required(VERSION 3.13)
project(ys2)

set(CMAKE_CXX_STANDARD 20)
link_libraries(dl gmp gmpxx)

add_executable(ys2 main.cpp value.hpp value.cpp frame.hpp frame.cpp code_feed.hpp operators.hpp operators.cpp operators/raw_data.hpp operators/ctl_flow.hpp operators/raw_data.cpp operators/ctl_flow.cpp operators/stack_ctl.hpp operators/stack_ctl.cpp operators/ref_ops.cpp operators/ref_ops.hpp operators/types.hpp operators/types.cpp operators/boolean_ops.hpp operators/boolean_ops.cpp operators/io.hpp operators/io.cpp)