#include <iostream>
#include <windows.h>
int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        printf("Please input time!\n");
        return 0;
    }
    Sleep(strtod(argv[1],0) * 1000.0);
    return 0;
}
