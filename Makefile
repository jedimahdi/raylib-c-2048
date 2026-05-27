build:
	gcc -Wall -Wextra -Wswitch-enum -Wpedantic -ggdb -std=c11 -I./raylib/include -L./raylib/lib -lraylib -lm -Wl,-rpath,'$(PWD)/raylib/lib' 2048.c -o main
