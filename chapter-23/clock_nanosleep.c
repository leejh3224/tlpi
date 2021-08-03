#define _POSIX_C_SOURCE 199309
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "tlpi_hdr.h"

static void sigintHandler(int sig) {
    return;
}

int main(int argc, char *argv[]) {
    struct timeval start, finish;
    struct timespec request, remain;
    struct sigaction sa;
    int s, flags;

    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s secs nanosecs [a]\n", argv[0]);

    /* Allow SIGINT handler to interrupt clock_nanosleep() */

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigintHandler;
    if (sigaction(SIGINT, &sa, NULL) == -1)
        errExit("sigaction");

    /* If more than three command-line arguments, use TIMER_ABSTIME flag */

    flags = (argc > 3) ? TIMER_ABSTIME : 0;

    if (flags == TIMER_ABSTIME) {
        if (clock_gettime(CLOCK_REALTIME, &request) == -1)
            errExit("clock_gettime");
        printf("Initial CLOCK_REALTIME value: %ld.%09ld\n",
                (long) request.tv_sec, (long) request.tv_nsec);

        request.tv_sec  += getLong(argv[1], 0, "secs");
        request.tv_nsec += getLong(argv[2], 0, "nanosecs");
        if (request.tv_nsec >= 1000000000) {
            request.tv_sec += request.tv_nsec / 1000000000;
            request.tv_nsec %= 1000000000;
        }

    } else {                    /* Relative sleep */
        request.tv_sec  = getLong(argv[1], 0, "secs");
        request.tv_nsec = getLong(argv[2], 0, "nanosecs");
    }

    if (gettimeofday(&start, NULL) == -1)
        errExit("gettimeofday");

    for (;;) {
        s = clock_nanosleep(CLOCK_REALTIME, flags, &request, &remain);
        if (s != 0 && s != EINTR)
            errExitEN(s, "clock_nanosleep");

        if (s == EINTR)
            printf("Interrupted... ");

        if (gettimeofday(&finish, NULL) == -1)
            errExit("gettimeofday");
        printf("Slept: %.6f secs", finish.tv_sec - start.tv_sec +
                        (finish.tv_usec - start.tv_usec) / 1000000.0);

        if (s == 0)
            break;                      /* sleep completed */

        if (flags != TIMER_ABSTIME) {
            printf("... Remaining: %ld.%09ld",
                    (long) remain.tv_sec, remain.tv_nsec);

            request = remain;
        }

        printf("... Restarting\n");
    }

    printf("\nSleep complete\n");
    exit(EXIT_SUCCESS);
} 
