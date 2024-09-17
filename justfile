default:
  @just --list

build:
  gcc -Wall -Wextra -Wswitch-enum -Wpedantic -ggdb -std=c11 \
    -lraylib linear.c board.c 2048.c -o main

run: build
  ./main

rr:
  gcc -Wall -Wextra -Wswitch-enum -Wpedantic -ggdb -std=c11 \
    -lraylib linear.c board.c main.c -o main
  ./main
