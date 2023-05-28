
#[[ Any copyright is dedicated to the Public Domain.
    https://creativecommons.org/publicdomain/zero/1.0/ ]]

cmake_minimum_required(VERSION 3.1)
Project(TestLibCMD)
set (CMAKE_CXX_STANDARD 20)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.2
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests testlibcmd.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
