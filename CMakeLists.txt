cmake_minimum_required(VERSION 3.28)  # For good C++23 and module support
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

# (Optional) Install support
include(GNUInstallDirs)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(FetchContent)

# === Add Eigen ===
option(NNS_USE_SYSTEM_EIGEN "Use system-provided Eigen" ON)

if(NNS_USE_SYSTEM_EIGEN)
    find_package(Eigen3 REQUIRED)
    target_link_libraries(NNS INTERFACE Eigen3::Eigen)
else()
    FetchContent_Declare(
        eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG        3.4.0
    )
    FetchContent_MakeAvailable(eigen)
    target_include_directories(NNS INTERFACE ${eigen_SOURCE_DIR})
endif()

# Add tests
enable_testing()
add_subdirectory(test)