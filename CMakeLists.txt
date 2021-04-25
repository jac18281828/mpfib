cmake_minimum_required(VERSION 3.9)

project(${PROJECT_NAME} LANGUAGES CXX VERSION 1.0.0)

add_compile_options(-std=c++20)

set(HDR include/bigmp.h)

set(SRC src/main.cc)

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} ${HDR} ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} m)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        bigmp_test
        test/bigmp_test.cc
        test/nr_test.cc
)
target_include_directories(bigmp_test PRIVATE include)
target_link_libraries(
  bigmp_test
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(bigmp_test)

