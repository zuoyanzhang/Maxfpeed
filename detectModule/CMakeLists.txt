file(GLOB_RECURSE srcs CONFIGURE_DEPENDS EIFFEL/src/*.cpp HSED/src/*.cpp)
add_library(detectModule ${srcs}
        EIFFEL/src/errordetect.cpp)