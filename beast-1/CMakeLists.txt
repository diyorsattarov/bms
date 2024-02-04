cmake_minimum_required(VERSION 3.12)
project(http_server)

set(CMAKE_CXX_STANDARD 14)

# Find Boost components
find_package(Boost REQUIRED COMPONENTS system)
file(GLOB_RECURSE SOURCE_FILES
  "src/*.cpp"
  "src/application.cpp"
  "src/client/session.cpp"
  "main.cpp")
# Add the executable
add_executable(http_server ${SOURCE_FILES})

target_link_libraries(http_server Boost::system pthread)
