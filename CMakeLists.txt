cmake_minimum_required(VERSION 3.13)
project(Bezpieczne_przesylanie_plikow C)

set(CMAKE_C_STANDARD 99)

link_libraries(-lcrypto -lssl)

add_executable(server server/main.c )
add_executable(client client/main.c)

