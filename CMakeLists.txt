cmake_minimum_required(VERSION 3.13)
project(shell C)
set(CMAKE_C_STANDARD 11)

include_directories(/usr/include/readline)
add_executable(shell main.c extra.c extra.h parser.c parser.h commandes_internes.c commandes_internes.h traitement.c traitement.h)
target_link_libraries(shell readline)