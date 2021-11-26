/**
 * ------------------mycp-----------------------
 *	Copy source directory to destination.
 *
 *	Author: xiabee
 *	Date  : 2020.1.7
 *  Compiling environment:	Visual Studio Pro 2019
 * ---------------------------------------------
 */
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAXN 1024
WIN32_FIND_DATA lpfindfiledata;

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

int Check(int argc, char* argv[]) // 检测输入错误
{
	if (argc != 3)
	{
		printf("Invalid arguments.\n");
		printf("Usage: .\\mycp.exe <path> <path> \n");
		return -1;
	} // 参数出错

	if (FindFirstFile(argv[1], &lpfindfiledata) == INVALID_HANDLE_VALUE)
	{
		printf("Cannot Find the Directory!\n");
		return -1;
	} // 找不到路径

	int result;
	struct _stat buf;
	result = _stat(argv[1], &buf);

	if (_S_IFREG & buf.st_mode)
	{
		printf("Please Input a Directory Name!\n");
		return -1;
	}
	// 检查是否为文件夹

	if (FindFirstFile(argv[2], &lpfindfiledata) == INVALID_HANDLE_VALUE)
	{
		CreateDirectory(argv[2], NULL); //创建目标文件目录
		printf("Create Directory Succeed!\n");
	}
	// 创建新目录
	return 0;
}

int main(int argc, char* argv[])
{
	if (Check(argc, argv))
		return -1;
	// 输入不合法

	CopyDir(argv[1], argv[2]);
	//设置目录时间属性一致

	HANDLE hsource_path = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	HANDLE hdest_path = CreateFile(argv[2], GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	// 文件句柄与目录句柄
	
	FILETIME createtime, accesstime, writetime;
	GetFileTime(hsource_path, &createtime, &accesstime, &writetime);
	SetFileTime(hdest_path, &createtime, &accesstime, &writetime);
	// 修改文件时间

	SetFileAttributes(argv[2], GetFileAttributes(argv[1]));
	// 设置属性
	
	printf("Copy Completed!\n");
	return 0;
}