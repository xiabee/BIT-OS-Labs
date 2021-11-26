// Name:    Consumer
// Auther:  xiabee
// Time:    2020.12.21
#include "pc.h"

int main()
{
    HANDLE	hMapFile;
    struct mybuffer* pBuf;
    int pid = GetCurrentProcessId();
    srand(pid);
    // shm
    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS, // read/write access
        FALSE,               // do not inherit the name
        szFileMappingName);  // name of mapping object

    // attach shm
    if (hMapFile == NULL)
    {
        printf("Mapping Failed!\n");
        return 1;
    }

    pBuf = (struct mybuffer*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, MYBUF_LEN);
    if (pBuf == NULL)
    {
        printf("View Failed!\n");
        CloseHandle(hMapFile);
        return 1;
    }

    // mutex
    HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, szMutexName);
    if (hMutex == NULL)
    {
        printf("Mutex Failed!\n");
        return 1;
    }
    // empty
    HANDLE hSemaphoreEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, szSemaphoreEmptyName);
    if (hSemaphoreEmpty == NULL)
    {
        printf("Emtpy Failed!\n");
        return 1;
    }
    // full
    HANDLE hSemaphoreFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, szSemaphoreFullName);
    if (hSemaphoreFull == NULL)
    {
        printf("Full Failed!\n");
        return 1;
    }

    int SleepTime;
    for (int i = 0; i < con_rep; i++)
    {
        QueryPerformanceCounter(&start_time);
        SleepTime = rand() % 1000;
        // p(full)
        WaitForSingleObject(hSemaphoreFull, INFINITE);
        // p(mutex)
        WaitForSingleObject(hMutex, INFINITE);

        // consume a sentence from the shm
        char* s = pBuf->str[pBuf->head];
        // sleep
        Sleep(SleepTime);

        printf("Con pop  %-10s ", pBuf->str[pBuf->head]);
        memset(s, 0, sizeof(pBuf->str[pBuf->head]));

        pBuf->head = (pBuf->head + 1) % buffer_cnt;
        
        // print buffers
        for (int cnt = 0; cnt < buffer_cnt; cnt++)
            printf("|%-10s", pBuf->str[cnt]);
        printf("|");

        

        QueryPerformanceCounter(&end_time);
        QueryPerformanceFrequency(&freq);
        running_time = (double)(end_time.QuadPart - start_time.QuadPart) / freq.QuadPart;
        printf(" running time:%lf ms\n", running_time);
        // v(mutex)
        ReleaseMutex(hMutex);
        // v(empty)
        ReleaseSemaphore(hSemaphoreEmpty, 1, NULL);
    }

    // release resources
    CloseHandle(hSemaphoreEmpty);
    CloseHandle(hSemaphoreFull);
    CloseHandle(hMutex);
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    return 0;
}