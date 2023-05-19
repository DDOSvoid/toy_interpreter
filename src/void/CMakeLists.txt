add_library(void_obj OBJECT token.cpp lexer.cpp ast.cpp parser.cpp object.cpp evaluator.cpp builtin.cpp)

target_include_directories(void_obj PUBLIC include)

add_library(void_shared SHARED)
target_link_libraries(void_shared void_obj)

add_library(void_static STATIC)
target_link_libraries(void_static void_obj)
