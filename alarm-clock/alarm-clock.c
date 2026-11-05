#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <disprint/disprint.h>

int fexit;

void shutdown_handler(int signo)
{
    fexit = 1;
}

void alarm()
{
    puts("ALARM or something");
}

int main(int argc, char **argv)
{
    struct sigaction act;
    int hours, minutes, seconds;
    char *sh, *sm, *ss;
    char s_end_time[50];
    time_t end_time, cur_time, prev_cur_time;
    struct tm *tm_end_time, *tm_cur_time;
    struct timespec delay;
    int fdisplay;

    setvbuf(stdout, NULL, _IONBF, 0);

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
        puts("alarm-clock (hours(:minutes(:seconds)))");
        exit(EXIT_SUCCESS);
    }

    sh = strtok(argv[1], ":");
    sm = strtok(NULL, ":");
    ss = strtok(NULL, ":");
    if(sh == NULL)
    {
        puts("invalid input");
        exit(EXIT_FAILURE);
    }
    hours = atoi(sh);
    minutes = 0;
    seconds = 0;
    if(sm != NULL)
        minutes = atoi(sm);
    if(ss != NULL)
        seconds = atoi(ss);

    cur_time = time(NULL);
    prev_cur_time = cur_time;
    tm_end_time = malloc(sizeof *tm_end_time);
    tm_cur_time = malloc(sizeof *tm_cur_time);
    (void)localtime_r(&cur_time, tm_cur_time);
    memcpy(tm_end_time, tm_cur_time, sizeof *tm_end_time);
    tm_end_time->tm_hour = hours;
    tm_end_time->tm_min = minutes;
    tm_end_time->tm_sec = seconds;
    end_time = mktime(tm_end_time);
    if(end_time < cur_time)
    {
        tm_end_time->tm_mday++;
        end_time = mktime(tm_end_time);
    }

    strftime(s_end_time, sizeof s_end_time,
        "%H:%M:%S",
        tm_end_time);

    printf("Alarm: %s\n", s_end_time);

    while(cur_time < end_time && !fexit)
    {
        char s_time_left[50], s_the_time[50];
        struct tm tm_left, tm_the_time;
        time_t time_diff;

        cur_time = time(NULL);
        nanosleep(&delay, NULL);

        if(cur_time != prev_cur_time)
        {
            prev_cur_time = cur_time;

            time_diff = end_time - cur_time;
            (void)gmtime_r(&time_diff, &tm_left);
            (void)localtime_r(&cur_time, &tm_the_time);

            strftime(s_the_time, sizeof s_the_time,
                "%H:%M:%S",
                &tm_the_time);
                
            strftime(s_time_left, sizeof s_time_left,
                "%H:%M:%S",
                &tm_left);
                
            if(s_the_time == NULL || s_time_left == NULL)
            {
                fprintf(stderr, "Error formatting time.");
                free(tm_end_time);
                exit(EXIT_FAILURE);
            }

            disflip(s_time_left, NULL);
            /*disputs(s_the_time, NULL);*/

            /*printf("\033[14A\r");*/
        }
    }

    free(tm_end_time);
    free(tm_cur_time);
    disflip(NULL, NULL);

    /*
    for(fdisplay=0; fdisplay<14; fdisplay++)
        puts("");
    */

    alarm();

    return 0;
}
