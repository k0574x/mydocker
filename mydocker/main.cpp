#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
//#include <sys/types.h>
/* popen实现mysystem
*/

#define BUF_SIZE 8192

int my_system(char* cmd, char* result)
{
    int rc = 0;
    int ret = -1;
    char retbuf[BUF_SIZE];
    FILE* fp = NULL;

    strcat(cmd, " 2>&1");
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

int
main(int argc, char** argv)
{
    char log[BUF_SIZE] = { 0 };
    char cmd[BUF_SIZE] = { 0 };
    char result[BUF_SIZE] = { 0 };
    int cmd_ret;

    system("touch /tmp/dockercover.log");
    if (argc > 1)
    {
        strncat(cmd, "docker.cover ", BUF_SIZE - 1);
        for (int i = 1; i < argc; i++)
        {
            strncat(cmd, argv[i], BUF_SIZE - 1);
            strncat(cmd, " ", BUF_SIZE - 1);
        }
    }
    else
    {
        snprintf(cmd, BUF_SIZE, "docker -v");
    }

    snprintf(log, BUF_SIZE-1, "echo \"[%s][cmd]%s\" >> /tmp/dockercover.log", __TIME__, cmd);
    system(log);

    cmd_ret = my_system(cmd, result);

    snprintf(log, BUF_SIZE - 1, "echo \"[%s][ret]%s\" >> /tmp/dockercover.log", __TIME__, result);
    system(log);

    snprintf(cmd, BUF_SIZE-1, "echo \"%s\"", result);
    if (0 != cmd_ret) {
        //printf("failed reason :[%s]\n", cmd);
    }
    else {
        //printf("success :[%s]\n", cmd);
        system(cmd);
    }

    return cmd_ret;
}