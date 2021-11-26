//*****************************
// name:    Linux_mytime.cpp
// author:  xiabee
// time:    2020.11.29
// version: 1.0
//*****************************

#include <stddef.h>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>

void mytime(int argc,char *argv[])
{
    struct timeval start_time;
    struct timeval end_time;
    // 精确到微秒
    double running_time = 0;
    
    pid_t pid;
    
    if(argc<=1)
    {
        printf("Please input the process name!\n");
        return;
    }

    pid = fork();
    // 获取进程pid

    if (pid < 0)
    {
        perror("fork failed!\n");
        exit(1);
    }
    // 进程出错，fork失败

    if(pid == 0) //返回子进程
    {
        // printf("Create Child:\n");
        gettimeofday(&start_time, NULL);
        
        // execvp(argv[1], &argv[1]);
        execv(argv[1], &argv[1]);
        
        printf("process name: %s\t pid:%d\n", argv[1], getpid());
    } 
    else
    {
        int status;
        gettimeofday(&start_time, NULL);
        wait(&status);
        gettimeofday(&end_time, NULL);
        running_time = (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec);
        printf("running time: %lf ms\n", running_time / 1000);
    }
}

int main(int argc,char *argv[])
{
    mytime(argc, argv);
    return 0;
}