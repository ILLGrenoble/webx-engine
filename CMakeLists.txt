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

file(GLOB_RECURSE SOURCES src/*.cpp  src/*/*.cpp lib/*.c lib/*.cpp)


find_package(PkgConfig)
if (NOT PKG_CONFIG_FOUND)
    message(FATAL_ERROR "pkg-config needed, install it please!")
endif ()

find_package(Threads REQUIRED)

pkg_check_modules(LIBPNG libpng)
pkg_check_modules(WEBP REQUIRED libwebp )

if (APPLE)
    include_directories("/usr/local/include" "/usr/X11/include" src lib)
    link_directories("/usr/local/lib" "/usr/X11/lib")
else()
    pkg_check_modules(X11 REQUIRED x11)
    include_directories(src lib)
endif()


add_executable(${PROJECT_NAME} ${SOURCES})

if (APPLE)
    target_link_libraries(
        ${PROJECT_NAME} 
        ${LIBPNG_LIBRARIES}
        -ljpeg
        -lwebp
        ${CMAKE_THREAD_LIBS_INIT}
        -lzmq
        -lX11
        -lXdamage
        -lXtst
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
        -ljpeg
        -lwebp
        ${X11_LIBRARIES}
        -lXdamage
        ${CMAKE_THREAD_LIBS_INIT}
        -lzmq
        -lXtst
        -lXfixes
    )

    target_include_directories(
        ${PROJECT_NAME} 
        PUBLIC
        ${LIBPNG_INCLUDEDIR}
        ${ZeroMQ_INCLUDE_DIR}
    )
endif()

file(GLOB_RECURSE TEST_IMAGE_SOURCES test/testImageEncoder.cpp src/image/*.cpp src/utils/*.cpp lib/*.cpp)
add_executable(testImageEncoder ${TEST_IMAGE_SOURCES})

target_link_libraries(
    testImageEncoder
    ${LIBPNG_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    -ljpeg
    -lpng
    -lwebp
    -lXtst
    -lX11
)



file(GLOB_RECURSE TEST_MOUSE_SOURCES test/testMouse.cpp)

add_executable(testMouse ${TEST_MOUSE_SOURCES})

target_link_libraries(
    testMouse
    -lXtst
    -lX11
)

file(GLOB_RECURSE TEST_CURSOR_IMAGE_ENCODER_SOURCES test/testCursorImageEncoder.cpp src/input/* src/utils/* src/image/*)

add_executable(testCursorImageEncoder ${TEST_CURSOR_IMAGE_ENCODER_SOURCES})

target_link_libraries(
    testCursorImageEncoder
    -lXtst
    -lX11
    -lXfixes
    -ljpeg
    -lwebp
    -lpng
)

