#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
/* popen实现mysystem
*/

#define BUF_SIZE 8192

void getNameByPid(pid_t pid, char* task_name) {
    char proc_pid_path[BUF_SIZE];
    char buf[BUF_SIZE];

    sprintf(proc_pid_path, "/proc/%d/status", pid);
    FILE* fp = fopen(proc_pid_path, "r");
    if (NULL != fp) {
        if (fgets(buf, BUF_SIZE - 1, fp) == NULL) {
            fclose(fp);
        }
        fclose(fp);
        sscanf(buf, "%*s %s", task_name);
    }
}

int my_system(char* cmd, char* result)
{
    int rc = 0;
    int ret = -1;
    char retbuf[BUF_SIZE];
    FILE* fp = NULL;

    strcat(cmd, " 2>&1");
    *result = 0;
    do {
        fp = popen(cmd, "r");
        if (NULL == fp) {
            perror("popen error\n");
            break;
        }
        while (NULL != fgets(retbuf, BUF_SIZE, fp)) {
            strncat(result, retbuf, BUF_SIZE - 1);
        }
        rc = pclose(fp);
        if (-1 == rc) {
            perror("pclose error\n");
            break;
        }
        if (!WIFEXITED(rc)) {
            perror("Run command failed\n");
            break;
        }
        else {
            ret = WEXITSTATUS(rc);
        }
    } while (0);

    if (NULL == fp || -1 == rc) {
        strncpy(result, strerror(errno), BUF_SIZE);
    }
    fp = NULL;
    return ret;
}

char* logtime()
{
    static char logtm[BUF_SIZE] = { 0 };
    time_t t_time;
    struct tm* tm_ptr = NULL;
    time(&t_time);
    tm_ptr = localtime(&t_time);
    //2017-12-21 18:53:58
    strftime(logtm, BUF_SIZE, "%Y-%m-%d %H:%M:%S", tm_ptr);
    //strftime(logtm, BUF_SIZE, "%Y-%m-%d --- %H:%M:%S", tm_ptr);

    return logtm;
}

#define COVER_LOG_PATH  "/tmp/cover"
int
main(int argc, char** argv)
{
    char name[BUF_SIZE] = { 0 };
    char logfile[BUF_SIZE] = { 0 };
    char log[BUF_SIZE] = { 0 };
    char cmd[BUF_SIZE] = { 0 };
    char result[BUF_SIZE] = { 0 };
    int cmd_ret;

    //snprintf(cmd, BUF_SIZE - 1, "ps -f $PPID | awk '{if (NR==2) print $3}'");
    //my_system(cmd, result);
    //printf("ppid=%d,result=%s", getppid(), result);

    mkdir(COVER_LOG_PATH, 777);
#if 0
    getNameByPid(getppid(), name);
#else
    snprintf(cmd, BUF_SIZE - 1, "head -n 7 /proc/%d/status | awk '{if (NR==5) print $2}'", getppid());
    my_system(cmd, result);
    //printf("pppid=%s", result);
    getNameByPid(atoi(result), name);
    //printf("pppidname=%s", name);
#endif
    snprintf(logfile, BUF_SIZE - 1, "%s/cover_%s.log", COVER_LOG_PATH, name);

    snprintf(cmd, BUF_SIZE - 1, "touch %s", logfile);
    system(cmd);

    //snprintf(cmd, BUF_SIZE - 1, "cat /proc/%d/cmdline", getpid());
    //my_system(cmd, result);
    //printf("pppid=%s", result);

    if (argc > 1)
    {
        snprintf(cmd, BUF_SIZE - 1, "/home/sysadm/docker ");
        for (int i = 1; i < argc; i++)
        {
            if (strrchr(argv[i], ' '))
            {
                strncat(cmd, "\"", BUF_SIZE - 1);
                strncat(cmd, argv[i], BUF_SIZE - 1);
                strncat(cmd, "\" ", BUF_SIZE - 1);
            }
            else
            {
                strncat(cmd, argv[i], BUF_SIZE - 1);
                strncat(cmd, " ", BUF_SIZE - 1);
            }
        }
    }
    else
    {
        snprintf(cmd, BUF_SIZE -1, "docker -v");
    }
    
    snprintf(log, BUF_SIZE - 1, "echo \"[%-1s][%s][cmd]%s\" >> %s", logtime(), name, cmd, logfile);
    system(log);

    cmd_ret = my_system(cmd, result);

    snprintf(log, BUF_SIZE - 1, "echo \"[%s][%s][ret]%s\" >> %s", logtime(), name, result, logfile);
    system(log);

    if (result[strlen(result) - 1] == '\n')
    {
        result[strlen(result) - 1] = '\0';
    }
    snprintf(cmd, BUF_SIZE-1, "echo \"%s\"", result);
    if (0 != cmd_ret) {
        system(cmd);
        //printf("failed reason :[%s]\n", cmd);
    }
    else {
        //printf("success :[%s]\n", cmd);
        system(cmd);
    }

    return cmd_ret;
}