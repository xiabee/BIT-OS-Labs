#include "pc.h"

// 生成一个随机数
int myrandom(int mod)
{
    int sand;
    sand = rand() % mod;
    return sand;
}

// 随机生成一个字符串
char* ca()
{
    static char le[buffer_len];
    memset(le, 0, sizeof(le));
    int n = myrandom(10) + 1;
    for (int i = 0; i < n; i++)
        le[i] = (char)(rand() % 26 + 'A');
    return le;
}

int main()
{
	HANDLE hMapFile;
	struct mybuffer* pBuf;
	int pid = GetCurrentProcessId();
	srand(pid);

	//shm
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
    for (int i = 0; i < pro_rep; i++)
    {
        QueryPerformanceCounter(&start_time);
        SleepTime = rand() % 1000;
        // p(empty)
        WaitForSingleObject(hSemaphoreEmpty, INFINITE);
        // p(mutex)
        WaitForSingleObject(hMutex, INFINITE);
        // sleep
        Sleep(SleepTime);

        // produce a sentence
        char* s = pBuf->str[pBuf->tail];
        strcpy_s(s, buffer_len, ca());
        pBuf->tail = (pBuf->tail + 1) % buffer_cnt;

        printf("Pro push %-10s ", s);
        
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
        // v(full)
        ReleaseSemaphore(hSemaphoreFull, 1, NULL);
    }

    // release resources
    CloseHandle(hSemaphoreEmpty);
    CloseHandle(hSemaphoreFull);
    CloseHandle(hMutex);
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    return 0;

}