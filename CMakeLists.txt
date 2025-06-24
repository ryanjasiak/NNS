cmake_minimum_required(VERSION 3.14)
project(NNS_CPP_Project VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
file(GLOB sources ${SRC_DIR}/*.cpp)

message(STATUS "SRC_DIR: ${SRC_DIR}")
message(STATUS "sources: ${sources}")

set(RSCRIPT_PATH "Rscript")
set(R_EXECUTABLE "R")
include(FetchContent)
include(CTest)


# ========== DOWNLOAD LIBS ==========

# Download Catch2
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.8.1 # Use latest stable tag (or update if newer)
)
FetchContent_MakeAvailable(Catch2)
include(Catch)


# ========== INCLUDE DIRECTORIES ==========


# Get R C++ include flags
execute_process(COMMAND ${R_EXECUTABLE} CMD config --cppflags
                OUTPUT_VARIABLE R_CPPFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
separate_arguments(R_CPPFLAGS)
message(STATUS "R_CPPFLAGS: ${R_CPPFLAGS}")

# Get Rcpp include flags
execute_process(COMMAND ${RSCRIPT_PATH} -e "Rcpp:::CxxFlags()"
                OUTPUT_VARIABLE RCPP_CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
separate_arguments(RCPP_CXXFLAGS)
message(STATUS "RCPP_CXXFLAGS: ${RCPP_CXXFLAGS}")

# Get RInside include flags
execute_process(COMMAND ${RSCRIPT_PATH} -e "RInside:::CxxFlags()"
                OUTPUT_VARIABLE RINSIDE_CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
separate_arguments(RINSIDE_CXXFLAGS)

message(STATUS "RINSIDE_CXXFLAGS: ${RINSIDE_CXXFLAGS}")
include_directories(/opt/homebrew/lib/R/4.5/site-library/RInside/include /opt/homebrew/lib/R/4.5/site-library/Rcpp/include /opt/homebrew/Cellar/r/4.5.0/lib/R/include)

include_directories(${R_CPPFLAGS} ${RCPP_CXXFLAGS} ${RINSIDE_CXXFLAGS})

# ========== LINK DIRECTORIES AND LIBS ==========

# Get R LDFLAGS
execute_process(COMMAND ${R_EXECUTABLE} CMD config --ldflags
                OUTPUT_VARIABLE R_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX MATCHALL "-L[^ ]+" R_LIB_DIRS "${R_LDFLAGS}")
string(REGEX MATCHALL "-l[^ ]+" R_LIBS "${R_LDFLAGS}")

# Get Rcpp link flags
execute_process(COMMAND ${RSCRIPT_PATH} -e "Rcpp:::LdFlags()"
                OUTPUT_VARIABLE RCPP_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX MATCHALL "-L[^ ]+" RCPP_LIB_DIRS "${RCPP_LDFLAGS}")
string(REGEX MATCHALL "-l[^ ]+" RCPP_LIBS "${RCPP_LDFLAGS}")

# Get RInside link flags
execute_process(COMMAND ${RSCRIPT_PATH} -e "RInside:::LdFlags()"
    OUTPUT_VARIABLE RINSIDE_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX MATCHALL "-L[^ ]+" RINSIDE_LIB_DIRS "${RINSIDE_LDFLAGS}")
message(STATUS "RINSIDE_LDFLAGS: '${RINSIDE_LDFLAGS}'")
string(REGEX MATCHALL "(^|[ \t])(-l[^ \t]+|[^ \t]*lib[^ \t]+\\.a)" RINSIDE_LIBS "${RINSIDE_LDFLAGS}")
message(STATUS "RINSIDE_LIBS: ${RINSIDE_LIBS}")

# Add linker directories
foreach(dir_flag IN LISTS R_LIB_DIRS RCPP_LIB_DIRS RINSIDE_LIB_DIRS)
  string(SUBSTRING ${dir_flag} 2 -1 dir_path)
  link_directories(${dir_path})
endforeach()
message(STATUS "R_LIBS: ${R_LIBS}")
message(STATUS "RCPP_LIBS: ${RCPP_LIBS}")
message(STATUS "RINSIDE_LIBS: ${RINSIDE_LIBS}")

# Collect all libraries (including -lR explicitly)
set(ALL_LINK_LIBS)
list(APPEND ALL_LINK_LIBS R)  # Explicitly add R
foreach(lib_flag IN LISTS R_LIBS RCPP_LIBS RINSIDE_LIBS)

#   string(SUBSTRING ${lib_flag} 2 -1 lib_name)
  if(lib_flag MATCHES "^-[lL].*")
    string(SUBSTRING ${lib_flag} 2 -1 lib_name)
  else()
    set(lib_name ${lib_flag})
  endif()
  if(NOT lib_name STREQUAL "R")  # Already added
    list(APPEND ALL_LINK_LIBS ${lib_name})
  endif()
endforeach()
message(STATUS "ALL_LINK_LIBS: ${ALL_LINK_LIBS}")
# Add BLAS and LAPACK
execute_process(COMMAND ${R_EXECUTABLE} CMD config BLAS_LIBS
                OUTPUT_VARIABLE RBLAS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${R_EXECUTABLE} CMD config LAPACK_LIBS
                OUTPUT_VARIABLE RLAPACK OUTPUT_STRIP_TRAILING_WHITESPACE)

separate_arguments(RBLAS)
separate_arguments(RLAPACK)

list(APPEND ALL_LINK_LIBS ${RBLAS} ${RLAPACK} Catch2::Catch2WithMain)


# ========== COMPILER FLAGS ==========

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

if (CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR
    CMAKE_BUILD_TYPE STREQUAL "RelWithDebugInfo")
    add_definitions(-DDEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    add_definitions(-O3)
endif()

# ========== TARGETS ==========

foreach(test_file ${sources})
  get_filename_component(test_name ${test_file} NAME_WE)
  message(STATUS "Creating test: ${test_name}")
  add_executable(${test_name} ${test_file})
  target_link_libraries(${test_name} ${ALL_LINK_LIBS})
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
