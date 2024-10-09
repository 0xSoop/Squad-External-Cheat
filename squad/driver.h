#pragma once
#include <Windows.h>

#include "stdint.h"
#include <stdio.h>
#include <basetsd.h>
#include <cstdint>
#include <iostream>
#include <vector>
#include <TlHelp32.h>
#include <cstdint>
#include <string>


/* ############ */
/* FIND GUARDED */
/* ############ */

typedef struct _GET_GUARDED_REGION {
	uintptr_t       allocation;
} GET_GUARDED_REGION, * PGUARDED_REGION;

/* ################# */
/* MEMORY OPERATIONS */
/* ################# */

typedef struct _dtb_invoke
{
	uint32_t pid;
	bool operation;
} dtb_invoke, * pdtb_invoke;

typedef struct _MEMORY_OPERATION_DATA {
	uint32_t        src_pid;
	uint64_t        src_addr;
	uint64_t        dst_addr;
	size_t          size;
	unsigned long long  DirBase;

	uint32_t        protect;
	PVOID           allocate_base;
	uint64_t        processPEB;
	uint64_t        curentCR3;
	// getProcess function, not being used
	// PEPROCESS    processor;
} MEMORY_OPERATION_DATA, * PMEMORY_OPERATION_DATA;

/* ############ */
/* BASE ADDRESS */
/* ############ */

typedef struct _GET_PROCESS_BASE_ADDRESS_DATA {
	uint32_t        src_pid;
	DWORD64         baseAddress;
} GET_PROCESS_BASE_ADDRESS_DATA, * PGET_PROCESS_BASE_ADDRESS_DATA;

/* ########## */
/* MOUSE MOVE */
/* ########## */

typedef struct _MOUSE_MOVE_DATA {
	LONG            x;
	LONG            y;
	USHORT          button_flags;
} MOUSE_MOVE_DATA, * PMOUSE_MOVE_DATA;

/* ################ */
/* DISPLAY AFFINITY */
/* ################ */

typedef struct _STREAMMODE {
	uint32_t        WindowHandle;
	uint64_t        Value;
} STREAMMODE, * PSTREAMODE;

#define COMMAND 0x600
#define IOCTL_MOVE_MOUSE CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x1, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_READ_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x2, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_WRITE_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x3, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_READ_MEMORY2 CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x4, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_READ_MEMORY3 CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x5, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_READ_MEMORY4 CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x6, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_WRITE_MEMORY2 CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x7, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_GET_PROCESS_BASE_ADD CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x8, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_FIND_GUARDED_REGION CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x9, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_FIND_GUARDED_REGION2 CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x10, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_ALLOCMEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x11, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_ALLOCMEMORY2 CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x12, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_PROTECTVIRTUALMEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x13, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_FREEVIRTUALMEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x14, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_STREAMMODE CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x15, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_GETPEB CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x16, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_CR3PROCESSOR CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x17, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_PROCESSOR CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x18, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_CREATETHREAD CTL_CODE(FILE_DEVICE_UNKNOWN, COMMAND + 0x19, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

class _driver
{
	HANDLE driverHandler = nullptr; // driver object
public:
	uint32_t _processid;
	uint64_t _guardedregion = 0;
	uintptr_t ProcessPeb;
	uintptr_t BaseAddress;

	unsigned long long ProcessorCr3;
	auto readvm(uint64_t src_addr, uint64_t dst_addr, size_t size) -> ULONG
	{
		if (_processid == 0 || src_addr == 0) return 0;

		MEMORY_OPERATION_DATA requestData = { _processid, src_addr, dst_addr, size };

		return DeviceIoControl(
			driverHandler,                  // Device handle
			IOCTL_READ_MEMORY2,				// Control code
			&requestData,                   // Input buffer
			sizeof(requestData),            // Input buffer size
			nullptr,                   // Output buffer (reuse input buffer for output)
			0,            // Output buffer size
			nullptr,                        // Bytes returned
			nullptr                         // Overlapped structure (not using overlapped I/O in this example)
		);
	}

	auto writevm(uint32_t src_pid, uint64_t src_addr, uint64_t dst_addr, size_t size) -> void
	{
		if (src_pid == 0 || src_addr == 0) return;

		MEMORY_OPERATION_DATA requestData = { src_pid, src_addr, dst_addr, size };

		DeviceIoControl(
			driverHandler,                  // Device handle
			IOCTL_WRITE_MEMORY,				// Control code
			&requestData,                   // Input buffer
			sizeof(requestData),            // Input buffer size
			&requestData,                   // Output buffer (reuse input buffer for output)
			sizeof(requestData),            // Output buffer size
			nullptr,                        // Bytes returned
			nullptr                         // Overlapped structure (not using overlapped I/O in this example)
		);
	}


