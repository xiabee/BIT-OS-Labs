# 实验四 内存监视



## 一、实验目的

了解内存监视的API接口，学习系统地址的空间布局与物理内存使用情况。



## 二、实验内容

设计一个内存监视器，能实时地显示当前系统中内存的使用情况，包括系统地址空间的布局，物理内存的使用情况；能实时显示某个进程的虚拟地址空间布局和工作集信息。



## 三、程序设计与实现

项目源码及其二进制文件: `./src`

头文件与链接文件：

```c++
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <Tlhelp32.h>
#include <stdio.h>
#include <tchar.h>
#include <shlwapi.h>
#include <psapi.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib,"kernel32.lib")
```



核心函数：

```c++
void displaySystemConfig();      // 显示系统信息  
void displayMemoryCondition();   // 显示内存使用情况  
void getAllProcessInformation(); // 获取所有进程  
void getProcessDetail(int pid);  // 获取特定进程信息  
```



### 实验环境：

* 操作系统： Windows 10 专业版
* 系统类型： 64 位操作系统, 基于 x64 的处理器
* 处理器： Intel(R) Core(TM) i7-8750H CPU\@2.20GHz 2.21 GHz 12核
* 机带RAM： 32.0 GB (31.9 GB 可用)



## 四、实验结果及分析

直接运行二进制文件，显示Manger控制台：

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwskyx3sfxj30r608p0ui.jpg)





输入*`help`*，打开说明界面：

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwskzk9q5yj30rr0c4grv.jpg)



输入*`config`*，查看系统地址空间布局：

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwskzxor4qj30mf0dfdm3.jpg)

实验显示结果与实验机器属性相同，均为64位12核处理器；



输入*`condition`*，查看内存使用情况：

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwsl0omo73j30ny0gjqa2.jpg)



输入*`process`*，查看所有进程`pid`：

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwsl1ahs8sj30iy0hq7av.jpg)



分步输入*`pid`*与进程`ID`，得到特定进程的虚拟地址空间布局核工作集信息：

![image.png](https://tva1.sinaimg.cn/large/0084b03xgy1gww69e25i7j30m80dhala.jpg)




输入*`clear`*，清空输出页面：

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwsl29e33lj30o209gaf8.jpg)