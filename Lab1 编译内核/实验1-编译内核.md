# 实验一 编译Linux内核



## 一、实验目的

学习重新编译 Linux 内核，理解、掌握 Linux 内核和发行版本的区别。



## 二、实验内容

在` Ubuntu 2004` 系统中，下载并重新编译其内核源代码（版本号 5.9.9）；

然后，配置 GNU 的启动引导工具 grub，成功运行重新编译成功的 Linux 内核。



## 三、实验步骤

### 1.下载内核:

```bash
wget https://mirrors.edge.kernel.org/pub/linux/kernel/v5.x/linux-5.9.9.tar.xz
sudo cp ./linux-5.9.9.tar.xz /usr/src
cd /usr/src/
```



### 2.解压:

```bash
sudo tar xvJf ./linux-5.9.9.tar.xz
cd linux-5.9.9
```



### 3.安装依赖

```bahs 
sudo apt-get install libncurses5-dev build-essential kernel-package
```



### 4.编译

为了提高速度，这里使用` 12 `线程同时编译（虚拟机分配了12个处理器）

```bash
sudo make -j 12
sudo make modules_install -j 12
sudo make install -j 12
```



## 四、实验结果及分析

实验前内核：Linux ubuntu 5.4.0-56-generic

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwsjx63ngyj30se06btc3.jpg)



实验前发行版：Ubuntu 9.3.0-17ubuntu1~20.04 

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwsjxgviznj311h07f43o.jpg)



实验后内核：Linux ubuntu 5.9.9

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwsjy622v2j310o06cacx.jpg)



实验后机器 GRUB 引导界面：

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwsjzguucoj30xw0nmn1d.jpg)