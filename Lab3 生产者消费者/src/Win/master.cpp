// Name:    Master
// Auther:  xiabee
// Time:    2020.12.21
#include <windows.h>
#include <stdio.h>
#include "pc.h"

int main()
{
    HANDLE hMapFile;
    BOOL result;
    DWORD pid = GetCurrentProcessId();
    // shared memory
    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,  // use paging file
        NULL,                  // default security
        PAGE_READWRITE,        // read/write access
        0,                     // maximum object size (high-order DWORD)
        sizeof(struct mybuffer), // maximum object size (low-order DWORD)
        szFileMappingName);    // name of mapping object

    if (hMapFile == NULL)
    {
        printf("Mapping Failed!\n");
        return 1;
    }
    // mutex
    HANDLE hMutex = CreateMutex(NULL, FALSE, szMutexName);
    if (hMutex == NULL)
    {
        printf("Mutex Failed!\n");
        return 1;
    }
    // empty
    HANDLE hSemaphoreEmpty = CreateSemaphore(NULL, 3, 3, szSemaphoreEmptyName);
    if (hSemaphoreEmpty == NULL)
    {
        printf("Empty Failed!\n");
        return 1;
    }
    // full
    HANDLE hSemaphoreFull = CreateSemaphore(NULL, 0, 3, szSemaphoreFullName);
    if (hSemaphoreFull == NULL)
    {
        printf("Full Failed!\n");
        return 1;
    }
    // attach & zero the memory
    struct buffer* pBuf = (struct buffer*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, MYBUF_LEN);
    if (pBuf == NULL)
    {
        printf("View Failed\n");
        CloseHandle(hMapFile);
        return 1;
    }
    memset(pBuf, 0, sizeof(struct mybuffer));
    UnmapViewOfFile(pBuf);
    pBuf = NULL;


    PROCESS_INFORMATION pi[pro_max + con_max] = { 0 };
    STARTUPINFO si[pro_max + con_max] = { 0 };
    for (int i = 0; i < pro_max + con_max; i++)
    {
        si[i].cb = sizeof(STARTUPINFO);
    }
    /* start producer */
    TCHAR ProducerName[] = TEXT("producer.exe");
    TCHAR ConsumerName[] = TEXT("consumer.exe");
    for (int i = 0; i < pro_max; i++)
    {
        result = CreateProcess(NULL, ProducerName,
            NULL, NULL, TRUE,
            NORMAL_PRIORITY_CLASS,
            NULL, NULL, &si[i], &pi[i]);
        if (!result) // fail
        {
            printf("Could not create producer process.\n");
            return 1;
        }
    }

    /* start consumer */
    for (int i = pro_max; i < pro_max + con_max; i++)
    {
        result = CreateProcess(NULL, ConsumerName,
            NULL, NULL, TRUE,
            NORMAL_PRIORITY_CLASS,
            NULL, NULL, &si[i], &pi[i]);
        if (!result) // fail
        {
            printf("Could not create consumer process.\n");
            return 1;
        }
    }
    HANDLE hProcesses[pro_max + con_max];
    DWORD ExitCode;
    for (int i = 0; i < pro_max + con_max; i++)
    {
        hProcesses[i] = pi[i].hProcess;
    }
    // wait...
    WaitForMultipleObjects(pro_max + con_max, hProcesses, TRUE, INFINITE);
    printf("Master Exit!\n");
    for (int i = 0; i < pro_max + con_max; i++)
    {
        if (pi[i].hProcess == 0)
            exit(-1);
        result = GetExitCodeProcess(pi[i].hProcess, &ExitCode);
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }
    CloseHandle(hMapFile);
    return 0;
}