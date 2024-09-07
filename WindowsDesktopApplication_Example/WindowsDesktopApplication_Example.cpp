#include "framework.h"
#include "WindowsDesktopApplication_Example.h"
#include <commctrl.h> // For ListView and scrollbar controls
#include <vector>
#include <string>
#include <thread>
#include <chrono>

#define MAX_LOADSTRING 100

// Global Variables
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

static HWND hListView = NULL;
static int itemCount = 30; // Number of tasks

// Forward declarations
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Updates the task status in the ListView
void UpdateTaskStatus(int index, const std::wstring& status) {
    LVITEM lvi = { 0 };
    lvi.mask = LVIF_TEXT;
    lvi.iItem = index;
    lvi.iSubItem = 1; // Status column
    lvi.pszText = const_cast<LPWSTR>(status.c_str());
    ListView_SetItem(hListView, &lvi);
}

// Simulate task processing
void StartTask(int index) {
    // Update status to "Running"
    UpdateTaskStatus(index, L"Running");

    // Simulate task work by sleeping for a few seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Update status to "Completed"
    UpdateTaskStatus(index, L"Completed");
}

// Entry point
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES | ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSDESKTOPAPPLICATIONEXAMPLE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow)) {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSDESKTOPAPPLICATIONEXAMPLE));

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

// Register window class
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSDESKTOPAPPLICATIONEXAMPLE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MYMENU);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// Initialize instance
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInst, nullptr);

    if (!hWnd) {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Create the Start Tasks button
    CreateWindowW(L"BUTTON", L"Start Tasks", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 10, 120, 30, hWnd, (HMENU)ID_START_TASKS, hInst, NULL);

    // Create ListView
    hListView = CreateWindowW(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
        10, 50, 800, 500, hWnd, (HMENU)ID_LISTVIEW, hInst, NULL);

    if (hListView == NULL) {
        MessageBox(hWnd, L"Failed to create ListView", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // Add columns to ListView
    LVCOLUMN lvc = { 0 };
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc.pszText = const_cast<LPWSTR>(L"Task Name");
    lvc.cx = 250;
    ListView_InsertColumn(hListView, 0, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"Status");
    lvc.cx = 250;
    ListView_InsertColumn(hListView, 1, &lvc);

    return TRUE;
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == ID_START_TASKS)
        {
            if (hListView != NULL)
            {
                // Populate ListView with tasks
                ListView_DeleteAllItems(hListView);

                for (int i = 0; i < itemCount; ++i)
                {
                    LVITEM lvi = { 0 };
                    lvi.mask = LVIF_TEXT;
                    lvi.iItem = i;
                    lvi.iSubItem = 0; // Task Name column
                    std::wstring taskName = L"Task " + std::to_wstring(i + 1);
                    lvi.pszText = const_cast<LPWSTR>(taskName.c_str());
                    ListView_InsertItem(hListView, &lvi);

                    // Initially set status to "Pending"
                    UpdateTaskStatus(i, L"Pending");
                }

                // Start tasks in separate threads to simulate asynchronous work
                for (int i = 0; i < itemCount; ++i) {
                    std::thread(StartTask, i).detach();
                }
            }
        }
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
