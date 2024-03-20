cmake_minimum_required(VERSION 3.16)

project(ErrorDetection VERSION 1.0)

# add the MathFunctions library
add_subdirectory(detectModule)

# add the executable
add_executable(${PROJECT_NAME} maxFpeedMain/main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC detectModule m mpfr python3.8)

# add the search path for include files
target_include_directories(detectModule PUBLIC ${PROJECT_SOURCE_DIR}/detectModule)
target_include_directories(detectModule PUBLIC /usr/include/python3.8)

