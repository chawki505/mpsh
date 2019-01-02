cmake_minimum_required(VERSION 3.13)
project(shell C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -g -std=gnu11")

include_directories(/usr/include/readline)
add_executable(shell main.c variables.h commandes_internes.c commandes_internes.h extra.c extra.h analyseur.c analyseur.h redirection.c redirection.h)
target_link_libraries(shell readline)
