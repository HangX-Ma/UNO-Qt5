cmake_minimum_required(VERSION 3.5)

project(UNO VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# determine the language standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -O2 -g -fPIC")

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

# find fundamental components
find_package(QT NAMES Qt6 Qt5 REQUIRED 
    COMPONENTS 
        Widgets
        Network
        Core
        Gui
    )
find_package(Qt${QT_VERSION_MAJOR} REQUIRED 
    COMPONENTS 
        Widgets
        Network
        Core
        Gui
    )

# user determined build option
option(WITH_QRC "build project with .qrc file" OFF)
if (WITH_QRC)
    set(QRC_SOURCE_FILES UNO2D.qrc)
    qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})
    source_group("Resources Files" FILES ${QRC_SOURCE_FILES})
else()
    set(QRC_FILES)
endif()

# find the source files
file(GLOB_RECURSE CURRENT_DIR_HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)
file(GLOB CURRENT_DIR_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(UNO 
    ${CURRENT_DIR_SOURCES} 
    ${QRC_FILES} 
    ${CURRENT_DIR_HEADERS}
    )

target_link_libraries(UNO 
    PRIVATE 
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
    )

target_include_directories(UNO 
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        $ENV{QTDIR}/5.15.2/gcc_64/include
    )

set_target_properties(UNO 
    PROPERTIES
        WIN32_EXECUTABLE TRUE
    )