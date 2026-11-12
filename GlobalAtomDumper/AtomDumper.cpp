#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

int main()
{
	SetConsoleTitle( _T("Global Atom Dumper") );
	
	HRESULT hr = OleInitialize(NULL);
	if (FAILED(hr))
	{
		fprintf( stderr, "Unable to initialize OLE.\nError code: 0x%08lX\n", hr );
		_getch();
		return 1;
	}

	printf( "#######\n" );

	for (ATOM i = 0xC000; i; i++)
	{
		TCHAR szAtomName[255];
		if (GlobalGetAtomName( i, szAtomName, 255 ))
			_tprintf( _T("Found Atom 0x%hX = %s\n"), i, szAtomName ); 
		else
		{
			const DWORD dwLastError = GetLastError();
			if (dwLastError == ERROR_INVALID_HANDLE)
				continue;

			fprintf( stderr, "GetAtomName failed with error code %d\n", dwLastError );
		}
	}

	printf( "#######\n" );

	OleUninitialize();
	_getch();
	return 0;
}