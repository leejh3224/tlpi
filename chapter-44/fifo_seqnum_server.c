#include <signal.h>
#include "fifo_seqnum.h"
#include <math.h>

#define SEQNUM_FILE "/tmp/seqnum"

static void removeFifo(void) {
    unlink(SERVER_FIFO);
}

int main(int argc, char *argv[]) {
    int serverFd, dummyFd, clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    FILE *seqFp;
    int seqNum;
    char buf[MAX_SEQ_LEN];

    if (access(SEQNUM_FILE, F_OK) < 0) {
        seqFp = fopen(SEQNUM_FILE, "w");
        fprintf(seqFp, "0");

        seqNum = 0;
    } else {
        seqFp = fopen(SEQNUM_FILE, "r");

        if (fgets(buf, MAX_SEQ_LEN, seqFp) != NULL) {
            seqNum = atoi(&buf[0]);
        }
    }

    if (signal(SIGINT, &removeFifo) == SIG_ERR)
        errExit("signal");
    
    if (signal(SIGTERM, &removeFifo) == SIG_ERR)
        errExit("signal");

    umask(0);
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
        errExit("mkfifo %s", SERVER_FIFO);
    
    serverFd = open(SERVER_FIFO, O_RDONLY);
    if (serverFd == -1)
        errExit("open %s", SERVER_FIFO);

    dummyFd = open(SERVER_FIFO, O_WRONLY);
    if (dummyFd == -1)
        errExit("open %s", SERVER_FIFO);
    
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        errExit("signal");
    
    for (;;) {
        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
            fprintf(stderr, "Error reading request; discarding\n");
            continue;
        }

        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) req.pid);
        clientFd = open(clientFifo, O_WRONLY);
        if (clientFd == -1) {
            errMsg("open %s", clientFifo);
            continue;
        }
    
        resp.seqNum = seqNum;
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response)) {
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
        }

        if (close(clientFd) == -1)
            errMsg("close");

        seqNum += req.seqLen;

        // update
        int fd = open(SEQNUM_FILE, O_WRONLY | O_SYNC);
        char data[MAX_SEQ_LEN];
        int seqNumLen = 1 + (int) log10((double) seqNum);
        snprintf(data, seqNumLen, "%d", seqNum);
        write(fd, data, strlen(data));
    }
}
