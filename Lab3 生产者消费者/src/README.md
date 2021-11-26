## 实验源码说明

#### 文件列表

```
.
├── Linux
│   ├── master
│   ├── master.c
│   └── pc.h
├── README.md
└── Win
    ├── consumer.cpp
    ├── consumer.exe
    ├── master.cpp
    ├── master.exe
    ├── pc.h
    ├── producer.cpp
    ├── producer.exe
    └── README.md

2 directories, 12 files
```



#### 编译环境

| 平台    | 编译环境                                |
| ------- | --------------------------------------- |
| Linux   | `gcc version 10.2.0 (Debian 10.2.0-19)` |
| Windows | `Visual Studio 2019 Pro`                |



#### Windows 端编译方式

* 将每个`.cpp`文件和`pc.h`拷贝至独立项目中，即3个项目共用`pc.h`



#### Windows 端运行方式

* 将`master.exe`，`producer.exe`，`consumer.exe`放入同一目录下，运行`master.exe`



#### Linux端 编译运行方式

```
gcc -o master master.c
./master
```

