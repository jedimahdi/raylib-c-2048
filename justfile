default:
  @just --list

build:
  gcc -Wall -Wextra -Wswitch-enum -Wpedantic -ggdb -std=c11 \
    -lraylib board.c main.c -o main

run: build
  ./main
