#define _POSIX_C_SOURCE 199309
#include <signal.h>
#include <time.h>
#include "curr_time.h"
#include "itimerspec_from_str.h"
#include "tlpi_hdr.h"

#define TIMER_SIG SIGRTMAX

int main(int argc, char *argv[]) {
    siginfo_t info;
    sigset_t mask;
    int signum;
    struct itimerspec ts;
    struct sigevent sev;
    timer_t *tidlist;
    int j;

    if (argc < 2)
        usageErr("%s secs[/nsecs][:int-secs[/int-nsecs]]...\n", argv[0]);
    
    tidlist = calloc(argc - 1, sizeof(timer_t));
    if (tidlist == NULL)
        errExit("malloc");
    
    sigemptyset(&mask);
    sigaddset(&mask, TIMER_SIG);
    
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = TIMER_SIG;

    for (j = 0; j < argc - 1; j++) {
        itimerspecFromStr(argv[j + 1], &ts);
        sev.sigev_value.sival_ptr = &tidlist[j];

        if (timer_create(CLOCK_REALTIME, &sev, &tidlist[j]) == -1)
            errExit("timer_create");
        printf("Timer ID: %ld (%s)\n", (long) tidlist[j], argv[j + 1]);

        if (timer_settime(tidlist[j], 0, &ts, NULL) == -1)
            errExit("timer_settime");
    }

    for (;;) {
        timer_t *tidptr;

        signum = sigwaitinfo(&mask, &info);

        if (signum == -1 && errno != EINTR) {
            errExit("sigwaitinfo");
        }

        tidptr = info.si_value.sival_ptr;

        printf("[%s] Got signal %d\n", currTime("%T"), signum);
        printf("    *sival_ptr        = %ld\n", (long) *tidptr);
        printf("    timer_getoverrun() = %d\n", timer_getoverrun(*tidptr));
    }
}
