#define _CRT_SECURE_NO_WARNINGS

#include "service.h"
#include <stdlib.h>
#include <process.h>

#define GetLastErrorAsHR (HRESULT_FROM_WIN32(GetLastError()))

// Constructor and Destructor

CStationService::CStationService(void)
	: m_cRef( 0 )
{

}

CStationService::~CStationService(void)
{

}

// IUnknown Members

STDMETHODIMP CStationService::QueryInterface(REFIID riid, void **ppv)
{
    HRESULT hr = S_OK;
    if (ppv)
    {
        *ppv = NULL;

        if (IsEqualIID( riid, IID_IDispatch ) || IsEqualIID( riid, IID_IUnknown ))
            *ppv = (IDispatch *) (this);
        else
            hr = E_NOINTERFACE;

        if (*ppv)
            AddRef();
    }
    else
        hr = E_INVALIDARG;

    return hr;
}

STDMETHODIMP_(ULONG) CStationService::AddRef(void)
{
    return((ULONG) (InterlockedIncrement((long *) &m_cRef)));
}

STDMETHODIMP_(ULONG) CStationService::Release(void)
{
    ULONG cRef = (ULONG) (InterlockedDecrement((long *) &m_cRef));
    if (!cRef)
        delete this;

    return cRef;
}

// IDispatch Members

STDMETHODIMP CStationService::GetTypeInfoCount(UINT FAR* pctInfo)
{
    if (!pctInfo)
        return E_INVALIDARG;

    *pctInfo = 0;
    return S_OK;
}

STDMETHODIMP CStationService::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo FAR* FAR* ppTInfo)
{
    return E_NOTIMPL;
}

STDMETHODIMP CStationService::GetIDsOfNames(REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames, LCID lcid, DISPID FAR* rgDispId)
{
    HRESULT hr = S_OK;

    for (UINT i = 0; i < cNames; i++)
    {
        if (!_wcsicmp( rgszNames[i], L"EnumAtoms" ))
			rgDispId[i] = DISPID_STATION_EnumAtoms;
        else if (!_wcsicmp( rgszNames[i], L"CreateAtom" ))
			rgDispId[i] = DISPID_STATION_CreateAtom;
        else if (!_wcsicmp( rgszNames[i], L"DestroyAtom" ))
			rgDispId[i] = DISPID_STATION_DestroyAtom;
        else
        {
            rgDispId[i] = DISPID_UNKNOWN;
            hr = DISP_E_UNKNOWNNAME;
        }
    }

    return hr;
}

STDMETHODIMP CStationService::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR* pDispParams, VARIANT FAR* pVarResult, EXCEPINFO FAR* pExcepInfo, UINT FAR* puArgErr)
{
	try
	{
		if (!pDispParams)
			_com_issue_error( E_INVALIDARG );

		if (!IsEqualIID( riid, IID_NULL ))
			_com_issue_error( DISP_E_UNKNOWNINTERFACE );

		HRESULT hr = S_OK;
		_variant_t varTempResult;

		switch (dispIdMember)
		{
		case DISPID_STATION_EnumAtoms :
			{
				if (!(wFlags & DISPATCH_METHOD))
					_com_issue_error( DISP_E_MEMBERNOTFOUND );

				if (pDispParams->cArgs != 1)
					_com_issue_error( DISP_E_BADPARAMCOUNT );

				_variant_t varAtomEnumCallback( pDispParams->rgvarg );

				EnumAtoms( varAtomEnumCallback );

				break;
			}
		case DISPID_STATION_CreateAtom :
			{
				if (!(wFlags & DISPATCH_METHOD))
					_com_issue_error( DISP_E_MEMBERNOTFOUND );

				if (pDispParams->cArgs != 1)
					_com_issue_error( DISP_E_BADPARAMCOUNT );

				_variant_t varAtomName( pDispParams->rgvarg );
				varAtomName.ChangeType( VT_BSTR );

				varTempResult = CreateAtom( varAtomName.bstrVal );

				break;
			}
		case DISPID_STATION_DestroyAtom :
			{
				if (!(wFlags & DISPATCH_METHOD))
					_com_issue_error( DISP_E_MEMBERNOTFOUND );

				if (pDispParams->cArgs != 1)
					_com_issue_error( DISP_E_BADPARAMCOUNT );

				_variant_t varAtom( pDispParams->rgvarg );

				DestroyAtom( varAtom );

				break;
			}
		default:
			{
				_com_issue_error( DISP_E_MEMBERNOTFOUND );
				break;
			}
		}

		if (pVarResult)
		{
			hr = VariantCopy( pVarResult, &varTempResult );
			if (FAILED(hr))
				_com_issue_error( hr );
		}
	}
	catch (_com_error &ce)
	{
		return ce.Error();
	}

	return S_OK;
}

// StationService Members

void STDMETHODCALLTYPE CStationService::EnumAtoms(IDispatchPtr pAtomEnumCallback)
{
	HRESULT hr = S_OK;

	for (ATOM x = 0xC000; x; x++)
	{
		WCHAR szAtomName[255];
		if (GlobalGetAtomNameW( x, szAtomName, 255 ))
		{
			_variant_t varAtomInfo[2], varProceed;
			DISPPARAMS dp = { varAtomInfo, NULL, 2, 0 };

			varAtomInfo[0] = x;
			varAtomInfo[1] = szAtomName;

			hr = pAtomEnumCallback->Invoke( DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dp, &varProceed, NULL, NULL );
			if (FAILED(hr))
				_com_issue_error( hr );

			if (!varProceed)
				break;
		}
		else
		{
			hr = GetLastErrorAsHR;
			if (hr != E_HANDLE)
				_com_issue_error( hr );
		}
	}
}

ATOM STDMETHODCALLTYPE CStationService::CreateAtom(BSTR bsAtomName)
{
	ATOM x = GlobalAddAtomW( bsAtomName );
	if (!x)
		_com_issue_error( GetLastErrorAsHR );

	return x;
}

UINT WINAPI CStationService::DestroyAtomCore(LPVOID lpAtom)
{
	if (FAILED(OleInitialize( NULL )))
		return 1;

	SetLastError( ERROR_SUCCESS );
	do
	{
		GlobalDeleteAtom( (ATOM) lpAtom );
	} while (GetLastError() == ERROR_SUCCESS);

	TCHAR szAtomName[255];
	const UINT res = GlobalGetAtomName( (ATOM) lpAtom, szAtomName, 255 ) ? 1U : (GetLastError() != ERROR_INVALID_HANDLE);
	OleUninitialize();
	return res;
}

void STDMETHODCALLTYPE CStationService::DestroyAtom(ATOM atom)
{
	HRESULT hr = S_OK;
	HANDLE hCoreThread = (HANDLE) _beginthreadex( NULL, 0, DestroyAtomCore, (LPVOID) atom, 0, NULL );
	if (!hCoreThread)
		_com_issue_error( HRESULT_FROM_WIN32( _doserrno ) );

	switch (WaitForSingleObject( hCoreThread, 3000UL ))
	{
	case WAIT_TIMEOUT :
		{
			hr = TerminateThread( hCoreThread, 1UL ) ? E_FAIL : GetLastErrorAsHR;

			break;
		}
	case WAIT_FAILED :
		{
			hr = GetLastErrorAsHR;

			break;
		}
	}

	CloseHandle( hCoreThread );

	if (FAILED(hr))
		_com_issue_error( hr );
}