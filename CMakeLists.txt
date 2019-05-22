cmake_minimum_required(VERSION 3.13)
project(ys2)

set(CMAKE_CXX_STANDARD 20)
include_directories("/usr/local/include/")
link_libraries(dl gmp gmpxx)

add_executable(ys2 main.cpp value.hpp value.cpp frame.hpp frame.cpp code_feed.hpp operators.hpp operators.cpp operators/literals.hpp operators/ctl_flow.hpp operators/literals.cpp operators/ctl_flow.cpp operators/stack_ctl.hpp operators/stack_ctl.cpp operators/ref_ops.cpp operators/ref_ops.hpp operators/types.hpp operators/types.cpp operators/boolean_ops.hpp operators/boolean_ops.cpp operators/io.hpp operators/io.cpp extend.cpp extend.hpp operators/data_types.hpp operators/data_types.cpp operators/const.hpp operators/const.cpp operators/basic_math.hpp operators/basic_math.cpp operators/list_stuff.hpp operators/generic_operators.hpp operators/list_stuff.cpp lambda.cpp lambda.hpp object.cpp object.hpp operators/objects.hpp operators/objects.cpp operators/generic_operators.cpp operators/threads.hpp operators/threads.cpp macro.hpp operators/libraries.hpp operators/libraries.cpp code_feed.cpp operators/math.hpp operators/math.cpp operators/errors.hpp operators/errors.cpp)
