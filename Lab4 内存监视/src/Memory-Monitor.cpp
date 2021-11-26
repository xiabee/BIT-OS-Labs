/**
 *	Author : xiabee
 *	Date   : 2021/1/5
 *
 */

#include <windows.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <Tlhelp32.h>
#include <stdio.h>
#include <tchar.h>
#include <shlwapi.h>
#include <psapi.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib,"kernel32.lib")

using namespace std;

// display protection status of a memory block.
void printProtection(DWORD dwTarget)
{
	char as[] = "----------";
	if (dwTarget & PAGE_NOACCESS) as[0] = 'N';
	if (dwTarget & PAGE_READONLY) as[1] = 'R';
	if (dwTarget & PAGE_READWRITE)as[2] = 'W';
	if (dwTarget & PAGE_WRITECOPY)as[3] = 'C';
	if (dwTarget & PAGE_EXECUTE) as[4] = 'X';
	if (dwTarget & PAGE_EXECUTE_READ) as[5] = 'r';
	if (dwTarget & PAGE_EXECUTE_READWRITE) as[6] = 'w';
	if (dwTarget & PAGE_EXECUTE_WRITECOPY) as[7] = 'c';
	if (dwTarget & PAGE_GUARD) as[8] = 'G';
	if (dwTarget & PAGE_NOCACHE) as[9] = 'D';
	if (dwTarget & PAGE_WRITECOMBINE) as[10] = 'B';
	printf("  %s  ", as);
}


void displaySystemConfig()
{
	SYSTEM_INFO si;
	memset(&si, 0, sizeof(si));
	GetNativeSystemInfo(&si);

	TCHAR str_page_size[MAX_PATH];
	StrFormatByteSize(si.dwPageSize, str_page_size, MAX_PATH);

	DWORD memory_size = (DWORD)si.lpMaximumApplicationAddress - (DWORD)si.lpMinimumApplicationAddress;
	TCHAR str_memory_size[MAX_PATH];
	StrFormatByteSize(memory_size, str_memory_size, MAX_PATH);

	printf("GetNativeSystemInfo\n");
	cout << "--------------------------------------------" << endl;
	cout << "Process architect              | " << (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL ? "x64" : "x86") << endl;
	cout << "Process Core                   | " << si.dwNumberOfProcessors << endl;
	cout << "Virtual memory page size       | " << str_page_size << endl;
	cout << "Minimum application address    | 0x" << hex << setfill('0') << setw(8) << (DWORD)si.lpMinimumApplicationAddress << endl;
	cout << "Maximum application address    | 0x" << hex << setw(8) << (DWORD)si.lpMaximumApplicationAddress << endl;
	cout << "Total available virtual memory | " << str_memory_size << endl;
	cout << "--------------------------------------------" << endl;
	return;

}

// display computer memory condition
void displayMemoryCondition()
{

	printf("GlobalMemoryStatusEx\n");
	cout << "--------------------------------------------" << endl;
	MEMORYSTATUSEX stat;
	stat.dwLength = sizeof(stat);
	GlobalMemoryStatusEx(&stat);

	long int DIV = 1024 * 1024;
	cout << "Memory Load               | " << setbase(10) << stat.dwMemoryLoad << "%\n"
		<< "Total physical memory     | " << setbase(10) << stat.ullTotalPhys / DIV << "MB\n"
		<< "Available physical memory | " << setbase(10) << stat.ullAvailPhys / DIV << "MB\n"
		<< "Total page file           | " << stat.ullTotalPageFile / DIV << "MB\n"
		<< "Avaliable page file       | " << stat.ullAvailPageFile / DIV << "MB\n"
		<< "Total virtual memory      | " << stat.ullTotalVirtual / DIV << "MB\n"
		<< "Avaliable virtual memory  | " << stat.ullAvailVirtual / DIV << "MB" << endl;
	cout << "--------------------------------------------" << endl;
}

// display a list of processes, with process filename and pid.
void getAllProcessInformation()
{
	printf("CreateToolhelp32Snapshot\n");

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessShot == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot error.\n");
		return;
	}

	cout << " |  num  |  pid  |  ProcessName" << endl;
	cout << "-----------------------------------------" << endl;
	if (Process32First(hProcessShot, &pe32)) {
		for (int i = 0; Process32Next(hProcessShot, &pe32); i++) {
			wprintf(L" | %4d  | %5d  |  %s\n", i, pe32.th32ProcessID, pe32.szExeFile);
		}
	}
	cout << "-----------------------------------------" << endl;
	CloseHandle(hProcessShot);
	return;
}

