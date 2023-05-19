include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  unit_test
  token_test.cpp
  lexer_test.cpp
  parser_test.cpp
  evaluator_test.cpp
)
target_link_libraries(
  unit_test
  PRIVATE gtest_main
  PRIVATE void_obj
)
include(GoogleTest)
gtest_discover_tests(unit_test)
