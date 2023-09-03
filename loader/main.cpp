#include "stdafx.hpp"
#include "vac_inhibitor.hpp"
#include "injector.hpp"
#include <windows.h>

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

int main()
{
	LPCSTR vac_inhibitor_url = "https://cdn.discordapp.com/attachments/1136059719484719175/1147729662701748224/vac3_inhibitor.dll";
	LPCSTR cheat_url = "https://cdn.discordapp.com/attachments/1136059719484719175/1147774626060062730/Osiris.dll";

	if (!loader(vac_inhibitor_url, cheat_url))
	{
		system("PAUSE");
		ExitProcess(1); // force exit
	}

	ExitProcess(0); // force exit
}