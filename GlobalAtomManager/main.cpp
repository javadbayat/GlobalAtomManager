#include <Windows.h>
#include <new>
#include "service.h"

#ifdef _WIN64
	#pragma comment(lib, "libstation.x64.lib")
#else
	#pragma comment(lib, "libstation.x86.lib")
#endif

BOOL WINAPI DllMain(HINSTANCE hInstance, ULONG ulReason, LPVOID pvReserved)
{
	return TRUE;
}

STDAPI StationGetObject(LPUNKNOWN *ppStation)
{
	try
	{
		(*ppStation = new CStationService)->AddRef();
	}
	catch (std::bad_alloc &)
	{
		return E_OUTOFMEMORY;
	}

	return S_OK;
}