cmake_minimum_required(VERSION 3.14)

project(libdsTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(libds REQUIRED)
  enable_testing()
endif()

find_package(Catch2 REQUIRED)
include(Catch)

# ---- Tests ----

add_executable(
  libds_test
    source/vec.cpp
)
target_link_libraries(
    libds_test PRIVATE
    libds::libds
    Catch2::Catch2WithMain
)
target_compile_features(libds_test PRIVATE cxx_std_17)

catch_discover_tests(libds_test)

# ---- End-of-file commands ----

add_folders(Test)
