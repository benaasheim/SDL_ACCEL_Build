cmake_minimum_required(VERSION 2.8)
project(ray_tracer)


if(${src} MATCHES "/")  
        set(src "src/${src}")
        message("src = ${src} ")
endif(${src} matches "/")


# use FindSDL2.cmake
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# enable C++11
set(CMAKE_CXX_FLAGS "gcc-10 -O3 -ftree-vectorize -fopt-info-vec -march=native -falign-functions=32 -falign-loops=32 ${CMAKE_CXX_FLAGS}")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(
        "${src}"
        ${SDL2_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIR}
)

set(SOURCE_FILES
        "${src}/main.c"
)

add_executable(ray_tracer ${SOURCE_FILES})

target_link_libraries(ray_tracer
        ${SDL2_LIBRARY}
        ${SDL2_IMAGE_LIBRARY}
)
