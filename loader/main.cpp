#include "stdafx.hpp"
#include "vac_inhibitor.hpp"
#include "injector.hpp"
#include <windows.h>
#include <conio.h>

bool loader(LPCSTR vac_inhibitor_url, LPCSTR cheat_url)
{
	if (!verifyDll(vac_inhibitor_url)) // verify vac_inhibitor before open steam
	{
		std::cerr << "[-] Invalid URL for VAC_Inhibitor DLL" << std::endl;
		std::cerr << "[!] DLL URL: " << vac_inhibitor_url << std::endl;
		std::cerr << "[!] Please, update the URL VAC_Inhibitor DLL" << std::endl;

		return false;
	}

	if (!verifyDll(cheat_url)) // verify cheat_url before open steam and game
	{
		std::cerr << "[-] Invalid URL for cheat DLL" << std::endl;
		std::cerr << "[!] DLL URL: " << cheat_url << std::endl;
		std::cerr << "[!] Please, update the URL cheat DLL" << std::endl;

		return false;
	}
	
	int vacDisabled = !vac_inhibitor(vac_inhibitor_url);

	if (vacDisabled)
	{

		int verifyDllLoaded = !injector(cheat_url, "csgo.exe", 1);

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

void menu()
{
	std::cout << "[0] Use Osiris DLL" << std::endl;
	std::cout << "[1] Use default VAC_Inhibitor DLL" << std::endl;
	std::cout << "[2] Use custom DLL" << std::endl;
	std::cout << "[3] Use custom URL to VAC_Inhibitor DLL" << std::endl;
	std::cout << "[4] Inject" << std::endl;
	std::cout << "[5] Default Injection" << std::endl;

	std::cout << "> ";
}

int main()
{
	std::string cheat_url;
	std::string vac_inhibitor_url;

	do
	{
		menu();
		char userInput = _getch();
		
		if (userInput == '0')
		{
			system("cls");
			cheat_url = "https://cdn.discordapp.com/attachments/1136059719484719175/1147774626060062730/Osiris.dll";
		}

		if (userInput == '1')
		{
			system("cls");
			vac_inhibitor_url = "https://cdn.discordapp.com/attachments/1136059719484719175/1147729662701748224/vac3_inhibitor.dll";
		}

		if (userInput == '2')
		{
			system("cls");

			std::string urlInput;
			std::cout << "URL> ";
			std::getline(std::cin, urlInput);

			cheat_url = urlInput;
		}

		if (userInput == '3')
		{
			system("cls");

			std::string urlInput;
			std::cout << "URL> ";
			std::getline(std::cin, urlInput);

			vac_inhibitor_url = urlInput;
		}
		
		if (userInput == '4')
		{
			system("cls");

			if (vac_inhibitor_url.empty())
			{
				std::cout << "Select VAC_Inhibitor default or set URL with VAC_Inhibitor DLL" << std::endl << std::endl;
				continue;
			}

			if (cheat_url.empty())
			{
				std::cout << "Select Osiris or set URL with cheat DLL" << std::endl << std::endl;
				continue;
			}

			break;
		}

		if (userInput == '5')
		{
			system("cls");

			vac_inhibitor_url = "https://cdn.discordapp.com/attachments/1136059719484719175/1147729662701748224/vac3_inhibitor.dll";
			cheat_url = "https://cdn.discordapp.com/attachments/1136059719484719175/1147774626060062730/Osiris.dll";

			break;
		}

		system("cls");
		continue;



	} while (true);

	if (!loader(vac_inhibitor_url.c_str(), cheat_url.c_str()))
	{
		system("PAUSE");
		ExitProcess(1); // force exit
	}

	ExitProcess(0); // force exit
}