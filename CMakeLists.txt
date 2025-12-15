# @file CMakeLists.txt for the Postgresql::PgClient library.
#
# © copyright 2025 by Hatem Nabli.

cmake_minimum_required(VERSION 3.8)
set(this PgClinet)

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
# Find PostgreSQL (libpq)
find_package(PostgreSQL REQUIRED)

# Link against libpq
target_link_libraries(${this}
    PRIVATE
        PostgreSQL::PostgreSQL
)

target_link_libraries(${this} PUBLIC
    SystemUtils
)

target_include_directories(${this} PUBLIC include)
