#include <stdio.h>
// #include "file1.c"

int main() {
    extern int y; // ANSWER: Compiler error because you cannot initialize extern variable
    printf("Hello World! %d\n", y);
    return 0;
}