#pragma once
#include <Windows.h>
#include <comdef.h>

#define DISPID_STATION_EnumAtoms DISPID_VALUE
#define DISPID_STATION_CreateAtom 1L
#define DISPID_STATION_DestroyAtom 2L

class CStationService : public IDispatch
{
private:
	ULONG m_cRef;
public:
	CStationService(void);
	~CStationService(void);

    // IUnknown Members
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    // IDispatch Members
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT FAR* pctInfo);
    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo FAR* FAR* ppTInfo);
    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames, LCID lcid, DISPID FAR* rgDispId);
	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR* pDispParams, VARIANT FAR* pVarResult, EXCEPINFO FAR* pExcepInfo, UINT FAR* puArgErr);

	// StationService Members
	void STDMETHODCALLTYPE EnumAtoms(IDispatchPtr pAtomEnumCallback);
	ATOM STDMETHODCALLTYPE CreateAtom(BSTR bsAtomName);
	void STDMETHODCALLTYPE DestroyAtom(ATOM atom);

	static UINT WINAPI DestroyAtomCore(LPVOID);
};