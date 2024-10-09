#pragma once

#include <Windows.h>
#include <dwmapi.h>

#include "encrypt.hpp"
#include "../../spoofer.hpp"

inline BOOL(*_GetCursorPos)(LPPOINT lpPoint) = nullptr;
inline BOOL(*_SetLayeredWindowAttributes)(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags) = nullptr;
inline HRESULT(*_DwmExtendFrameIntoClientArea)(HWND hwnd, const MARGINS* pMarInset) = nullptr;
inline LRESULT(*_DefWindowProc)(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam) = nullptr;
inline HWND(*_CreateWindowExA)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) = nullptr;
inline LONG(*_SetWindowLongA)(HWND hwnd, int nIndex, LONG dwNewLong) = nullptr;
inline LONG(*_GetWindowLongA)(HWND hwnd, int nIndex) = nullptr;
inline ATOM(*_RegisterClassExA)(const WNDCLASSEXA*) = nullptr;
inline int(*_GetSystemMetrics)(int nIndex) = nullptr;
inline HWND(*_FindWindowA)(LPCSTR lpClassName, LPCSTR lpWindowName) = nullptr;
inline SHORT(*_GetAsyncKeyState)(int vKey) = nullptr;
inline HWND(*_GetForegroundWindow)() = nullptr;
inline BOOL(*_MoveWindow)(HWND hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint) = nullptr;
inline BOOL(*_PeekMessageA)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) = nullptr;
inline BOOL(*_TranslateMessage)(const MSG* lpMsg) = nullptr;
inline LRESULT(*_DispatchMessageA)(const MSG* lpMsg) = nullptr;
inline HANDLE(*_CreateThread)(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE     lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) = nullptr;
inline BOOL(*_CloseHandle)(HANDLE hObject) = nullptr;
inline BOOL(*_SetWindowPos)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) = nullptr;
inline BOOL(*_ShowWindow)(HWND hWnd, int nCmdShow) = nullptr;
inline BOOL(*_UpdateWindow)(HWND) = nullptr;

inline void hide_imports()
{
	auto user32 = msa_import(LoadLibraryA).safe_cached()(_("user32.dll"));
	auto kernel32 = msa_import(LoadLibraryA).safe_cached()(_("Kernel32.dll"));
	auto dwmapi = msa_import(LoadLibraryA).safe_cached()(_("Dwmapi.dll"));

	if (!user32)
	{
		return;
	}

	if (!dwmapi)
	{
		return;
	}

	if (!kernel32)
	{
		return;
	}

	void* GetCursorPos = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("GetCursorPos"));
	void* SetLayeredWindowAttributes = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("SetLayeredWindowAttributes"));
	void* DwmExtendFrameIntoClientArea = (void*)msa_import(GetProcAddress).safe_cached()(dwmapi, _("DwmExtendFrameIntoClientArea"));
	void* DefWindowProcA = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("DefWindowProcA"));
	void* CreateWindowExA = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("CreateWindowExA"));
	void* SetWindowLongA = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("SetWindowLongA"));
	void* GetWindowLongA = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("GetWindowLongA"));
	void* RegisterClassExA = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("RegisterClassExA"));
	void* GetSystemMetrics = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("GetSystemMetrics"));
	void* FindWindowA = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("FindWindowA"));
	void* GetAsyncKeyState = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("GetAsyncKeyState"));
	void* GetForegroundWindow = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("GetForegroundWindow"));
	void* MoveWindow = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("MoveWindow"));
	void* PeekMessageA = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("PeekMessageA"));
	void* TranslateMessage = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("TranslateMessage"));
	void* DispatchMessageA = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("DispatchMessageA"));
	void* CreateThread = (void*)msa_import(GetProcAddress).safe_cached()(kernel32, _("CreateThread"));
	void* SetWindowPos = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("SetWindowPos"));
	void* CloseHandle = (void*)msa_import(GetProcAddress).safe_cached()(kernel32, _("CloseHandle"));
	void* ShowWindow = (void*)msa_import(GetProcAddress).safe_cached()(user32, _("ShowWindow"));
	void* updetewindow = (void*)msa_import(GetProcAddress).get()(user32, _("UpdateWindow"));

	if (!GetCursorPos)
	{
		return;
	}

	if (!SetLayeredWindowAttributes)
	{
		return;
	}

	if (!DwmExtendFrameIntoClientArea)
	{
		return;
	}

	if (!DefWindowProcA)
	{
		return;
	}

	if (!CreateWindowExA)
	{
		return;
	}

	if (!SetWindowLongA)
	{
		return;
	}

	if (!GetWindowLongA)
	{
		return;
	}

	if (!RegisterClassExA)
	{
		return;
	}

	if (!GetSystemMetrics)
	{
		return;
	}

	if (!FindWindowA)
	{
		return;
	}

	if (!GetAsyncKeyState)
	{
		return;
	}

	if (!GetForegroundWindow)
	{
		return;
	}

	if (!MoveWindow)
	{
		return;
	}

	if (!PeekMessageA)
	{
		return;
	}

	if (!TranslateMessage)
	{
		return;
	}

	if (!DispatchMessageA)
	{
		return;
	}

	if (!CreateThread)
	{
		return;
	}

	if (!CloseHandle)
	{
		return;
	}

	if (!SetWindowPos)
	{
		return;
	}

	if (!ShowWindow)
	{
		return;
	}

	if (!updetewindow)
	{
		return;
	}

	*(void**)&_GetCursorPos = GetCursorPos;
	*(void**)&_SetLayeredWindowAttributes = SetLayeredWindowAttributes;
	*(void**)&_DwmExtendFrameIntoClientArea = DwmExtendFrameIntoClientArea;
	*(void**)&_DefWindowProc = DefWindowProcA;
	*(void**)&_CreateWindowExA = CreateWindowExA;
	*(void**)&_SetWindowLongA = SetWindowLongA;
	*(void**)&_GetWindowLongA = GetWindowLongA;
	*(void**)&_RegisterClassExA = RegisterClassExA;
	*(void**)&_GetSystemMetrics = GetSystemMetrics;
	*(void**)&_FindWindowA = FindWindowA;
	*(void**)&_GetAsyncKeyState = GetAsyncKeyState;
	*(void**)&_GetForegroundWindow = GetForegroundWindow;
	*(void**)&_MoveWindow = MoveWindow;
	*(void**)&_PeekMessageA = PeekMessageA;
	*(void**)&_TranslateMessage = TranslateMessage;
	*(void**)&_DispatchMessageA = DispatchMessageA;
	*(void**)&_CreateThread = CreateThread;
	*(void**)&_CloseHandle = CloseHandle;
	*(void**)&_SetWindowPos = SetWindowPos;
	*(void**)&_ShowWindow = ShowWindow;
	*(void**)&_UpdateWindow = updetewindow;

	msa_import(FreeLibrary).get()(kernel32);
	msa_import(FreeLibrary).get()(user32);
	msa_import(FreeLibrary).get()(dwmapi);
}