void ShowHelp()
{
	cout << "--------------------------------------------------------------------------" << endl;
	cout << "Type command: " << endl
		<< "\"config\"   : Display system memory configuration." << endl
		<< "\"condition\": Display system memory usage condition." << endl
		<< "\"process\"  : Display all running processes information (with pid)." << endl
		<< "\"pid\"      : Type a pid number and display detail of process with pid" << endl
		<< "\"help\"     : Display this help text." << endl
		<< "\"exit\"     : Exit" << endl;
	cout << "--------------------------------------------------------------------------" << endl;
	return;
}

// display memory distribution of a process with pid.
void getProcessDetail(int pid)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	if (!hProcess) return;
	cout << " | "
		<< "   Memory Addr    | "
		<< "   Size    | "
		<< "PageStatus| "
		<< "    Protect    | "
		<< "  Type  | "
		<< " ModuleName"
		<< endl;
	SYSTEM_INFO si;					// system info
	ZeroMemory(&si, sizeof(si));
	GetSystemInfo(&si);

	MEMORY_BASIC_INFORMATION mbi;		// save info here
	ZeroMemory(&mbi, sizeof(mbi));

	LPCVOID pBlock = (LPVOID)si.lpMinimumApplicationAddress;

	//int totalSize = 0;

	while (pBlock < si.lpMaximumApplicationAddress) {
		if (VirtualQueryEx(hProcess, pBlock, &mbi, sizeof(mbi)) == sizeof(mbi))
		{
			cout << " | ";

			// size of block 
			LPCVOID pEnd = (PBYTE)pBlock + mbi.RegionSize;
			TCHAR szSize[MAX_PATH];
			StrFormatByteSize(mbi.RegionSize, szSize, MAX_PATH); //size of block

			// addr and size
			cout.fill('0');
			cout << hex << setw(8) << (DWORD)pBlock
				<< "-"
				<< hex << setw(8) << (DWORD)pEnd - 1
				<< " | ";
			printf("%10ls", szSize);

			// display memory block status
			switch (mbi.State)
			{

			case MEM_COMMIT:cout << " | " << setw(9) << "Committed" << " | "; break;
			case MEM_FREE:cout << " | " << setw(9) << "   Free  " << " | "; break;
			case MEM_RESERVE:cout << " | " << setw(9) << " Reserved" << " | "; break;
			default: cout << "          | "; break;
			}

			// protect status
			if (mbi.Protect == 0 && mbi.State != MEM_FREE)
			{

				mbi.Protect = PAGE_READONLY;

			}
			printProtection(mbi.Protect);

			switch (mbi.Type)
			{

			case MEM_IMAGE:cout << " |  Image  | "; break;
			case MEM_PRIVATE:cout << " | Private | "; break;
			case MEM_MAPPED:cout << " |  Mapped | "; break;
			default:cout << " |         | "; break;
			}

			// module
			TCHAR str_module_name[MAX_PATH];
			if (GetModuleFileName((HMODULE)pBlock, str_module_name, MAX_PATH) > 0) {
				PathStripPath(str_module_name);
				wprintf(L"%s", str_module_name);
			}
			cout << endl;
			pBlock = pEnd;	// proceed next memory block
		}
	}
}

int main()
{
	setlocale(LC_ALL, "CHS");
	//int flag = 9;
	cout << endl << "*-----------System Memory Manager-----------*" << endl << endl;
	cout << "--Type 'help' for help.\n" << endl;
	string cmd;
	char cmd_charstr[127];
	while (1)
	{
		cout << "Manager> ";
		cin.getline(cmd_charstr, 127);
		cmd = cmd_charstr;

		if (cmd == "config") {
			cout << endl;
			displaySystemConfig();
		}
		else if (cmd == "condition") {
			cout << endl;
			displayMemoryCondition();
		}
		else if (cmd == "process") {
			cout << endl;
			getAllProcessInformation();
		}
		else if (cmd == "pid") {
			cout << "PID> ";
			int pid = 0;
			cin >> pid;
			cin.getline(cmd_charstr, 127);
			if (pid <= 0) continue;
			cout << endl;
			getProcessDetail(pid);
		}
		else if (cmd == "help") {
			cout << endl;
			ShowHelp();
		}
		else if (cmd == "exit") {
			break;
		}
		else if (cmd == "clear" || cmd == "cls") {
			system("cls");
		}
		else {
			if (cmd != "") cout << "Invalid command, maybe you can type \"help\"?." << endl;
			//cout << "'" << cmd_charstr << "'" << endl;
			fflush(stdin);
			cin.clear();
			continue;
		}
		cin.clear();

	}
	return 0;

}
