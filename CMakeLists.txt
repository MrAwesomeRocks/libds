cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    libds
    VERSION 0.1.0
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES NONE
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

find_package(Microsoft.GSL)

# ---- Declare library ----

add_library(libds_libds INTERFACE)
add_library(libds::libds ALIAS libds_libds)

set_property(
    TARGET libds_libds PROPERTY
    EXPORT_NAME libds
)

target_include_directories(
    libds_libds ${warning_guard}
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(libds_libds INTERFACE cxx_std_17)

target_link_libraries(libds_libds
  INTERFACE Microsoft.GSL::GSL)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${libds_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- Developer mode ----

if(NOT libds_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of libds"
  )
endif()

include(cmake/dev-mode.cmake)
