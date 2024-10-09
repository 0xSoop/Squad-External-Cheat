#pragma once
#include <tchar.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
namespace gui
{
	
	typedef HWND(WINAPI* CreateWindowInBand)(_In_ DWORD dwExStyle, _In_opt_ ATOM atom, _In_opt_ LPCWSTR lpWindowName, _In_ DWORD dwStyle, _In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight, _In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam, DWORD band);

	CreateWindowInBand create_window_in_band = 0;
	enum ZBID
	{
		ZBID_DEFAULT = 0,
		ZBID_DESKTOP = 1,
		ZBID_UIACCESS = 2,
		ZBID_IMMERSIVE_IHM = 3,
		ZBID_IMMERSIVE_NOTIFICATION = 4,
		ZBID_IMMERSIVE_APPCHROME = 5,
		ZBID_IMMERSIVE_MOGO = 6,
		ZBID_IMMERSIVE_EDGY = 7,
		ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
		ZBID_IMMERSIVE_INACTIVEDOCK = 9,
		ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
		ZBID_IMMERSIVE_ACTIVEDOCK = 11,
		ZBID_IMMERSIVE_BACKGROUND = 12,
		ZBID_IMMERSIVE_SEARCH = 13,
		ZBID_GENUINE_WINDOWS = 14,
		ZBID_IMMERSIVE_RESTRICTED = 15,
		ZBID_SYSTEM_TOOLS = 16,
		ZBID_LOCK = 17,
		ZBID_ABOVELOCK_UX = 18,
	};
	DWORD duplicate_winlogin_token(DWORD dwSessionId, DWORD dwDesiredAccess, PHANDLE phToken)
	{
		DWORD dwErr;
		PRIVILEGE_SET ps;
		ps.PrivilegeCount = 1;
		ps.Control = PRIVILEGE_SET_ALL_NECESSARY;

		if (LookupPrivilegeValue(NULL, SE_TCB_NAME, &ps.Privilege[0].Luid)) {
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hSnapshot != INVALID_HANDLE_VALUE) {
				PROCESSENTRY32 pe;
				pe.dwSize = sizeof(pe);
				BOOL bFound = FALSE;

				for (BOOL bCont = Process32First(hSnapshot, &pe); bCont; bCont = Process32Next(hSnapshot, &pe)) {
					if (_tcsicmp(pe.szExeFile, TEXT("winlogon.exe")) != 0) continue;

					HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
					if (hProcess) {
						HANDLE hToken;
						if (OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_DUPLICATE, &hToken)) {
							BOOL fTcb;
							DWORD sid;

							if (PrivilegeCheck(hToken, &ps, &fTcb) && fTcb) {
								DWORD dwRetLen;
								if (GetTokenInformation(hToken, TokenSessionId, &sid, sizeof(sid), &dwRetLen) && sid == dwSessionId) {
									bFound = TRUE;
									if (DuplicateTokenEx(hToken, dwDesiredAccess, NULL, SecurityImpersonation, TokenImpersonation, phToken)) {
										dwErr = ERROR_SUCCESS;
									}
									else {
										dwErr = GetLastError();
									}
								}
							}
							CloseHandle(hToken);
						}
						CloseHandle(hProcess);
					}
					if (bFound) break;
				}
				CloseHandle(hSnapshot);
			}
			else {
				dwErr = GetLastError();
			}
		}
		else {
			dwErr = GetLastError();
		}

		return dwErr;
	}
	DWORD create_ui_access_token(PHANDLE token_phandle)
	{
		DWORD dwerr;
		HANDLE token_self_handle;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE, &token_self_handle))
		{
			DWORD session_id, retlen;
			if (GetTokenInformation(token_self_handle, TokenSessionId, &session_id, sizeof(session_id), &retlen))
			{
				HANDLE token_system_handle;
				dwerr = duplicate_winlogin_token(session_id, TOKEN_IMPERSONATE, &token_system_handle);
				if (ERROR_SUCCESS == dwerr)
				{
					if (SetThreadToken(NULL, token_system_handle))
					{
						if (DuplicateTokenEx(token_self_handle, TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_DEFAULT, 0, SecurityAnonymous, TokenPrimary, token_phandle))
						{
							BOOL ui_access = TRUE;
							if (!SetTokenInformation(*token_phandle, TokenUIAccess, &ui_access, sizeof(ui_access)))
							{
								dwerr = GetLastError();
								CloseHandle(*token_phandle);
							}
						}
						else
						{
							dwerr = GetLastError();
						}
						RevertToSelf();
					}
					else
					{
						dwerr = GetLastError();
					}
					CloseHandle(token_system_handle);
				}
			}
			else
			{
				dwerr = GetLastError();
			}
			CloseHandle(token_self_handle);
		}
		else
		{
			dwerr = GetLastError();
		}
		return dwerr;
	}
	BOOL check_for_ui_acces(DWORD* pdwerr, DWORD* ui_access)
	{
		BOOL result = FALSE;
		HANDLE token_handle;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token_handle))
		{
			DWORD retlen;
			if (GetTokenInformation(token_handle, TokenUIAccess, ui_access, sizeof(*ui_access), &retlen))
			{
				result = TRUE;
			}
			else
			{
				*pdwerr = GetLastError();
			}
			CloseHandle(token_handle);
		}
		else
		{
			*pdwerr = GetLastError();
		}
		return result;
	}
	DWORD prepare_for_ui_access()
	{
		DWORD dwerr;
		HANDLE token_ui_access_handle;
		BOOL ui_access;
		if (check_for_ui_acces(&dwerr, (DWORD*)&ui_access))
		{
			if (ui_access)
			{
				dwerr = ERROR_SUCCESS;
			}
			else
			{
				dwerr = create_ui_access_token(&token_ui_access_handle);
				if (ERROR_SUCCESS == dwerr)
				{
					STARTUPINFO si;
					PROCESS_INFORMATION pi;
					GetStartupInfo(&si);
					if (CreateProcessAsUser(token_ui_access_handle, 0, GetCommandLine(), 0, 0, FALSE, 0, 0, 0, &si, &pi))
					{
						CloseHandle(pi.hProcess), CloseHandle(pi.hThread);
						ExitProcess(0);
					}
					else
					{
						dwerr = GetLastError();
					}
					CloseHandle(token_ui_access_handle);
				}
			}
		}
		return dwerr;
	}
	namespace librarys
	{
		HMODULE user32;
		HMODULE win32u;
		bool init()
		{
			HMODULE user32_lib = LoadLibraryA("user32.dll");
			if (!user32_lib) return false;
			HMODULE win32u_lib = LoadLibraryA("win32u.dll");
			if (!win32u_lib) return false;
			user32 = GetModuleHandleA("user32.dll");
			if (!user32) return false;
			win32u = GetModuleHandleA("win32u.dll");
			if (!win32u) return false;
			if (!user32_lib) {
				printf("Failed to load user32.dll\n");
				return false;
			}
			if (!win32u_lib) {
				printf("Failed to load win32u.dll\n");
				return false;
			}
			return true;

		}
	}
	bool init()
	{
		const DWORD dwerr = prepare_for_ui_access();
		if (dwerr != ERROR_SUCCESS) {
			printf("prepare_for_ui_access failed with error code: %lu\n", dwerr);
			return false;
		}
		librarys::init();
		create_window_in_band = reinterpret_cast<CreateWindowInBand>(GetProcAddress(librarys::user32, "CreateWindowInBand"));
		if (!create_window_in_band) {
			printf("Failed to get CreateWindowInBand from user32\n");
			create_window_in_band = reinterpret_cast<CreateWindowInBand>(GetProcAddress(librarys::win32u, "CreateWindowInBand"));
			if (!create_window_in_band) {
				printf("Failed to get CreateWindowInBand from win32u\n");
				return false;
			}
		}

		return true;
	}

}