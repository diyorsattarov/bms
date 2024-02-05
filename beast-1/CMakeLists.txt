cmake_minimum_required(VERSION 3.12)
project(beast-1)

set(CMAKE_CXX_STANDARD 14)

# Find Boost components
find_package(Boost REQUIRED COMPONENTS system)
file(GLOB_RECURSE SOURCE_FILES
  "src/*.cpp"
  "src/application.cpp"
  "main.cpp")
# Add the executable
add_executable(beast-1 ${SOURCE_FILES})

target_link_libraries(beast-1 Boost::system pthread)
