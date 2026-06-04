#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
    DIR *dir;
    struct dirent *entry;
    char path[256];
    char link[256];
    ssize_t len;

    dir = opendir("/proc/self/fd");
    if (!dir) {
        perror("opendir");
        return 1;
    }

    printf("Open file descriptors:\n");

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] >= '0' && entry->d_name[0] <= '9') {
            snprintf(path, sizeof(path), "/proc/self/fd/%s", entry->d_name);

            len = readlink(path, link, sizeof(link) - 1);
            if (len != -1) {
                link[len] = '\0';
                printf("fd %s -> %s\n", entry->d_name, link);
            }
        }
    }

    closedir(dir);
    return 0;
}