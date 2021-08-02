#include <stdio.h>
#include <unistd.h>

int main() {
    printf("If I had more time, \n");
    write(STDOUT_FILENO, "I would have written you a shorter letter.\n", 43);
}
