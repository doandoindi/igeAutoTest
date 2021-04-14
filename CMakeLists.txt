cmake_minimum_required(VERSION 3.15.0)

if (NOT DEFINED APP_STYLE)
    set(APP_STYLE STATIC)
endif()

if(${APP_STYLE} MATCHES "SHARED")
    set(TARGET_NAME igeAutoTest)
    if (NOT DEFINED PYTHON_VERSION)
        set(PYTHON_VERSION 3.7)
    endif()
else()
    set(TARGET_NAME igeAutoTest-static)
endif()

project(${TARGET_NAME})

set (CMAKE_CXX_STANDARD 17)

if(DEFINED ANDROID_PLATFORM)
    set (ANDROID 1)
    if(ANDROID_ABI STREQUAL "armeabi-v7a")
        add_compile_options(-marm -march=armv7-a -mtune=cortex-a15 -mfpu=neon-fp16 -mfloat-abi=softfp)
    elseif(ANDROID_ABI STREQUAL "x86")
        add_compile_options(-march=atom -m32 -msse -msse2 -msse3 -mssse3 -mfpmath=sse -mstackrealign)
    elseif(ANDROID_ABI STREQUAL "arm64-v8a")
        add_compile_options(-march=armv8-a+simd+sve -mtune=cortex-a53 -D__ARM_NEON -D__ARM_NEON__)
    elseif(ANDROID_ABI STREQUAL "x86_64")
        add_compile_options(-march=atom -m64 -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mfpmath=sse)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-g0 -Ofast)
    else(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-Og)
    endif()
elseif(MSVC)
    if(${APP_STYLE} MATCHES "SHARED")
        set(CMAKE_CXX_FLAGS "/MD ${CMAKE_CXX_FLAGS}")
        set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /Zc:inline")
        set(CMAKE_CXX_FLAGS_DEBUG "/MD /JMC  /ZI /Od")
    endif()
    add_compile_options(/MP /std:c++17)
elseif(IOS)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-g0 -Ofast)
    endif()
endif()

if(${APP_STYLE} MATCHES "SHARED")
    find_package (Python3 ${PYTHON_VERSION} EXACT REQUIRED COMPONENTS Development)
else()
    set (Python3_INCLUDE_DIRS "")
endif()

# [IGE]: try include conan build config
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

    # Do not use python from conan for shared
    if(${APP_STYLE} MATCHES "SHARED")
        set(CONAN_INCLUDE_DIRS_PYTHON "")
        set(CONAN_LIBS_PYTHON "")
        set(CONAN_PKG_LIBS_PYTHON "")
    endif()

    conan_basic_setup()

    list(GET CONAN_INCLUDE_DIRS_PYXCORE 0 PYXCORE_INCLUDE_DIRS)
endif()
# [/IGE]

file(GLOB SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

set(INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${Python3_INCLUDE_DIRS}"
    "${CONAN_INCLUDE_DIRS}"
)

if(APPLE)
    if(IOS)
        set(INCLUDES ${INCLUDES} "ios")
        file(GLOB EXTRA_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/ios/*.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/ios/*.mm"
            "${CMAKE_CURRENT_SOURCE_DIR}/ios/*.m"
        )
        set(SRC ${SRC} ${EXTRA_SOURCES})
    elseif(OSX)
        set(INCLUDES ${INCLUDES} "macos")
        file(GLOB EXTRA_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/macos/*.mm"
            "${CMAKE_CURRENT_SOURCE_DIR}/macos/*.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/macos/*.cpp"
        )
        set(SRC ${SRC} ${EXTRA_SOURCES})
    endif()
elseif(ANDROID)
    set(INCLUDES ${INCLUDES} "android")
    file(GLOB EXTRA_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/android/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/android/*.cpp"
    )
    set(SRC ${SRC} ${EXTRA_SOURCES})
elseif(MSVC)
    set(INCLUDES 
        ${INCLUDES}
        "win32"
        "${PYXCORE_INCLUDE_DIRS}/ThirdParty/GLEW"
    )
    file(GLOB EXTRA_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/win32/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/win32/*.cpp"
    )
    set(SRC ${SRC} ${EXTRA_SOURCES})
endif()

add_library(${TARGET_NAME} ${APP_STYLE} ${SRC})

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDES})

target_compile_definitions(${TARGET_NAME} PUBLIC ${CONAN_DEFINES})

if(${APP_STYLE} MATCHES "SHARED")
    set_target_properties(
        ${TARGET_NAME}
        PROPERTIES
            PREFIX ""
            OUTPUT_NAME ${TARGET_NAME}
            LINKER_LANGUAGE C
    )

    if(MSVC)
        set_target_properties(
            ${TARGET_NAME}
            PROPERTIES
            SUFFIX ".pyd"
        )
    endif()

    target_compile_definitions(${TARGET_NAME} PUBLIC BUILD_EXTENSION=1)

    target_link_libraries(${TARGET_NAME} ${Python3_LIBRARIES} opengl32 winmm ${CONAN_LIBS}
)
endif()

