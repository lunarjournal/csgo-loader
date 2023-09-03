#include "stdafx.hpp"
#include <iostream>
#include <Windows.h>
#include <wininet.h>
#include "injector.hpp"
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


int vac_inhibitor(LPCSTR dllUrl)
{
	std::atexit( []()
	{
		std::cin.get();
	} );

	HKEY h_key = nullptr;
	if ( RegOpenKeyExA( HKEY_CURRENT_USER, "Software\\Valve\\Steam", 0, KEY_QUERY_VALUE, &h_key ) != ERROR_SUCCESS )
	{
		RegCloseKey( h_key );

		std::cout << "failed to find steam registry" << std::endl;
		return EXIT_FAILURE;
	}

	char steam_path_reg[MAX_PATH]; steam_path_reg[0] = '"';
	DWORD steam_path_size = sizeof( steam_path_reg ) - sizeof( char );

	if ( RegQueryValueExA( h_key, "SteamExe", nullptr, nullptr, (LPBYTE)( steam_path_reg + 1 ), &steam_path_size ) != ERROR_SUCCESS )
	{
		RegCloseKey( h_key );

		std::cout << "failed to get steam path" << std::endl;
		return EXIT_FAILURE;
	}
	RegCloseKey( h_key );

	auto steam_path = std::string( steam_path_reg ) + "\"";

	do
	{
		memory::kill_process( "csgo.exe" );
		memory::kill_process( "steam.exe" );
		memory::kill_process( "steamservice.exe" );
		memory::kill_process( "steamwebhelper.exe" );

		if ( !memory::is_process_open( "steam.exe" ) && !memory::is_process_open( "steamservice.exe" ) && !memory::is_process_open( "steamwebhelper.exe" ) )
			break;

		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
	while ( true );

	PROCESS_INFORMATION pi;
	if ( !memory::open_process( steam_path, { "-console" }, pi ) )
	{
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );

		std::cout << "failed to open steam.exe" << std::endl;
		return EXIT_FAILURE;
	}

	if ( memory::is_process_open( "steamservice.exe" ) )
	{
		std::cout << "steamservice is running, steam did not run as admin" << std::endl;
		return EXIT_FAILURE;
	}

	LPCSTR processName = FindProcessName(pi.dwProcessId);
	injector(dllUrl, processName, false);

	int startGameResult = system("start steam://rungameid/730");
	
	if (!!startGameResult)
	{
		std::cout << "[-] Fail with open csgo" << std::endl;
		return EXIT_FAILURE;
	}

	do
	{
		if (memory::is_process_open("csgo.exe"))
			break;

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	} while (true);

	std::cout << "Waiting injection..." << std::endl;

	return EXIT_SUCCESS;
}
