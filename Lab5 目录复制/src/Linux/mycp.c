/**
 * ------------------mycp-----------------------
 *	Copy source directory to destination.
 *
 *	Author: xiabee
 *	Date  : 2020.1.7
 *  Compiling environment:	gcc version 10.2.1 20201207 (Debian 10.2.1-1)
 * ---------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <utime.h>
#include <sys/time.h>
#include <fcntl.h>

#define MAXN 1024

void CopySoftLink(char *fsource, char *ftarget) //复制软链接
{
    char buffer[2 * MAXN];
    char oldpath[MAXN];
    getcwd(oldpath, sizeof(oldpath));
    strcat(oldpath, "/");
    memset(buffer, 0, sizeof(buffer));
    readlink(fsource, buffer, 2 * MAXN);
    symlink(buffer, ftarget);
    struct stat statbuf;
    lstat(fsource, &statbuf);
    //复制时间属性
    struct timeval ftime[2];
    ftime[0].tv_usec = 0;
    ftime[0].tv_sec = statbuf.st_atime;
    ftime[1].tv_usec = 0;
    ftime[1].tv_sec = statbuf.st_mtime;
    lutimes(ftarget, ftime);
}

void CopyFile(char *fsource, char *ftarget) // 直接复制
{
    int fd = open(fsource, 0); //打开文件，文件描述符
    int fdr;                   //目标文件描述符

    struct stat statbuf;
    struct utimbuf timeby;
    char BUFFER[MAXN]; //新开缓冲区，保存数据
    int wordbit;
    stat(fsource, &statbuf);
    fdr = creat(ftarget, statbuf.st_mode); //创建新文件,返回文件描述符

    while ((wordbit = read(fd, BUFFER, MAXN)) > 0) //读取源文件字节数>0
    {
        //写入目标文件
        if (write(fdr, BUFFER, wordbit) != wordbit)
        {
            printf("Write Error!\n");
            exit(-1);
        }
    }

    timeby.actime = statbuf.st_atime; //修改时间属性
    timeby.modtime = statbuf.st_mtime;
    utime(ftarget, &timeby);
    close(fd); //关闭文件
    close(fdr);
}

void Mycp(char *fsource, char *ftarget) // 将源目录信息复制到目标目录下
{
    char source[MAXN / 2];
    char target[MAXN / 2];
    struct stat statbuf;
    struct utimbuf timeby;
    DIR *dir;
    struct dirent *entry;

    strcpy(target, ftarget);

    strcat(target, "/");

    dir = opendir(fsource); //打开目录,返回指向DIR结构的指针
    //refer to P215
    if (NULL == dir)
    {
        printf("Open Dir Error!\n");
        exit(-1);
    }

    while ((entry = readdir(dir)) != NULL) //读目录
    {
        if (entry->d_type == 4) // 目录文件
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) //判断目录
                continue;

            memset(source, 0, sizeof(source));
            strcpy(source, fsource);

            strcat(source, "/");
            strcat(source, entry->d_name);
            strcat(target, entry->d_name);
            //相当于windows中CreateFileD函数功能

            stat(source, &statbuf);         //统计文件属性信息
            mkdir(target, statbuf.st_mode); //创建目标目录

            Mycp(source, target);

            timeby.actime = statbuf.st_atime;
            timeby.modtime = statbuf.st_mtime; //修改文件存取和修改时间
            utime(target, &timeby);

            strcpy(target, ftarget);
            strcat(target, "/");
        }

        else if (entry->d_type == 10) // 软链接文件
        {

            memset(source, 0, sizeof(source));
            strcpy(source, fsource);
            strcat(source, "/");
            strcat(source, entry->d_name);
            strcat(target, entry->d_name);
            CopySoftLink(source, target);
            strcpy(target, ftarget);
            strcat(target, "/");
        }

        else // 普通文件
        {
            memset(source, 0, sizeof(source));
            strcpy(source, fsource);
            strcat(source, "/");
            strcat(source, entry->d_name);
            strcat(target, entry->d_name);
            CopyFile(source, target);
            strcpy(target, ftarget);
            strcat(target, "/");
        }
    }
    closedir(dir);
}

int Check(int argc, char *argv[], struct stat statbuf) // 检测输入与目标文件是否有误
{
    DIR *dir;
    
    if (argc != 3)         //参数出错
    {
        printf("Invalid arguments.\n");
		printf("Usage: ./mycp <path> <path> \n");
        return -1;
    }

    int result;
	result = lstat(argv[1], &statbuf);
	if (S_IFREG & statbuf.st_mode)
	{
		printf("Please Input a Directory Name!\n");
		return -1;
	}
	// 检查是否为文件夹

    if ((dir = opendir(argv[1])) == NULL)
    {
        printf("Source Dir does not exist.\n"); //源文件打开出错
        return -1;
    }

    if ((dir = opendir(argv[2])) == NULL)
    {
        mkdir(argv[2], statbuf.st_mode); //创建目录
        printf("Make Dir Succeed!\n");
    }
    closedir(dir);
    return 0;
}

              
//DIR结构的指针，指向目录的第一个文件

int main(int argc, char *argv[])
{
    struct stat statbuf;   //stat结构
    struct utimbuf timeby; //文件时间结构

    if(Check(argc, argv, statbuf))
        return -1;
    
    Mycp(argv[1], argv[2]); //开始复制
    
    stat(argv[1], &statbuf);
    timeby.actime = statbuf.st_atime;  //修改时间属性，存取时间
    timeby.modtime = statbuf.st_mtime; //修改时间
    utime(argv[2], &timeby);
    printf("Copy Finished!\n");

    return 0;
}