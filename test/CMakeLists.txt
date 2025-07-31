set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
file(GLOB sources ${SRC_DIR}/*.cpp)

include(FetchContent)

# ========== DOWNLOAD LIBS ==========

# Download Catch2
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.8.1 # Use latest stable tag (or update if newer)
)
FetchContent_MakeAvailable(Catch2)
include(Catch)


set(ALL_LINK_LIBS)
list(APPEND ALL_LINK_LIBS Catch2::Catch2WithMain)

foreach(test_file ${sources})
  get_filename_component(test_name ${test_file} NAME_WE)
  message(STATUS "Creating test: ${test_name}")
  add_executable(${test_name} ${test_file})
  target_link_libraries(${test_name} ${ALL_LINK_LIBS} NNS)
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
