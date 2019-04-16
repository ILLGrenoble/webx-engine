cmake_minimum_required(VERSION 3.1)

project(webx)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -pg")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 11)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE SOURCES src/*.cpp lib/*.c lib/*.cpp)


find_package(PkgConfig)
find_package(Threads REQUIRED)
find_package(PkgConfig)

pkg_check_modules(LIBPNG libpng)

if (APPLE)
    include_directories("/usr/local/include" "/usr/X11/include" src lib)
    link_directories("/usr/local/lib" "/usr/X11/lib")

else()
    pkg_check_modules(X11 REQUIRED x11)
    pkg_check_modules(PC_ZeroMQ zmq)
    include_directories(src lib)

    find_path(ZeroMQ_INCLUDE_DIR
            NAMES zmq.hpp
            PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
            )

    find_library(ZeroMQ_LIBRARY
            NAMES zmq
            PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
            )
endif()


add_executable(${PROJECT_NAME} ${SOURCES})

if (APPLE)
    target_link_libraries(
        ${PROJECT_NAME} 
        ${LIBPNG_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        -lzmq
        -lX11
        -lXdamage
    )

    target_include_directories(
        ${PROJECT_NAME} 
        PUBLIC
        ${LIBPNG_INCLUDEDIR}
    )

else()
    target_link_libraries(
        ${PROJECT_NAME}
        ${LIBPNG_LIBRARIES}
        ${X11_LIBRARIES}
        -lXdamage
        ${CMAKE_THREAD_LIBS_INIT}
        ${ZeroMQ_LIBRARY}
    )

    target_include_directories(
        ${PROJECT_NAME} 
        PUBLIC
        ${LIBPNG_INCLUDEDIR}
        ${ZeroMQ_INCLUDE_DIR}
    )
endif()

file(GLOB_RECURSE TEST_SOURCES test/testImageEncoder.cpp src/image/*.cpp src/utils/*.cpp lib/*.cpp)
add_executable(testImageEncoder ${TEST_SOURCES})

target_link_libraries(
    testImageEncoder
    ${LIBPNG_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

