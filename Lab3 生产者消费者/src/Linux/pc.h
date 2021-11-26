// Name:    Producer and Consumer.h
// Auther:  xiabee
// Time:    2020.12.17

#ifndef _PC_H
#define _PC_H

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <stdlib.h>

//2个生产者，每个生产者重复12次
#define pro_max 2
#define pro_rep 12
 
//3个消费者，每个消费者重复8次
#define con_max 3
#define con_rep 8
 
//缓冲区为6
#define buffer_len 11
#define buffer_cnt 6
 
#define MYBUF_LEN (sizeof(struct mybuffer))
 
#define SHM_MODE 0600//可读可写
#define SEM_ALL_KEY 1234
#define SHM_ALL_KEY 1235
#define SEM_EMPTY 0
#define SEM_FULL 1
#define SEM_MUTEX 2

//缓冲区的结构
struct mybuffer
{
    char str[buffer_cnt][buffer_len];
    int head;
    int tail;
};

#endif