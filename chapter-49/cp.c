#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    char *contents;
    int src, dest;
    struct stat sbuf;

    src = open(argv[1], O_RDONLY);
    if (src == -1)
        errExit("open");
    
    if (fstat(src, &sbuf) == -1)
        errExit("fstat");
    
    contents = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, src, 0);
    if (contents == MAP_FAILED) {
        close(src);
        errExit("mmap");
    }
    
    dest = open(argv[2], O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if (dest == -1)
        errExit("open");

    if (ftruncate(dest, sbuf.st_size) == -1)
        errExit("ftruncate");
    
    char *dst = mmap(NULL, sbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, dest, 0);
    if (dst == MAP_FAILED) {
        close(dest);
        errExit("mmap");
    }

    memcpy(dst, contents, sbuf.st_size);

    if (msync(dst, sbuf.st_size, MS_SYNC) == -1)
        errExit("msync");

    // if (write(dest, contents, sbuf.st_size) != sbuf.st_size)
    //     errExit("write");

    // // make sure file is flushed
    // if (close(dest) == -1);
    //     errExit("close");

    // dest = open(argv[2], O_RDONLY);
    // if (dest == -1)
    //     errExit("open");

    // file content is updated although fd wasn't closed
    char *buf = (char *) malloc(sbuf.st_size);
    int result = read(dest, buf, sbuf.st_size);
    printf("read %d bytes\n", result);

    printf("%.*s\n", sbuf.st_size, buf);

    free(buf);

    if (close(src) == -1)
        errExit("close");
    if (close(dest) == -1)
        errExit("close");

    exit(EXIT_SUCCESS);
}
