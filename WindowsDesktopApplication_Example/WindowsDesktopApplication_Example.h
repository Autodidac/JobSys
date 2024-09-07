#pragma once

#include "resource.h"         // For resource definitions
#include "SafeJobSystem.h" // Include header for ThreadManager
#include <vector>
#include <string>
#include <mutex>

// Forward declaration of ThreadManager class
namespace SafeJobSystem {
    class ThreadManager;
}

// Global declaration of threadManager
extern SafeJobSystem::ThreadManager threadManager;

// Function declarations
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Custom min function
template<typename T>
T customMin(T a, T b) {
    return (a < b) ? a : b;
}

// Custom max function
template<typename T>
T customMax(T a, T b) {
    return (a > b) ? a : b;
}

// Custom clamp function
template<typename T>
T customClamp(T value, T minValue, T maxValue) {
    return customMax(minValue, customMin(value, maxValue));
}
