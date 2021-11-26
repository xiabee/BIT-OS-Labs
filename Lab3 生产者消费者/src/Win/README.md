## Windows端实验源码说明

#### 文件列表

```
.
├── consumer.cpp
├── consumer.exe
├── master.cpp
├── master.exe
├── pc.h
├── producer.cpp
├── producer.exe
└── README.md

0 directories, 8 files
```



#### 编译环境

* `Visual Studio 2019 Pro`



#### 编译方式

* 将每个`.cpp`文件和`pc.h`拷贝至独立项目中，即3个项目共用`pc.h`



#### 运行方式

* 将`master.exe`，`producer.exe`，`consumer.exe`放入同一目录下，运行`master.exe`

