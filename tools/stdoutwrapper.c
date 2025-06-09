#include <unistd.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
    if (argc < 3) return -1;
    freopen(argv[1], "w", stdout);
    execv(argv[2], &argv[2]);
    return 0;
}