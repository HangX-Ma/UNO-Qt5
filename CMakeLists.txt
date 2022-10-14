cmake_minimum_required(VERSION 3.5)

project(UNO VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# determine the language standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -O2 -fPIC")

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

# build out of source
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Please select another folder to build the project.")
endif()

# build type checker
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message("Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
        "MinSizeRel" "RelWithDebInfo")
    # force configuration set in VS
    set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)
endif()
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

option(CROSS_COMPILE_WIN64 "build exe file for win64" OFF)
if(CROSS_COMPILE_WIN64)
    # if you want to compile for win10 
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_SYSTEM_VERSION 1)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)

    # which compilers to use for C and C++
    set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc-posix)
    set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++-posix)
    SET(CMAKE_RC_COMPILER  x86_64-w64-mingw32-windres)
    # set(CMAKE_AR           x86_64-w64-mingw32-ar CACHE FILEPATH "Archiver")
    # set(CMAKE_RANLIB       x86_64-w64-mingw32-ranlib CACHE FILEPATH "Ranlib")
    # set(CMAKE_ASM_COMPILER x86_64-w64-mingw32-as)
    set(CMAKE_LINKER       x86_64-w64-mingw32-ld)
    # set(CMAKE_NM           x86_64-w64-mingw32-nm)
    # set(CMAKE_OBJCOPY      x86_64-w64-mingw32-objcopy)
    # set(CMAKE_OBJDUMP      x86_64-w64-mingw32-objdump)
    # set(CMAKE_STRIP        x86_64-w64-mingw32-strip)

    # Set or retrieve the cached flags.
    # This is necessary in case the user sets/changes flags in subsequent
    # configures. If we included the flags in here, they would get
    # overwritten.
    set(CMAKE_C_FLAGS ""
            CACHE STRING "Flags used by the compiler during all build types.")
    set(CMAKE_CXX_FLAGS ""
            CACHE STRING "Flags used by the compiler during all build types.")
    set(CMAKE_ASM_FLAGS ""
            CACHE STRING "Flags used by the compiler during all build types.")
    set(CMAKE_C_FLAGS_DEBUG ""
            CACHE STRING "Flags used by the compiler during debug builds.")
    set(CMAKE_CXX_FLAGS_DEBUG ""
            CACHE STRING "Flags used by the compiler during debug builds.")
    set(CMAKE_ASM_FLAGS_DEBUG ""
            CACHE STRING "Flags used by the compiler during debug builds.")
    set(CMAKE_C_FLAGS_RELEASE ""
            CACHE STRING "Flags used by the compiler during release builds.")
    set(CMAKE_CXX_FLAGS_RELEASE ""
            CACHE STRING "Flags used by the compiler during release builds.")
    set(CMAKE_ASM_FLAGS_RELEASE ""
            CACHE STRING "Flags used by the compiler during release builds.")
    set(CMAKE_MODULE_LINKER_FLAGS ""
            CACHE STRING "Flags used by the linker during the creation of modules.")
    set(CMAKE_SHARED_LINKER_FLAGS ""
            CACHE STRING "Flags used by the linker during the creation of dll's.")
    set(CMAKE_EXE_LINKER_FLAGS ""
            CACHE STRING "Flags used by the linker.")

    set(CMAKE_C_FLAGS             "${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS}")
    set(CMAKE_ASM_FLAGS           "${CMAKE_ASM_FLAGS}")
    set(CMAKE_C_FLAGS_DEBUG       "${CMAKE_C_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_DEBUG     "${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_ASM_FLAGS_DEBUG     "${CMAKE_ASM_FLAGS_DEBUG}")
    set(CMAKE_C_FLAGS_RELEASE     "${CMAKE_C_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_ASM_FLAGS_RELEASE   "${CMAKE_ASM_FLAGS_RELEASE}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}")

    # where is the target environment located
    set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32
                            /usr/lib/gcc/x86_64-w64-mingw32/9.3-posix)

    # adjust the default behavior of the FIND_XXX() commands:
    # search programs in the host environment
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

    # search headers and libraries in the target environment
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    # set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

endif()

# Qt checker
if(DEFINED ENV{QTDIR})
    message(STATUS "QTDIR: $ENV{QTDIR}")
else()
    message(FATAL_ERROR "NO DEFINED QTDIR VARIABLES")
endif()

# find fundamental components
find_package(QT NAMES Qt6 Qt5 REQUIRED 
    COMPONENTS 
        Widgets
        Network
        Core
        Gui
        Multimedia
    )
find_package(Qt${QT_VERSION_MAJOR} REQUIRED 
    COMPONENTS 
        Widgets
        Network
        Core
        Gui
        Multimedia
    )

# user determined build option
option(WITH_QRC "build project with .qrc file" ON)
if (WITH_QRC)
    set(QRC_SOURCE_FILES UNORes.qrc)
    qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})
    source_group("Resources Files" FILES ${QRC_SOURCE_FILES})
else()
    set(QRC_FILES)
endif()


set(FRONT_END_HEADERS_ROOT_DIR ${CMAKE_SOURCE_DIR}/front-end/include)
set(FRONT_END_SOURCES_ROOT_DIR ${CMAKE_SOURCE_DIR}/front-end/src)

# find the necessary files
file(GLOB_RECURSE FRONT_DIR_HEADERS 
    ${FRONT_END_HEADERS_ROOT_DIR}/*.h
    ${FRONT_END_HEADERS_ROOT_DIR}/*.hpp)
file(GLOB FRONT_DIR_SOURCES 
    ${FRONT_END_SOURCES_ROOT_DIR}/*.cpp)

add_executable(UNO 
    ${FRONT_DIR_SOURCES} 
    ${QRC_FILES} 
    ${FRONT_DIR_HEADERS}
    )

target_link_libraries(UNO 
    PRIVATE 
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Multimedia
    )

target_include_directories(UNO 
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${FRONT_END_HEADERS_ROOT_DIR}
        $ENV{QTDIR}/${QT_VERSION}/gcc_64/include
    )

set_target_properties(UNO 
    PROPERTIES
        WIN32_EXECUTABLE TRUE
    )

message(STATUS "Build documentation open")
include(${CMAKE_CURRENT_LIST_DIR}/cmake-modules/build_doxygen.cmake)
build_doxygen()