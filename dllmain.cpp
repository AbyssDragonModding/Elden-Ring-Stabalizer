// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <thread>
#include <string>
#include <tchar.h>

const bool DEBUG_MODE = false;

DWORD_PTR SetCpuAffinity()
{
    HANDLE hProcess = GetCurrentProcess();

    // Get system affinity mask to know how many cores are available
    DWORD_PTR processAffinityMask = 0;
    DWORD_PTR systemAffinityMask = 0;

    if (!GetProcessAffinityMask(hProcess, &processAffinityMask, &systemAffinityMask))
        return 0;

    // Build new mask: All cores except core 0;
    DWORD_PTR newMask = systemAffinityMask & ~(DWORD_PTR)1;

    // Safety check if only 1 core exists
    if (newMask == 0)
        newMask = systemAffinityMask;

    // Apply and return previous mask to caller so you can restore it later
    DWORD_PTR previousMask = 0;
    if (!SetProcessAffinityMask(hProcess, newMask))
        return 0;

    if (DEBUG_MODE)
    {
        MessageBoxA(nullptr, "Elden Ring Stabalizer", "Process Affinity Set", MB_OK);
    }
    return processAffinityMask;
}
BOOL IsServiceRunning()
{
    SC_HANDLE schSCManager = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
    if (!schSCManager) return false;

    SC_HANDLE schService = OpenService(schSCManager, "SSDPSRV", SERVICE_QUERY_STATUS);
    if (!schService) {
        CloseServiceHandle(schSCManager);
        return false;
    }

    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwBytesNeeded;
    bool isRunning = false;

    if (QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus,
        sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded)) {
        isRunning = (ssStatus.dwCurrentState == SERVICE_RUNNING);
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return isRunning;
}

BOOL StopService()
{
    // Open SCM
    SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!scm) return false;

    // Open the service with STOP + QUERY access
    SC_HANDLE svc = OpenService(
        scm,
        "SSDPSRV",
        SERVICE_STOP | SERVICE_QUERY_STATUS
    );

    if (!svc) {
        CloseServiceHandle(scm);
        return false;
    }

    SERVICE_STATUS_PROCESS status = {};
    DWORD bytesNeeded = 0;

    // Query current state
    if (!QueryServiceStatusEx(
        svc,
        SC_STATUS_PROCESS_INFO,
        reinterpret_cast<LPBYTE>(&status),
        sizeof(status),
        &bytesNeeded))
    {
        CloseServiceHandle(svc);
        CloseServiceHandle(scm);
        return false;
    }

    // If already stopped, nothing to do
    if (status.dwCurrentState == SERVICE_STOPPED) {
        CloseServiceHandle(svc);
        CloseServiceHandle(scm);
        return true;
    }

    // Send STOP control code
    if (!ControlService(svc, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&status)) {
        CloseServiceHandle(svc);
        CloseServiceHandle(scm);
        return false;
    }

    // Wait for the service to stop
    DWORD startTick = GetTickCount64();
    DWORD timeout = 30000; // 30 seconds

    while (status.dwCurrentState != SERVICE_STOPPED) {
        Sleep(status.dwWaitHint);

        if (!QueryServiceStatusEx(
            svc,
            SC_STATUS_PROCESS_INFO,
            reinterpret_cast<LPBYTE>(&status),
            sizeof(status),
            &bytesNeeded))
        {
            break;
        }

        if (GetTickCount64() - startTick > timeout) {
            break;
        }
    }

    bool stopped = (status.dwCurrentState == SERVICE_STOPPED);

    CloseServiceHandle(svc);
    CloseServiceHandle(scm);
    if (DEBUG_MODE) {
        MessageBoxA(nullptr, "Elden Ring Stabalizer", "SSDPSRV Service stopped", MB_OK);
    }
    return stopped;

}

void procThread()
{
    Sleep(1000); // Sleep for 1 second
    SetCpuAffinity();

    if (IsServiceRunning())
        StopService();

    // wait for 2 min before checking again
    while (true)
    {
        Sleep(2 * 60 * 1000);
        // check if the service is running and stop it
        if (IsServiceRunning())
            StopService();
    }
}

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
    static std::thread processThread; // Declare outside switch to ensure initialization

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Create new thread on process Atatch
        processThread = std::thread(procThread);
        processThread.detach();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