inline BOOL GetCursorPosA(LPPOINT lpPoint)
{
	SPOOF_FUNC;

	return _GetCursorPos(lpPoint);
}

inline BOOL SetLayeredWindowAttributes_Spoofed(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags)
{
	SPOOF_FUNC;

	return _SetLayeredWindowAttributes(hwnd, crKey, bAlpha, dwFlags);
}

inline HRESULT __stdcall DwmExtendFrameIntoClientArea_Spoofed(HWND hwnd, const MARGINS* pMarInset)
{
	SPOOF_FUNC;

	return _DwmExtendFrameIntoClientArea(hwnd, pMarInset);
}

inline LRESULT __stdcall DefWindowProcA_Spoofed(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	SPOOF_FUNC;

	return _DefWindowProc(hwnd, Msg, wParam, lParam);
}

inline HWND __stdcall CreateWindowExA_Spoofed(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	SPOOF_FUNC;

	return _CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

inline LONG __stdcall SetWindowLongA_Spoofed(HWND hwnd, int nIndex, LONG dwNewLong)
{
	SPOOF_FUNC;

	return _SetWindowLongA(hwnd, nIndex, dwNewLong);
}

inline LONG __stdcall GetWindowLongA_Spoofed(HWND hwnd, int nIndex)
{
	SPOOF_FUNC;

	return _GetWindowLongA(hwnd, nIndex);
}

inline ATOM __stdcall RegisterClassExA_Spoofed(const WNDCLASSEXA* classame)
{
	SPOOF_FUNC;

	return _RegisterClassExA(classame);
}

inline int __stdcall GetSystemMetrics_Spoofed(int nIndex)
{
	SPOOF_FUNC;

	return _GetSystemMetrics(nIndex);
}

inline HWND __stdcall FindWindowA_Spoofed(LPCSTR lpClassName, LPCSTR lpWindowName)
{
	SPOOF_FUNC;

	return _FindWindowA(lpClassName, lpWindowName);
}

inline BOOL __stdcall UpdateWindow_Spoofed(HWND hWnd) {
	SPOOF_FUNC;

	return _UpdateWindow(hWnd);
}

inline HWND __stdcall GetForegroundWindow_Spoofed()
{
	SPOOF_FUNC;

	return _GetForegroundWindow();
}

inline SHORT GetAsyncKeyState_Spoofed(int vKey)
{
	SPOOF_FUNC;

	return _GetAsyncKeyState(vKey);
}

inline SHORT MoveWindow_Spoofed(HWND hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint)
{
	SPOOF_FUNC;

	return _MoveWindow(hWnd, X, Y, nWidth, nHeight, bRepaint);
}

inline BOOL __stdcall PeekMessageA_Spoofed(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
	SPOOF_FUNC;

	return _PeekMessageA(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
}

inline BOOL __stdcall TranslateMessage_Spoofed(const MSG* lpMsg)
{
	SPOOF_FUNC;

	return _TranslateMessage(lpMsg);
}

inline LRESULT __stdcall DispatchMessageA_Spoofed(const MSG* lpMsg)
{
	SPOOF_FUNC;

	return _DispatchMessageA(lpMsg);
}

inline HANDLE __stdcall CreateThread_Spoofed(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId)
{
	SPOOF_FUNC;

	return _CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
}

inline BOOL __stdcall CloseHandle_Spoofed(HANDLE hObject)
{
	SPOOF_FUNC;

	return _CloseHandle(hObject);
}

inline BOOL __stdcall SetWindowPos_Spoofed(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	SPOOF_FUNC;

	return _SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

inline BOOL __stdcall ShowWindow_Spoofed(HWND hWnd, int nCmdShow)
{
	SPOOF_FUNC;

	return _ShowWindow(hWnd, nCmdShow);
}