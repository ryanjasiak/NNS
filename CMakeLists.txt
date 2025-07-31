cmake_minimum_required(VERSION 3.28)  # For good C++20 and module support
project(my_cpp_library LANGUAGES CXX)

# Enable C++20 and experimental modules
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API ON)

# Library target (header-only)
add_library(NNS INTERFACE)
target_include_directories(NNS INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API ON)
set_target_properties(NNS PROPERTIES
    CXX_STANDARD          20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS        NO)   # -std=c++20 instead of -std=gnu++20
# (Optional) Install support
include(GNUInstallDirs)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(FetchContent)

include(CTest)  # defines BUILD_TESTING
# Add tests
# Optional tests only make sense if BUILD_TESTING is ON
option(ENABLE_OPTIONAL_TESTS "Build optional test cases" OFF)

if(BUILD_TESTING)
    enable_testing()    
    add_subdirectory(test)

    if(ENABLE_OPTIONAL_TESTS)
        add_subdirectory(test/against_legacy)
    endif()
endif()