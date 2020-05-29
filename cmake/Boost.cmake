find_package(Boost 1.71.0 REQUIRED)

if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR})
endif()
