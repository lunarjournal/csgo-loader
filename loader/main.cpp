#include "stdafx.hpp"
#include "vac_inhibitor.hpp"
#include "injector.hpp"
#include <windows.h>
#include <conio.h>
#include <iostream>

const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string BLUE = "\033[34m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";

void menu()
{
	system("cls");

	std::string banner = R"(
            _             
           (_)            
  ___  _ __ _  ___  _ __  
 / _ \| '__| |/ _ \| '_ \ 
| (_) | |  | | (_) | | | |
 \___/|_|  |_|\___/|_| |_|
                          
                          
)";

	std::cout << banner << std::endl;

	std::cout << YELLOW << "[!] Osiris is used by default" << RESET << std::endl << std::endl;

	std::cout << BLUE << "[1] Use custom cheat DLL (Optional)" << RESET << std::endl;
	std::cout << BLUE << "[2] Use custom VAC_Inhibitor DLL (Optional)" << RESET << std::endl;
	std::cout << GREEN << "[3] Inject" << RESET << std::endl;

	std::cout << "> ";
}

bool loader(LPCSTR vac_inhibitor_path_or_url, LPCSTR cheat_path_or_url, bool remote_cheat_dll, bool remote_vac_inhibitor_dll)
{
	if (remote_vac_inhibitor_dll)
	{
		if (!verifyDll(vac_inhibitor_path_or_url)) // verify vac_inhibitor before open steam
		{
			std::cerr << RED << "[-] Invalid URL for VAC_Inhibitor DLL" << RESET << std::endl;
			std::cerr << RED << "[!] DLL URL: " << vac_inhibitor_path_or_url << RESET << std::endl;
			std::cerr << RED << "[!] Please, update the URL VAC_Inhibitor DLL" << RESET << std::endl;

			return false;
		}
	}

	if (remote_cheat_dll)
	{
		if (!verifyDll(cheat_path_or_url)) // verify cheat_url before open steam and game
		{
			std::cerr << RED << "[-] Invalid URL for cheat DLL" << RESET << std::endl;
			std::cerr << RED << "[!] DLL URL: " << cheat_path_or_url << RESET << std::endl;
			std::cerr << RED << "[!] Please, update the URL cheat DLL" << RESET << std::endl;

			return false;
		}
	}
	
	int vacDisabled = !vac_inhibitor(vac_inhibitor_path_or_url, remote_vac_inhibitor_dll);

	if (vacDisabled)
	{

		int verifyDllLoaded = !injector(cheat_path_or_url, "csgo.exe", remote_cheat_dll, true);

		if (verifyDllLoaded)
		{
			Beep(500, 100);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			Beep(500, 100);

			return true;
		}

		Beep(300, 100);
	}

	return false;
}

int main()
{
	std::string cheat_path_or_url = "https://cdn.discordapp.com/attachments/1136059719484719175/1147774626060062730/Osiris.dll";
	std::string vac_inhibitor_path_or_url = "https://cdn.discordapp.com/attachments/1136059719484719175/1147729662701748224/vac3_inhibitor.dll";

	bool remote_cheat_dll = true;
	bool remote_vac_inhibitor_dll = true;

	bool shouldExit = false;

	do
	{
		menu();
		char userInput = _getch();
		std::string dllPathOrUrl;

		
		switch (userInput)
		{
			case('1'):
				do
				{
					system("cls");

					std::cout << BLUE << "[1] Local DLL" << RESET << std::endl;
					std::cout << BLUE << "[2] Remote DLL" << RESET << std::endl;

					userInput = _getch();

					if (userInput == '1')
					{
						system("cls");

						std::cout << "DLL Path> ";
						std::getline(std::cin, dllPathOrUrl);

						remote_cheat_dll = false;
						break;
					}

					else if (userInput == '2')
					{
						system("cls");

						std::cout << "URL> ";
						std::getline(std::cin, dllPathOrUrl);
						
						break;
					}
					
					else
					{
						continue;
					}

				} while (true);

				cheat_path_or_url = dllPathOrUrl;
				continue;

			case('2'):
				do
				{
					system("cls");

					std::cout << BLUE << "[1] Local DLL" << RESET << std::endl;
					std::cout << BLUE << "[2] Remote DLL" << RESET << std::endl;

					userInput = _getch();

					if (userInput == '1')
					{
						system("cls");

						std::cout << "DLL Path> ";
						std::getline(std::cin, dllPathOrUrl);

						remote_vac_inhibitor_dll = false;
						break;
					}

					else if (userInput == '2')
					{
						system("cls");

						std::cout << "URL> ";
						std::getline(std::cin, dllPathOrUrl);

						break;
					}

					else
					{
						continue;
					}

				} while (true);

				vac_inhibitor_path_or_url = dllPathOrUrl;
				continue;
			case('3'):
				system("cls");
				shouldExit = true;
				break;
			default:
				break;
		}

		if (shouldExit)
		{
			break;
		}

		system("cls");
		continue;

	} while (true);

	if (!loader(vac_inhibitor_path_or_url.c_str(), cheat_path_or_url.c_str(), remote_cheat_dll, remote_vac_inhibitor_dll))
	{
		system("PAUSE");
		ExitProcess(1); // force exit
	}

	ExitProcess(0); // force exit
}