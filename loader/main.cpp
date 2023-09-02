#include "stdafx.hpp"
#include "vac_inhibitor.hpp"
#include "injector.hpp"
#include <windows.h>

int main()
{

	int vacDisabled = !vac_inhibitor();

	if (vacDisabled)
	{
		LPCSTR dllPath = "C:\\Users\\muril\\Downloads\\Osiris\\Release\\Osiris.dll";
		loader(dllPath);

		ExitProcess(0);
		return 0;
	}

	return 1;
}