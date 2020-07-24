cmake_minimum_required(VERSION 3.4.1)

file(TO_CMAKE_PATH $ENV{IGE_LIBS} IGE_LIBS)

if (NOT DEFINED APP_STYLE)
    set(APP_STYLE STATIC)
endif()

if(${APP_STYLE} MATCHES "SHARED")
    set(TARGET_NAME igeAutoTest)
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
        set(CMAKE_CXX_FLAGS_DEBUG "/MD /JMC /ZI /Od")
    endif()
    add_compile_options(/MP)
elseif(IOS)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-g0 -Ofast)
    endif()
endif()

if(${APP_STYLE} MATCHES "SHARED")
    find_package(PythonInterp 3.7 REQUIRED)
    find_package(PythonLibs 3.7 REQUIRED)
else()
    set (PYTHON_INCLUDE_DIRS "${IGE_LIBS}/Python/Include")
    if (MSVC)
        list(APPEND PYTHON_INCLUDE_DIRS "${IGE_LIBS}/Python/PC")
    elseif (OSX)
        list(APPEND PYTHON_INCLUDE_DIRS "${IGE_LIBS}/Python/Mac")
    elseif (ANDROID)
        list(APPEND PYTHON_INCLUDE_DIRS "${IGE_LIBS}/Python/Android")
    elseif (IOS)
        list(APPEND PYTHON_INCLUDE_DIRS "${IGE_LIBS}/Python/IOS")
    endif ()
endif()

file(GLOB SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

set(INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${IGE_LIBS}/pyxCore/include/pyxcore"
    "${IGE_LIBS}/igeCore/include"
    "${IGE_LIBS}/SDL/include"
    "${IGE_LIBS}/json/include"
    "${IGE_LIBS}/json/include/json"
    "${IGE_LIBS}/cpp-taskflow/include"
    "${FIREBASE_CPP_SDK_DIR}/include"
    "${IGE_LIBS}/igeAdjust/include"
    "${IGE_LIBS}/igeGameAnalytics/include"
    "${IGE_LIBS}/igeAds/include"
    "${IGE_LIBS}/stb/include"
    "${PYTHON_INCLUDE_DIRS}"
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
        "${IGE_LIBS}/pyxCore/include/ThirdParty/GLEW"
    )
    file(GLOB EXTRA_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/win32/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/win32/*.cpp"
    )
    set(SRC ${SRC} ${EXTRA_SOURCES})
endif()

add_library(${TARGET_NAME} ${APP_STYLE} ${SRC})

target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDES})

if(${APP_STYLE} MATCHES "SHARED")
    set_target_properties(
        ${TARGET_NAME}
        PROPERTIES
            PREFIX ""
            OUTPUT_NAME ${TARGET_NAME}
            LINKER_LANGUAGE C
    )
    if(WIN32)
        set_target_properties(
            ${TARGET_NAME}
            PROPERTIES
            SUFFIX ".pyd"
        )
    endif()
    target_link_libraries(${TARGET_NAME} ${PYTHON_LIBRARIES})
endif()

