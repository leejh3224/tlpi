#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <paths.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 50

static struct pid {
    struct pid *next;
    FILE *fp;
    pid_t pid;
} *pidlist;

extern char **environ;

FILE *popen(const char *command, const char *type) {
    struct pid *volatile cur;
    FILE *iop;
    int pdes[2];
    pid_t pid;
    char *argp[] = {"sh", "-c", NULL, NULL};

    if ((cur = malloc(sizeof(struct pid))) == NULL)
        return NULL;
    
    if (pipe(pdes) < 0) {
        free(cur);
        return NULL;
    }

    switch (pid = fork()) {
        case -1:
            close(pdes[0]);
            close(pdes[1]);
            free(cur);
            return NULL;
        case 0:
            struct pid *pcur;

            for (pcur = pidlist; pcur; pcur = pcur->next)
                close(fileno(pcur->fp));
            
            if (*type == 'r') {
                close(pdes[0]);
                if (pdes[0] != STDIN_FILENO) {
                    dup2(pdes[1], STDOUT_FILENO);
                    close(pdes[1]);
                }
            }

            if (*type == 'w') {
                close(pdes[1]);
                if (pdes[0] != STDIN_FILENO) {
                    dup2(pdes[0], STDIN_FILENO);
                    close(pdes[0]);
                }
            }

            argp[2] = (char *) command;
            execve(_PATH_BSHELL, argp, environ);
            _exit(127);
    }

    if (*type == 'r') {
        iop = fdopen(pdes[0], type);
        close(pdes[1]);
    }

    if (*type == 'w') {
        iop = fdopen(pdes[1], type);
        close(pdes[0]);
    }

    cur->fp = iop;
    cur->pid = pid;
    cur->next = pidlist;
    pidlist = cur;

    return iop;
}

int pclose(FILE *iop) {
    struct pid *cur, *last;
    int pstat;
    pid_t pid;

    for (last = NULL, cur = pidlist; cur; last = cur, cur = cur->next)
        if (cur->fp == iop)
            break;
    
    if (cur == NULL)
        return -1;
    
    fclose(iop);

    do {
        pid = waitpid(cur->pid, &pstat, 0);
    } while (pid == -1 && errno == EINTR);

    if (last == NULL)
        pidlist = cur->next;
    else
        last->next = cur->next;
    
    free(cur);

    return pid == -1 ? -1 : pstat;
}

int main(int argc, char *argv[]) {
    FILE *fp;

    fp = popen("ls -l | wc -l", "r");
    if (fp == NULL)
        errExit("popen() failed");
    
    char buf[BUF_SIZE];
    while (fgets(buf, BUF_SIZE, fp) != NULL) {
        printf("%s\n", buf);
    }

    int status = pclose(fp);
    if (status == -1)
        errExit("pclose() failed");
    
    exit(EXIT_SUCCESS);
}
