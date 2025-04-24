#include<iostream>
using namespace std;
#include<Windows.h>
#include <tlhelp32.h>
#include<tchar.h>
#pragma comment(lib, "msvcrt.lib")

DWORD GetProcessID(LPCTSTR szProcessName)
{
    DWORD dwProcessID = 0;
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapShot == INVALID_HANDLE_VALUE) {
        return dwProcessID;
    }
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    if (!Process32First(hSnapShot, &pe32)) {
        CloseHandle(hSnapShot);
        return dwProcessID;
    }
    do {
        if (_tcsicmp(pe32.szExeFile, szProcessName) == 0) {
            dwProcessID = pe32.th32ProcessID;
            break;
        }
    } while (Process32Next(hSnapShot, &pe32));
    CloseHandle(hSnapShot);
    return dwProcessID;
}

int main()
{
    const char* data = "G:/DllInfoTest/x64/Release/DllInfoTest.dll";
    int buffSize = strlen(data) + 1;
    DWORD dwProcessID = GetProcessID(L"MechaBREAK.exe");
    HANDLE hGameProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
    char* lpRemoteMem = (char*)VirtualAllocEx(hGameProcess, NULL, buffSize, MEM_COMMIT, PAGE_READWRITE);
    SIZE_T nBytesWritten = 0;
    BOOL bRet = WriteProcessMemory(hGameProcess, lpRemoteMem, data, buffSize, &nBytesWritten);
    if (bRet == FALSE || nBytesWritten != buffSize)
    {
        CloseHandle(hGameProcess);
        VirtualFreeEx(hGameProcess, lpRemoteMem, buffSize, MEM_RELEASE);
        return 0;
    }
    HANDLE hThread = CreateRemoteThread(hGameProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, lpRemoteMem, 0, NULL);

    WaitForSingleObject(hThread, INFINITE);
    VirtualFreeEx(hGameProcess, lpRemoteMem, buffSize, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hGameProcess);
    cout << "报告！！已成功退出注入程序" << endl;
    system("pause");
    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
