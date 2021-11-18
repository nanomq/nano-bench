#
# This software is supplied under the terms of the MIT License, a
# copy of which should be located in the distribution where this
# file was obtained (LICENSE.txt).  A copy of the license may also be
# found online at https://opensource.org/licenses/MIT.

cmake_minimum_required (VERSION 2.8.7)

project(nano_bench)

find_package(nng CONFIG REQUIRED)

# include_directories(/home/lee/workspace/nng/include)
# set(CMAKE_PREFIX_PATH "/home/lee/workspace/nng/build/")
# find_library(NNG_LIBRIES NAMES nng PATHS /home/lee/workspace/nng/build/)
# if (NNG_LIBRIES)  
# 	message(STATUS "${NNG_LIBRIES} library found")
# else (NNG_LIBRIES)
# 	message(FATAL_ERROR "nng library not found")
# endif(NNG_LIBRIES)

find_package(Threads)

add_executable(nano_bench mqtt_async.c nnb_opt.c)
# target_link_libraries(nano_bench ${NNG_LIBRIES})
target_link_libraries(nano_bench nng)
target_link_libraries(nano_bench ${CMAKE_THREAD_LIBS_INIT})
