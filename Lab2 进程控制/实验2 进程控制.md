# 实验二 进程控制



## 一、实验目的

学习 `Unix` 系统的进程控制，设计并实现 `Unix` 的`time`命令，取名为`mytime`，使用 `mytime`命令创建子进程，并记录其用时。



## 二、实验内容

在 `Unix` 平台下，创建 `mytime` 命令，用 `mytime` 命令记录一个可执行程序的运行时间，用命令行参数接受其可执行程序，并为该可执行程序创建一个独立的进程。



在 `Linux` 下实现：

* 使用 `fork()`/`execv()`来创建进程并运行新的可执行程序
* 使用 `wait()`等待新创建的进程结束
* 调用 `gettimeofday()`来获取时间



在 `Windows `下实现：

* 使用 `CreateProcess()`来创建进程
* 使用`WaitForSingleObject()`在`mytime`命令中和新创建的进程之间同步
* 调用 `QueryPerformanceCounter ()`来获取时间



## 三、程序设计与实现

项目源码及其二进制文件:` ./src`

* 在 `Linux `平台下：使用`fork()`和`execv()`来创建进程并运行新的可执行程序，调用`wait()`等待新创建的进程结束，调用`gettimeofday()`来获取时间

* `Linux` 端`mytime.c`代码如下:

  ```c
  //*****************************
  // name:    Linux_mytime.c
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
  ```



* 在 `Windows` 平台下：通过`GetCommandLine()`获取当前命令行参数，传递给`mytime`，再通过`CreateProcess()`创建子进程，并通过`QueryPerformanceFrequency()`来统计子进程运行时间

* `Windows` 端`mytime.cpp`代码如下:

  ```c
  //*****************************
  // name:    Windows_mytime.cpp
  // author:  xiabee
  // time:    2020.11.29
  // version: 1.0
  //*****************************
  #include<stdio.h>
  #include<windows.h>
  #include<iostream>
  using namespace std;
  
  
  void mytime(int argc, char* argv[])
  {
  	LARGE_INTEGER start_time, end_time;
  	LARGE_INTEGER freq;
  	
  	double running_time;
  	//------------------------------------
  	auto lpCmdline = GetCommandLine();
  	auto s = lpCmdline;
  	if (*s == '"') {
  		++s;
  		while (*s)
  			if (*s++ == '"')
  				break;
  	}
  	else {
  		while (*s && *s != ' ' && *s != '\t')
  			++s;
  	}
  	/* (optionally) skip spaces preceding the first argument */
  	while (*s == ' ' || *s == '\t')
  		s++;
  	STARTUPINFO si = { sizeof(si) };
  	PROCESS_INFORMATION pi;
  
  	BOOL status = CreateProcess(NULL, s, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
  	if (!status)
  	{
  		cout << "Fork Failed!" << endl;
  		return;
  	}
  
  	QueryPerformanceCounter(&start_time);
  	WaitForSingleObject(pi.hProcess, INFINITE);
  	CloseHandle(pi.hProcess);
  	CloseHandle(pi.hThread);
  
  	//------------------------------------
  	QueryPerformanceCounter(&end_time);
  	QueryPerformanceFrequency(&freq);
  	running_time = (double)(end_time.QuadPart - start_time.QuadPart) / freq.QuadPart;
  	printf("%lf\n", running_time);
  }
  
  int main(int argc, char* argv[])
  {
  	mytime(argc, argv);
  	return 0;
  }
  ```





## 四、实验结果及分析

### Linux

```bash
gcc -o mytime Linux-mytime.c # 编译生成可执行文件
./mytime /bin/ls # 创建不带参数的子进程
./mytime /bin/sleep 2 # 创建带参数的子进程
```

实验结果如下：![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwskblcvvmj30x608bn2f.jpg)



结果分析：

* 在执行` ./mytime /bin/ls` 时，`mytime` 仅接收到一个参数，直接传入` execv `中，创建子进程` /bin/ls`，列举当前目录下的文件，并计算执行时间；

* 在执行 `./mytime /bin/sleep 2` 时，`mytime` 接收到两个参数，将`sleep`和 `2` 同时传入`execv`中，创建子进程执行 `sleep 2` ，延时两秒，计算运行时间。

* 在输出结果中，调用子进程消耗一定时间，故`./mytime /bin/sleep 2 `执行时间略大于` 2 `秒。

  

### Windows

由于 `Windows` 机器中没有与 `Linux` 的`/bin/sleep`相似的延时程序，我们自行编写了一个`sleep.cpp`，编译生成`sleep.exe`

```c
#include <iostream>
#include <windows.h>
int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        printf("Please input time!\n");
        return 0;
    }
    Sleep(strtod(argv[1],0) * 1000.0);
    return 0;
}
```



执行以下命令：

```bash
.\mytime.exe notepad
.\mytime.exe sleep 2
```

实验结果如下：

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwskf30sjbj30m503pabc.jpg)



结果分析：

* 执行命令 1 时，`mytime.exe` 接收到了一个参数 `notepad`，传入 CreateProcess创建新进程`notepad`，并记录用时；
* 执行命令 2 时，`mytime.exe `接收到了两个参数,分别为` sleep` 和 `2`，创建新进程` sleep 2`，延时两秒，记录用时。
* 在输出结果中，调用子进程消耗一定时间，故`.\mytime.exe sleep 2`执行时间略大于 `2` 秒；同时 `Windows` 端的 `Sleep `延时不精确，有一定误差。

