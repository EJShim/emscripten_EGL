cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_WEBGL2=1")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s FULL_ES3=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ERROR_ON_UNDEFINED_SYMBOLS=0")


find_package(VTK REQUIRED)
include(${VTK_USE_FILE})



add_executable(index main.cpp K_EGLRenderWindow.cpp) 

target_link_libraries(index ${VTK_LIBRARIES})