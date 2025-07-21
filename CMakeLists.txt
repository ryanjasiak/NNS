cmake_minimum_required(VERSION 3.28)  # For good C++23 and module support
project(my_cpp_library LANGUAGES CXX)

# --- Apple Clang: manually set OpenMP paths for Homebrew ---
if(APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include")
    set(OpenMP_CXX_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY /opt/homebrew/opt/libomp/lib/libomp.dylib)
    set(OpenMP_CXX_INCLUDE_DIRS "/opt/homebrew/opt/libomp/include")
    set(OpenMP_CXX_LIBRARIES "/opt/homebrew/opt/libomp/lib/libomp.dylib")
endif()

# Enable C++20 and experimental modules
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API ON)

# Library target (header-only)
add_library(NNS INTERFACE)
target_include_directories(NNS INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
# Tell *every* consumer that C++23 is required
target_compile_features(NNS INTERFACE cxx_std_23)

# Optional but recommended: no GNU / MSVC extensions
set_target_properties(NNS PROPERTIES
    CXX_STANDARD          23
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS        NO)   # -std=c++23 instead of -std=gnu++23


# (Optional) Install support
include(GNUInstallDirs)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(FetchContent)

# === Add Eigen ===
# option(NNS_USE_SYSTEM_EIGEN "Use system-provided Eigen" OFF)

# if(NNS_USE_SYSTEM_EIGEN)
#     find_package(Eigen3 REQUIRED)
#     target_link_libraries(NNS INTERFACE Eigen3::Eigen)
# else()
    FetchContent_Declare(
        eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG        3.4.0
    )
    FetchContent_MakeAvailable(eigen)
    target_include_directories(NNS INTERFACE ${eigen_SOURCE_DIR})
# endif()

# === Enable OpenMP ===
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    target_link_libraries(NNS INTERFACE OpenMP::OpenMP_CXX)
else()
    message(WARNING "OpenMP not found: CPU parallelism will be disabled")
endif()

# Add tests
enable_testing()
add_subdirectory(test)