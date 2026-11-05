#include "timer.h"
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <disprint/disprint.h>

int fexit;

static void shutdown_handler(int signo)
{
    fexit = 1;
}

static void alarm()
{
    puts("ALARM or something");
}

int main(int argc, char **argv)
{
    struct sigaction act;
    time_t hours, minutes, seconds;
    char *s0, *s1, *s2;
    time_t end_time, cur_time, prev_cur_time;
    struct timespec delay;

    memset(&act, 0, sizeof act);
    memset(&delay, 0, sizeof delay);
    act.sa_handler = &shutdown_handler;
    delay.tv_nsec = 16000000;
    fexit = 0;
    if(sigaction(SIGINT, &act, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if(argc != 2)
    {
        puts("timer ((hours:(minutes)):seconds)");
        exit(EXIT_SUCCESS);
    }

    s0 = strtok(argv[1], ":");
    s1 = strtok(NULL, ":");
    s2 = strtok(NULL, ":");
    if(s0 == NULL)
    {
        puts("invalid input");
        exit(EXIT_FAILURE);
    }
    hours = 0;
    minutes = atoi(s0);
    seconds = 0;
    if(s1 != NULL)
    {
        minutes = atoi(s0);
        seconds = atoi(s1);
    }
    if(s2 != NULL)
    {
        seconds = atoi(s2);
        minutes = atoi(s1);
        hours = atoi(s0);
    }

    cur_time = time(NULL);
    prev_cur_time = 0;
    end_time = cur_time + seconds + 60 * minutes + 3600 * hours;

    while(cur_time < end_time && !fexit)
    {
        char s_time_left[50];
        struct tm tm_left;
        time_t time_diff;

        cur_time = time(NULL);
        nanosleep(&delay, NULL);

        if(cur_time != prev_cur_time)
        {
            time_t dh, dm, ds;

            prev_cur_time = cur_time;

            time_diff = end_time - cur_time;

            dh = time_diff / 3600;
            dm = (time_diff % 3600) / 60;
            ds = (time_diff % 3600) % 60;

            snprintf(s_time_left,
              sizeof s_time_left, "%02d:%02d:%02d", dh, dm, ds);
                
            if(s_time_left == NULL)
            {
                fprintf(stderr, "Error formatting time.");
                exit(EXIT_FAILURE);
            }

            disflip(s_time_left, NULL);
        }
    }

    disflip(NULL, NULL);

    alarm();

    return 0;
}
