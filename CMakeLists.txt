cmake_minimum_required(VERSION 3.5)

project(webx-engine)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -pg")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
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
pkg_check_modules(WEBP REQUIRED libwebp)
pkg_check_modules(X11 REQUIRED x11)

include_directories(src lib)


add_executable(${PROJECT_NAME} ${SOURCES})

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
    -lxkbfile
    -lXfixes
)

target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC
    ${LIBPNG_INCLUDEDIR}
    ${ZeroMQ_INCLUDE_DIR}
)

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

file(GLOB_RECURSE TEST_ALPHA_SOURCES test/testAlpha.cpp src/image/*.cpp src/utils/*.cpp lib/*.cpp)
add_executable(testAlpha ${TEST_ALPHA_SOURCES})
target_link_libraries(
    testAlpha
    ${LIBPNG_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    -ljpeg
    -lwebp
)

file(GLOB_RECURSE TEST_CURSOR_IMAGE_ENCODER_SOURCES test/testCursorImageEncoder.cpp src/input/* src/utils/* src/image/* lib/crc32/*)
add_executable(testCursorImageEncoder ${TEST_CURSOR_IMAGE_ENCODER_SOURCES})
target_link_libraries(
    testCursorImageEncoder
    -lXtst
    -lX11
    -lXfixes
    -lxkbfile
    -ljpeg
    -lwebp
    -lpng
)

file(GLOB_RECURSE TEST_IMAGE_COMPARE_SOURCES test/testImageCompare.cpp)
add_executable(testImageCompare ${TEST_IMAGE_COMPARE_SOURCES})
target_link_libraries(
    testImageCompare
)

file(GLOB_RECURSE TEST_ALPHA_CHECK_SOURCES test/testAlphaCheck.cpp)
add_executable(testAlphaCheck ${TEST_ALPHA_CHECK_SOURCES})
target_link_libraries(
    testAlphaCheck
)

file(GLOB_RECURSE TEST_ALPHA_CREATE_SOURCES test/testAlphaCreate.cpp)
add_executable(testAlphaCreate ${TEST_ALPHA_CREATE_SOURCES})
target_link_libraries(
    testAlphaCreate
)

install(TARGETS ${PROJECT_NAME} DESTINATION "/usr/bin")

SET(CPACK_GENERATOR "DEB")
set(CPACK_CMAKE_GENERATOR Ninja)
set(CPACK_PACKAGE_VERSION "0.0.0-dev")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Stuart Caunt <caunt@ill.fr>, Jamie Hall <hall@ill.fr>")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The WebX Engine hooks on to an X11 display and provides a protocol to obtain X11 windows, receive display events and forward mouse and keyboard events.")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_DEBIAN_PACKAGE_SECTION "net")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
SET(CPACK_OUTPUT_FILE_PREFIX packages)
INCLUDE(CPack)
