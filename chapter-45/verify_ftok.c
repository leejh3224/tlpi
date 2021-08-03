#define DEFAULT_SOURCE
#include <sys/sysmacros.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    const char *path = "./epoll.c";
    int proj_id = 1;
    key_t key;

    key = ftok(path, proj_id);
    if (key == -1)
        errExit("ftok");
    
    printf("key = %x\n", (int) key);

    int fd, inode;
    fd = open(path, O_RDONLY);

    if (fd < 0)
        errExit("open");
    
    struct stat file_stat;

    if (fstat(fd, &file_stat) < 0)
        errExit("fstat");
    
    inode = file_stat.st_ino;

    printf("inode = %x\n", (int) inode);

    dev_t dev = file_stat.st_dev;

    printf("minor dev number = %x\n", (int) dev);

    printf("proj number = %x\n", 1);
}
