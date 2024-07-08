default:
  @just --list

build:
  gcc -Wall -Wextra -Wswitch-enum -Wpedantic -ggdb -std=c11 \
    -lraylib animation.c board.c main.c -o main

run: build
  ./main
