#include <unistd.h>

int main(int argc, char const *argv[]) {
    write(STDOUT_FILENO, "test", 4);
    return 0;
}
