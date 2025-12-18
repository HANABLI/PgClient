# @file CMakeLists.txt for the Postgresql::PgClient library.
#
# © copyright 2025 by Hatem Nabli.

cmake_minimum_required(VERSION 3.8)
set(this PgClient)

set(Headers
    include/PgClient/PgClient.hpp
    include/PgClient/PgResult.hpp
)
set(Sources
    src/PgClient.cpp
    src/PgResult.cpp
)

add_library(${this} STATIC ${Sources} ${Headers})
set_target_properties(${this} PROPERTIES
    FOLDER Libraries
)
find_path(LIBPQ_INCLUDE_DIR libpq-fe.h
  HINTS
    "$ENV{PROGRAMFILES}/PostgreSQL/18/include"
    "C:/Program Files/PostgreSQL/18/include"
)

find_library(LIBPQ_LIBRARY NAMES libpq
  HINTS
    "$ENV{PROGRAMFILES}/PostgreSQL/18/lib/"
    "C:/Program Files/PostgreSQL/18/lib/"
)

if (LIBPQ_INCLUDE_DIR AND LIBPQ_LIBRARY)
  message(STATUS "Found libpq include: ${LIBPQ_INCLUDE_DIR}, lib: ${LIBPQ_LIBRARY}")
  target_include_directories(PgClient PUBLIC ${LIBPQ_INCLUDE_DIR})
  target_link_libraries(PgClient PUBLIC ${LIBPQ_LIBRARY})
else()
  message(FATAL_ERROR "libpq (libpq-fe.h / libpq) not found. Install PostgreSQL dev files or use vcpkg.")
endif()

target_link_libraries(${this} PUBLIC
    SystemUtils
    Json
)

target_include_directories(${this} PUBLIC include)
