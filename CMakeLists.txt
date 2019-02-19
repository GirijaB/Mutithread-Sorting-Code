# CMakeList.txt : CMake project for ProducerConsumer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (ParallelSort "ParallelSort.cpp" "ParallelSort.h" "main.h" "main.cpp")
set_property(TARGET ParallelSort PROPERTY CXX_STANDARD 17)
set_property(TARGET ParallelSort PROPERTY CXX_STANDARD_REQUIRED ON)
# TODO: Add tests and install targets if needed.
