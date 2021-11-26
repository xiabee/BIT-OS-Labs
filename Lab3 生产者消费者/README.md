# 实验三 生产者消费者



## 一、实验目的

编程模拟生产者消费者问题，深入了解`P`、`V`操作相关机制与代码构成。



## 二、 实验内容

分别在`Windows`和`Linux`平台下创建一个有`6`个缓冲区的缓冲池，初始为空，每个缓冲区能存放一个长度若为`10`个字符的字符串。创建两个生产者进程和三个消费者进程

* 2个生产者进程

  * 随机等待一段时间，往缓冲区添加数据
  * 若缓冲区已满，等待消费者取走数据后再添加
  * 重复12次

  

* 3个消费者进程
  * 随机等待一段时间，从缓冲区读取数据
  * 若缓冲区为空，等待生产者添加数据后再读取
  * 重复8次



## 三、程序设计与实现

项目源码及其二进制文件: `./src`

源码及其二进制文件运行说明: `./src/README.md`

### Linux

仅有一个二进制文件`master`，在运行时分别创建生产者和消费者子进程，通过信号量和共享内存实现共享缓冲区的模拟。

```c
//缓冲区的结构
struct mybuffer
{
    char str[buffer_cnt][buffer_len];
    int head;
    int tail;
};
```



信号量P、V操作：自定义P、V相关函数

```c
// P 操作
void P(int sem_id, int sem_num)
{
    struct sembuf xiabee;
    xiabee.sem_num = sem_num;
    // 信号量索引
    xiabee.sem_flg = 0;
    // 访问标志
    xiabee.sem_op = -1;
    // 信号量操作值
    semop(sem_id, &xiabee, 1);
    //一次需进行的操作的数组sembuf中的元素数为1
}

// V 操作
void V(int sem_id, int sem_num)
{
    struct sembuf xiabee;
    xiabee.sem_num = sem_num;
    // 信号量索引
    xiabee.sem_flg = 0;
    // 访问标志
    xiabee.sem_op = 1;
    // 信号量操作值
    semop(sem_id, &xiabee, 1);
    //一次需进行的操作的数组sembuf中的元素数为1
}
```



核心操作：

* 生产者随机生成字符串，写入缓冲区`mybuffer`结构体的`str[i]`字符串中；消费者清除对应缓冲区，将`str[j]`置零，即`memset(str[j],0, sizeof(str[j]))`
* 计时器：使用`clock_gettime`进行高精度计时
* 输出：
  * 生产者：输出生产者产生的当前字符串、当前时刻缓冲区的映像、本次生产所用时间；
  * 消费者：输出消费者消费的当前字符串、当前时刻缓冲区的映像、本次消费所用时间。



### Windows

有三个二进制文件，分别为: `master.exe`, `producer.exe`, `consumer.exe`，通过主文件`master.exe`调用生产者和消费者的二进制文件，创建子进程。



核心数据结构：

```c
// 定义共享内存相关信息
const TCHAR szFileMappingName[] = TEXT("PCFileMappingObject");
const TCHAR szMutexName[] = TEXT("PCMutex");
const TCHAR szSemaphoreEmptyName[] = TEXT("PCSemaphoreEmpty");
const TCHAR szSemaphoreFullName[] = TEXT("PCSemaphoreFull");

//缓冲区的结构
struct mybuffer
{
    char str[buffer_cnt][buffer_len];
    // 6个缓冲区，每个缓冲区中存放一个十位以内的字符串
    int head;
    int tail;
};
```



信号量`P`、`V`操作：利用`Windows`原生`API`

```c
WaitForSingleObject(hSemaphoreFull, INFINITE); 
ReleaseSemaphore(hSemaphoreEmpty, 1, NULL); 
```



核心操作：

* 生产者随机生成字符串，写入缓冲区`mybuffer`结构体的`str[i]`字符串中；消费者清除对应缓冲区，将`str[j]`置零，即`memset(str[j],0, sizeof(str[j]))`
* 计时器：使用`QueryPerformanceFrequency`进行高精度计时
* 输出：
  * 生产者：输出生产者产生的当前字符串、当前时刻缓冲区的映像、本次生产所用时间
  * 消费者：输出消费者消费的当前字符串、当前时刻缓冲区的映像、本次消费所用时间



## 四、实验结果及分析

### Linux

```bash
gcc -o master master.c # 编译生成可执行文件
./master # 运行
```



实验结果如下：

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwskr58e6wj30tk0loh4h.jpg)

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwskscni7vj30ts04ejtn.jpg)

结果分析：

在本次实验中，共享内存区域被MUTEX, FULL, EMPTY等信号量限制，程序逻辑正常，没有出现缓冲区读脏数据、重复写入、死锁等不佳现象；缓冲区的写入与读出亦控制较好。本次实验运行成功，符合实验预期。



### Windows

分别编译`master`, `producer`, `consumer`，运行`master`

实验结果如下：

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwsksoj8lbj30m60iwdtd.jpg)



![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwskswuouyj30m603jmyo.jpg)



结果分析：

结果同Linux，共享内存区域被信号量限制，程序逻辑未出现异常，没有出现缓冲区读脏数据、重复写入、死锁等不佳现象；缓冲区的写入与读出亦控制较好。本次实验运行成功，符合实验预期。