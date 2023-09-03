#pragma once

int injector(LPCSTR Dll, LPCSTR processName, bool waitWindow = false);
DWORD __stdcall LibraryLoader(LPVOID Memory);
DWORD __stdcall stub();
PVOID remoteDownloadDll(LPCSTR dllUrl, DWORD& dllSize);
LPCSTR FindProcessName(DWORD processId);
bool verifyDll(LPCSTR dllUrl);