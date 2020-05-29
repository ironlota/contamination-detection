cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)

set(PROJECT_TITLE           "MYCELIUM_CONTAMINATION")
set(PROJECT_ALT_TITLE       "MyceliumContamination")
set(PROJECT_DESCRIPTION     "Mycelium Automated Contamination Detection")
set(PROJECT_AUTHOR          "Ray Andrew")
set(PROJECT_VERSION_MAJOR   0)
set(PROJECT_VERSION_MINOR   0)
set(PROJECT_VERSION_PATCH   1)
set(PROJECT_VERSION         "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_NAMESPACE       "emmerich")

project(${PROJECT_TITLE}
  VERSION ${PROJECT_VERSION}
  DESCRIPTION ${PROJECT_DESCRIPTION})

option(ENABLE_LTO "Enable link time optimization" ON)
option(BUILD_SHARED_LIBS "Build shared libs in order for some libraries to work" ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(ConfigSafeGuards)
include(Colors)
include(Cotire)
include(CTest)
include(Documentation)
include(LTO)
include(Misc)
include(Warnings)

# Vendors
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
# include(Boost)
include(Gtkmm)

find_lto(CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES
  src/detector.cpp
  src/camera.cpp)

set(LIBRARY_NAME engine)

# Compile all sources into a library.
add_library(${LIBRARY_NAME} OBJECT
  ${SOURCES})

target_include_directories(${LIBRARY_NAME} PUBLIC
  ${GTKMM_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${LIBRARY_NAME} PUBLIC
  ${OpenCV_LIBS}
  ${GTKMM_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  # doctest
  spdlog)

# Add an executable for the file app/main.cpp.
add_executable(main app/main.cpp)

target_link_libraries(main PRIVATE
  ${LIBRARY_NAME}
  ${OpenCV_LIBS}
  ${GTKMM_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  spdlog)

# Precompiled Headers
# cotire(main)

target_enable_lto(main optimized)

set_target_properties(
    ${LIBRARY_NAME} main 
    PROPERTIES
        CXX_STANDARD 17 
        CXX_STANDARD_REQUIRED YES 
        CXX_EXTENSIONS NO)

configure_file(
  "${PROJECT_SOURCE_DIR}/ui.glade"
  "${PROJECT_BINARY_DIR}/ui.glade"
  COPYONLY)
