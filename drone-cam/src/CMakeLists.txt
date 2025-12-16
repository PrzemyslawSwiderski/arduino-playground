# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

# Glob all files in the data directory
FILE(GLOB data_files ${CMAKE_SOURCE_DIR}/data/*)

idf_component_register(
    SRCS ${app_sources}
    EMBED_FILES ${data_files}
)
