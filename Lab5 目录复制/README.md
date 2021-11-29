# 实验五 目录复制



## 一、实验目的

学习模拟`cp`与`copy`命令的程序编写，完成一个目录复制命令`mycp`，包括目录下的文件和子目录。



## 二、实验内容

分别在`Windows`与`Linux`平台上完成文件复制命令：

* `Windows`相关`API`：`CreateDirectory`,`FindFirstFile`,`FindNextFile`，要求复制原文件的权限、时间等；
* `Linux`相关`API`：`mkdir`,`opendir`,`readdir`,`symlink`,`readlink`，要求复制原文件的权限、时间等，要求能够复制`软链接`。



要求实现结果如下：

```bash
beta@bugs.com [~]# ls --l sem  
total 56  
drwxr-xr-x  3 beta beta 4096 Dec 19 02:53 ./  
drwxr-xr-x  8 beta beta 4096 Nov 27 08:49 ../  
-rw-r--r--  1 beta beta  128 Nov 27 09:31 Makefile  
-rwxr-xr-x  1 beta beta 5705 Nov 27 08:50 consumer\*  
-rw-r--r--  1 beta beta  349 Nov 27 09:30 consumer.c  
drwxr-xr-x  2 beta beta 4096 Dec 19 02:53 subdir/  
beta@bugs.com [~]# mycp sem target  
beta@bugs.com [~]# ls --l target  
total 56  
drwxr-xr-x  3 beta beta 4096 Dec 19 02:53 ./  
drwxr-xr-x  8 beta beta 4096 Nov 27 08:49 ../  
-rw-r--r--  1 beta beta  128 Nov 27 09:31 Makefile  
-rwxr-xr-x  1 beta beta 5705 Nov 27 08:50 consumer\*  
-rw-r--r--  1 beta beta  349 Nov 27 09:30 consumer.c  
drwxr-xr-x  2 beta beta 4096 Dec 19 02:53 subdir/  
```





## 三、程序设计与实现

项目源码及其二进制文件: `./src`

### 编译环境:

* Windows: Visual Studio Pro 2019

* Linux: gcc version 10.2.1 20201207 (Debian 10.2.1-1)



### 程序内容:

### Windows:

检测文件属性，目录内的普通文件直接复制，子目录文件与子文件先创建新的子目录再复制。

Windows核心函数：

复制文件：复制文件的内容、时间、权限等

```c++
void CopyFile(char* source_file, char* dest_file) //复制文件
{
	WIN32_FIND_DATA lpfindfiledata;
	HANDLE hfindfile = FindFirstFile(source_file, &lpfindfiledata);
	HANDLE hsource = CreateFile(source_file, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hdest_file = CreateFile(dest_file, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	// 文件句柄与目录句柄

	LONG size = lpfindfiledata.nFileSizeLow - lpfindfiledata.nFileSizeHigh;
	int* buffer = new int[size];
	DWORD temp;
	bool tmp = ReadFile(hsource, buffer, size, &temp, NULL);
	WriteFile(hdest_file, buffer, size, &temp, NULL);
	// 复制文件时间

	SetFileTime(hdest_file, &lpfindfiledata.ftCreationTime, &lpfindfiledata.ftLastAccessTime, &lpfindfiledata.ftLastWriteTime);
	
	SetFileAttributes(dest_file, GetFileAttributes(source_file));
	// 设置文件属性

	CloseHandle(hfindfile);
	CloseHandle(hsource);
	CloseHandle(hdest_file);
	// 关闭句柄
}
```

 

复制目录: 区别普通文件与目录文件

```c++
void CopyDir(char* source_file, char* dest_file) //复制目录
{
	WIN32_FIND_DATA lpfindfiledata;
	char source[MAXN], dest_path[MAXN];
	strcpy_s(source, source_file);
	strcpy_s(dest_path, dest_file);
	strcat_s(source, "\\*.*");
	strcat_s(dest_path, "\\");
	HANDLE hfindfile = FindFirstFile(source, &lpfindfiledata);
	while (FindNextFile(hfindfile, &lpfindfiledata) != 0) //遍历文件
	{
		if (lpfindfiledata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //目录文件
		{
			if (strcmp(lpfindfiledata.cFileName, ".") != 0 && strcmp(lpfindfiledata.cFileName, "..") != 0)
			{
				memset(source, 0, sizeof(source));
				strcpy_s(source, source_file);
				strcat_s(source, "\\");
				strcat_s(source, lpfindfiledata.cFileName);
				strcat_s(dest_path, lpfindfiledata.cFileName);
				CreateDirectory(dest_path, NULL);
				CopyDir(source, dest_path);
				//复制文件时间
				HANDLE hsource_path = CreateFile(source, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
				HANDLE hdest_path = CreateFile(dest_path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
				FILETIME createtime, accesstime, writetime;
				GetFileTime(hsource_path, &createtime, &accesstime, &writetime);
				SetFileTime(hdest_path, &createtime, &accesstime, &writetime);
				SetFileAttributes(dest_path, GetFileAttributes(source));
				strcpy_s(dest_path, dest_file);
				strcat_s(dest_path, "\\");
			}
		}
		else //若目标为文件
		{
			memset(source, 0, sizeof(source));
			strcpy_s(source, source_file);
			strcat_s(source, "\\");
			strcat_s(source, lpfindfiledata.cFileName);
			strcat_s(dest_path, lpfindfiledata.cFileName);
			CopyFile(source, dest_path);
			strcpy_s(dest_path, dest_file);
			strcat_s(dest_path, "\\");
		}
	}
	CloseHandle(hfindfile);
}
```



### Linux: 

检测文件属性，目录内的普通文件直接复制，子目录文件与子文件先创建新的子目录再复制，软链接文件单独复制。



Linux核心函数：

复制软链接:

```c
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
```



复制文件与复制目录基本与`Windows`相同



## 四、实验结果及分析

### `Windows` 

原文件夹**test1**, 目标文件夹 **test2**



在控制台输入错误命令: `.\\mycp.exe` ,会提示参数错误：

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwsljar3pej30i604j40z.jpg)



复制不存在的文件夹: 提示文件夹不存在

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwsllwx7t3j30o1056whx.jpg)



直接复制文件: 提示不能复制文件

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwslm5noozj30o202sdi5.jpg)



复制文件夹: 目标文件夹不存在时，先创建文件夹再复制；目标文件夹存在时，不创建文件夹直接复制

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwslmf0crhj30o107caem.jpg)



**Windows程序执行结果:**

子目录全部复制: 原文件夹**test1**, 目标文件夹 **test2**

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwslmtiby9j30if07ujtk.jpg)



目录内文件内容全部复制、时间与属性全部复制: 原文件夹**test1**, 目标文件夹 **test2**

被复制的原文件夹`test1`:

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwsln5z4z7j30ru05q75z.jpg)



复制后的目标文件夹`test2`:

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwslopqbfnj30su062jt9.jpg)





### `Linux`

原文件夹**test1**, 目标文件夹 **test2**

错误信息处理同Windows:无效参数、直接复制文件、复制不存在的文件夹：

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwslpewytoj30m008hjw6.jpg)





**Linux程序执行结果:**

子目录、普通文件、软链接全部复制: 原文件夹**test1**, 目标文件夹 **test2**

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwslpk230jj30l708v0uu.jpg)





目录内文件内容、时间、权限全部复制:

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwslprtc0bj30o208s44m.jpg)





原文件夹时间与权限被复制:

![image.png](https://tva1.sinaimg.cn/large/0084b03xly1gwslpxx6jqj30o1051gp4.jpg)