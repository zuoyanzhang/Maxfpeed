cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(ErrorDetection VERSION 1.0)

find_package(Python3 COMPONENTS Development)

include_directories(/opt/homebrew/opt/gmp/include)
include_directories(/opt/homebrew/opt/mpfr/include)
include_directories(${Python3_INCLUDE_DIRS}) 

link_directories(/opt/homebrew/opt/gmp/lib)
link_directories(/opt/homebrew/opt/mpfr/lib)

# add the MathFunctions library
add_subdirectory(detectModule)

# add the executable
add_executable(${PROJECT_NAME} maxFpeedMain/main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    detectModule 
    m 
    mpfr 
    gmp 
    ${Python3_LIBRARIES} 
)

# add the search path for include files
target_include_directories(detectModule PUBLIC 
    ${PROJECT_SOURCE_DIR}/detectModule
    ${Python3_INCLUDE_DIRS} 
    /opt/homebrew/opt/gmp/include   
    /opt/homebrew/opt/mpfr/include 
)

