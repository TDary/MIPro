// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include<iostream>
#include<Windows.h>
#include<TlHelp32.h>
#include<tchar.h>
using namespace std;
#pragma comment(lib,"msvcrt.lib")

// 定义宏以捕获分配位置
#define MY_MALLOC(size) my_malloc(size, __FILE__, __LINE__)
#define MY_FREE(p) my_free(p)

//定义一个简单的链表结构来跟踪内存分配
typedef struct Allocation {
    void* address;
    size_t size;
    const char* file;
    int line;
    struct Allocation* next;
} Allocation;

Allocation* allocations = NULL;

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH://DLL_PROCESS_ATTACH: DLL 被进程加载时调用。
    {
        MessageBoxA(NULL, "注入成功", "提示", MB_OK);
        void* addr = Find_GameProcess();
        if (addr == 0)
        {
            MessageBoxA(NULL, "地址为空", "提示", MB_OK);
        }
        break;
    }
    case DLL_THREAD_ATTACH://DLL_PROCESS_DETACH: DLL 被进程卸载时调用。
        break;
    case DLL_THREAD_DETACH://DLL_THREAD_ATTACH: 进程创建新线程时调用。
        break;
    case DLL_PROCESS_DETACH://DLL_THREAD_DETACH: 进程中线程退出时调用。
        //退出钩子函数，停止Hook
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) DWORD GetProcessID(LPCTSTR szProcessName) {
    DWORD dwProcessID = 0;
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapShot == INVALID_HANDLE_VALUE)
    {
        return dwProcessID;
    }
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    if (Process32First(hSnapShot, &pe32))
    {
        CloseHandle(hSnapShot);
        return dwProcessID;
    }
    do {
        if (_tcsicmp(pe32.szExeFile, szProcessName) == 0)
        {
            dwProcessID = pe32.th32ProcessID;
            break;
        }
    } while (Process32Next(hSnapShot, &pe32));
    CloseHandle(hSnapShot);
    return dwProcessID;
}

//bool Hook_GameProcess() {
//    void* addr = Find_GameProcess();
//    _OpenProcessAddr = addr;
//
//    if (addr == 0)
//    {
//        MessageBoxA(NULL, "Func addr not found.", NULL, 0);
//        return false;
//    }
//
//    // 代码段一般不可写的，改写内存属性
//
//    VirtualProtect((void*)addr, 5, PAGE_EXECUTE_READWRITE, &__old_protect);
//
//    // 跳转偏移计算方式: 跳转偏移 = 目标地址 - ( 指令地址 + 5 )
//    // 0xE9 为 JMP
//
//    __jmp[0] = 0xE9;
//    *(DWORD*)&__jmp[1] = (DWORD)((long long)MyTerminateProcess - (long long)addr - 5);
//    // 保存原先字节
//    memcpy(__old_jmp, (void*)addr, 5);
//    // 替换原先字节
//    memcpy((void*)addr, __jmp, 5);
//}

void* Find_GameProcess() {
    void* addr = 0;
    // 获取当前进程的句柄
    HANDLE hProcess = GetCurrentProcess();

    // 获取当前进程的基址
    HMODULE hModule = GetModuleHandle(NULL);
    //获取GameProcess的地址
    addr = (void*)GetProcAddress(hModule, "MechaBREAK");
    return addr;
}

////函数来添加内存分配记录
//void add_allocation(void* p, size_t size, const char* file, int line) {
//    Allocation* newAlloc = (Allocation*)malloc(sizeof(Allocation));
//    newAlloc->address = p;
//    newAlloc->size = size;
//    newAlloc->file = file;
//    newAlloc->line = line;
//    newAlloc->next = allocations;
//    allocations = newAlloc;
//}
//
////函数来移除内存分配记录
//void remove_allocation(void* p) {
//    Allocation** ptr = &allocations;
//    while (*ptr)
//    {
//        Allocation* entry = *ptr;
//        if (entry->address==p)
//        {
//            *ptr = entry->next;
//            free(entry);
//            return;
//        }
//        ptr = &entry->next;
//    }
//}
//
////自定义的malloc函数
//void* my_malloc(size_t size, const char* file, int line) {
//    void* p = malloc(size);
//    add_allocation(p, size,file,line);
//    return p;
//}
//
////自定义的free函数
//void my_free(void* p) {
//    remove_allocation(p);
//    free(p);
//}
//
////函数来检查和报告内存泄漏
//void check_for_leaks() {
//    Allocation* current = allocations;
//    if (current==NULL)
//    {
//        printf("No memory leaks detected.\n");
//    }
//    else {
//        printf("Memory leaks detected:\n");
//        while (current)
//        {
//            printf("Leaked memory at address %p，size %zu，allocated at %s:%d\n", current->address, current->size,current->file,current->line);
//            current = current->next;
//        }
//    }
//}