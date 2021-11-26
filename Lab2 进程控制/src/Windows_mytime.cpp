#include<stdio.h>
#include<windows.h>
#include<iostream>
using namespace std;


void mytime(int argc, char* argv[])
{
	LARGE_INTEGER start_time, end_time;
	LARGE_INTEGER freq;
	
	double running_time;
	//------------------------------------
	auto lpCmdline = GetCommandLine();
	auto s = lpCmdline;
	if (*s == '"') {
		++s;
		while (*s)
			if (*s++ == '"')
				break;
	}
	else {
		while (*s && *s != ' ' && *s != '\t')
			++s;
	}
	/* (optionally) skip spaces preceding the first argument */
	while (*s == ' ' || *s == '\t')
		s++;
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	BOOL status = CreateProcess(NULL, s, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (!status)
	{
		cout << "Fork Failed!" << endl;
		return;
	}

	QueryPerformanceCounter(&start_time);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	//------------------------------------
	QueryPerformanceCounter(&end_time);
	QueryPerformanceFrequency(&freq);
	running_time = (double)(end_time.QuadPart - start_time.QuadPart) / freq.QuadPart;
	printf("%lf\n", running_time);
}

int main(int argc, char* argv[])
{
	mytime(argc, argv);
	return 0;
}