	auto initdriver(int processid) -> void
	{
		driverHandler = CreateFileA(
			"\\\\.\\KPRL",
			GENERIC_READ | GENERIC_WRITE,
			0,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
		_processid = processid;
		printf("DriverHandle : %p \n\n", driverHandler);
	}
	auto GetPeb() -> uintptr_t
	{
		MEMORY_OPERATION_DATA inputRequestData = { _processid };
		MEMORY_OPERATION_DATA outputRequestData = {};

		BOOL result = DeviceIoControl(
			driverHandler,
			IOCTL_GETPEB,
			&inputRequestData,
			sizeof(inputRequestData),
			&outputRequestData,
			sizeof(outputRequestData),
			nullptr,
			nullptr
		);

		if (!result) {
			std::cout << "[+] CANT GET PEB ;-; \n";
		}

		ProcessPeb = outputRequestData.processPEB;

		return outputRequestData.processPEB;
	}

	auto GetCr3() -> unsigned long long
	{
		MEMORY_OPERATION_DATA inputRequestData = { _processid };
		MEMORY_OPERATION_DATA outputRequestData = {};

		BOOL result = DeviceIoControl(
			driverHandler,                  // Device handle
			IOCTL_CR3PROCESSOR,				// Control code
			&inputRequestData,                   // Input buffer
			sizeof(inputRequestData),            // Input buffer size
			&outputRequestData,                   // Output buffer (reuse input buffer for output)
			sizeof(outputRequestData),            // Output buffer size
			nullptr,                        // Bytes returned
			nullptr                         // Overlapped structure (not using overlapped I/O in this example)
		);

		if (!result) {
			std::cout << "[+] CANT GET CR3 ;-; \n";
		}

		ProcessorCr3 = outputRequestData.curentCR3;
		return outputRequestData.curentCR3;
	}

	PVOID AllocMemory(ULONGLONG size, DWORD protect)
	{
		MEMORY_OPERATION_DATA inputRequestData = { _processid,NULL,NULL,NULL,size,protect };
		MEMORY_OPERATION_DATA outputRequestData = {};

		BOOL result = DeviceIoControl(
			driverHandler,                  // Device handle
			IOCTL_ALLOCMEMORY,				// Control code
			&inputRequestData,                   // Input buffer
			sizeof(inputRequestData),            // Input buffer size
			&outputRequestData,                   // Output buffer (reuse input buffer for output)
			sizeof(outputRequestData),            // Output buffer size
			nullptr,                        // Bytes returned
			nullptr                         // Overlapped structure (not using overlapped I/O in this example)
		);

		if (!result) {
			std::cout << "[+] CANT GET CR3 ;-; \n";
		}

		std::cout << "MyFuck ALLOC  " << outputRequestData.allocate_base << std::endl;
		return outputRequestData.allocate_base;
	}
	auto FreeMemory(PVOID base) -> ULONG
	{
		if (_processid == 0) return 0;

		MEMORY_OPERATION_DATA requestData;
		requestData.src_pid = _processid;
		requestData.allocate_base = base;
		return DeviceIoControl(
			driverHandler,                  // Device handle
			IOCTL_FREEVIRTUALMEMORY,				// Control code
			&requestData,                   // Input buffer
			sizeof(requestData),            // Input buffer size
			nullptr,                   // Output buffer (reuse input buffer for output)
			0,            // Output buffer size
			nullptr,                        // Bytes returned
			nullptr                         // Overlapped structure (not using overlapped I/O in this example)
		);
	}
	auto ProtecMemory(PVOID base, ULONGLONG size, DWORD protect) -> ULONG
	{
		if (_processid == 0) return 0;

		MEMORY_OPERATION_DATA requestData;
		requestData.src_pid = _processid;
		requestData.allocate_base = base;
		requestData.size = size;
		requestData.protect = protect;
		return DeviceIoControl(
			driverHandler,                  // Device handle
			IOCTL_PROTECTVIRTUALMEMORY,				// Control code
			&requestData,                   // Input buffer
			sizeof(requestData),            // Input buffer size
			nullptr,                   // Output buffer (reuse input buffer for output)
			0,            // Output buffer size
			nullptr,                        // Bytes returned
			nullptr                         // Overlapped structure (not using overlapped I/O in this example)
		);
	}


	auto guarded_region() -> uintptr_t
	{
		GET_GUARDED_REGION requestData = { };

		DeviceIoControl(
			driverHandler,                  // Device handle
			IOCTL_FIND_GUARDED_REGION,		// Control code
			&requestData,                   // Input buffer
			sizeof(requestData),            // Input buffer size
			&requestData,                   // Output buffer (reuse input buffer for output)
			sizeof(requestData),            // Output buffer size
			nullptr,                        // Bytes returned
			nullptr                         // Overlapped structure (not using overlapped I/O in this example)
		);

		_guardedregion = requestData.allocation;
		return requestData.allocation;
	}


	template <typename T>
	bool readvb(uintptr_t src, T& result, size_t size = sizeof(T))
	{
		T ret = {};
		if (readvm((uint64_t)(src), (uint64_t)&ret, sizeof(T)))
		{
			result = ret;
			return true;
		}

		result = T();
		return false;
	}

	DWORD64 TraceAddress(DWORD64 BaseAddress, std::vector<DWORD> Offsets)
	{
		DWORD64 Address = 0;

		if (Offsets.size() == 0)
			return BaseAddress;

		if (!readvb<DWORD64>(BaseAddress, Address))
			return 0;

		for (int i = 0; i < Offsets.size() - 1; i++)
		{
			if (!readvb<DWORD64>(Address + Offsets[i], Address))
				return 0;
		}
		return Address == 0 ? 0 : Address + Offsets[Offsets.size() - 1];
	}

	template <typename T>
	T readguarded(uintptr_t src, size_t size = sizeof(T))
	{
		T buffer;
		readvm(src, (uintptr_t)&buffer, size);
		uintptr_t val = _guardedregion + (*(uintptr_t*)&buffer & 0xFFFFFF);
		return *(T*)&val;
	}
	bool ReadMem(uint64_t base, uint64_t buffer, SIZE_T size)
	{
		ULONG Value = readvm(base, buffer, size);
		if (0 == Value)
			return true;
		return false;
	}

	template <typename T>
	T readv(uintptr_t src, size_t size = sizeof(T))
	{
		T buffer;
		readvm(src, (uintptr_t)&buffer, size);
		return buffer;
	}

	template <typename T>
	void write(uint64_t src, T data, size_t size = sizeof(T))
	{
		writevm(_processid, src, (uint64_t)&data, size);
	}

	template<typename T>
	void readarray(uint64_t address, T* array, size_t len)
	{
		readvm(address, (uintptr_t) & array, sizeof(T) * len);
	}

	//bluefire1337
	inline static bool isguarded(uintptr_t pointer) noexcept
	{
		static constexpr uintptr_t filter = 0xFFFFFFF000000000;
		uintptr_t result = pointer & filter;
		return result == 0x8000000000 || result == 0x10000000000;
	}

	template <typename T>
	T read(T src)
	{
		T buffer = readv< uintptr_t >(src);

		if (isguarded((uintptr_t)buffer))
		{
			return readguarded< uintptr_t >(src);
		}

		return buffer;
	}

	auto move_mouse(LONG x, LONG y, USHORT button_flags) -> void
	{
		MOUSE_MOVE_DATA mouseData = { x, y, button_flags };

		DeviceIoControl(
			driverHandler,                  // Device handle
			IOCTL_MOVE_MOUSE,				// Control code
			&mouseData,						// Input buffer
			sizeof(mouseData),				// Input buffer size
			&mouseData,						// Output buffer (reuse input buffer for output)
			sizeof(mouseData),				// Output buffer size
			nullptr,                        // Bytes returned
			nullptr                         // Overlapped structure (not using overlapped I/O in this example)
		);
	}


	auto get_base_address() -> UINT64
	{
		GET_PROCESS_BASE_ADDRESS_DATA requestData = { _processid };

		DeviceIoControl(
			driverHandler,                  // Device handle
			IOCTL_GET_PROCESS_BASE_ADD,     // Control code
			&requestData,                   // Input buffer
			sizeof(requestData),            // Input buffer size
			&requestData,                   // Output buffer (reuse input buffer for output)
			sizeof(requestData),            // Output buffer size
			nullptr,                        // Bytes returned
			nullptr                         // Overlapped structure (not using overlapped I/O in this example)
		);
		BaseAddress = requestData.baseAddress;
		return requestData.baseAddress;
	}

	auto getprocessid(const std::wstring& processname) -> DWORD
	{
		auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot == INVALID_HANDLE_VALUE) {
			return 0; // Handle error
		}

		PROCESSENTRY32W entry;
		entry.dwSize = sizeof(PROCESSENTRY32W); // Ensure this matches the wide version

		if (!Process32FirstW(snapshot, &entry)) { // Use Process32FirstW for wide characters
			CloseHandle(snapshot);
			return 0; // Handle error
		}

		do {
			if (processname.compare(entry.szExeFile) == 0) {
				CloseHandle(snapshot);
				return entry.th32ProcessID;
			}
		} while (Process32NextW(snapshot, &entry)); // Use Process32NextW

		CloseHandle(snapshot);
		return 0;
	}
};

static _driver d;