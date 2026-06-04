#include <unistd.h>
#include <fcntl.h>

#define TARGET_FD 3

int main(void)
{
    char buf[4096];
    int n;

    lseek(TARGET_FD, 0, SEEK_SET);

    while ((n = read(TARGET_FD, buf, sizeof(buf))) > 0)
        write(1, buf, n);

    return 0;
}