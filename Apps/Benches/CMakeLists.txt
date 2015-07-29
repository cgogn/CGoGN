cmake_minimum_required(VERSION 2.8)

project(benches)

add_executable(bench_trav bench_trav.cpp )
target_link_libraries( bench_trav ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

add_executable(bench_remesh bench_remesh.cpp )
target_link_libraries( bench_remesh ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )

add_executable(bench_compact bench_compact.cpp )
target_link_libraries( bench_compact ${CGoGN_LIBS} ${CGoGN_EXT_LIBS} )
