#pragma once
#include <Windows.h>
#include <mscoree.h>
#include <metahost.h>

#include "MemoryManager.h"

static GUID xIID_IHostControl = { 0x02CA073C, 0x7079, 0x4860, {0x88, 0x0A, 0xC2, 0xF7, 0xA4, 0x49, 0xC9, 0x91} };

#undef DUMMY_METHOD
#define DUMMY_METHOD(x) HRESULT ( STDMETHODCALLTYPE *dummy_##x )(IHostControl *This)
typedef struct _MyHostControlVtbl {
	BEGIN_INTERFACE

	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IHostControl* This, REFIID riid, void** ppvObject);
	ULONG(STDMETHODCALLTYPE* AddRef)(IHostControl* This);
	ULONG(STDMETHODCALLTYPE* Release)(IHostControl* This);

	HRESULT(STDMETHODCALLTYPE* GetHostManager)(IHostControl* This, REFIID riid, void** ppObject);
	HRESULT(STDMETHODCALLTYPE* SetAppDomainManager)(IHostControl* This, DWORD dwAppDomainID, IUnknown* pUnkAppDomainManager);// { return E_NOTIMPL; }

	END_INTERFACE

} MyHostControlVtbl;

//lpVtbl and count are required in this struct, the other two are just my implementation
typedef struct __MyHostControl {
	MyHostControlVtbl* lpVtbl;
	TargetAssembly* targetAssembly;
	MemoryManager* memoryManager;
	DWORD count;
} MyHostControl;

HRESULT MyHostControl_QueryInterface(MyHostControl* This, REFIID riid, void** ppvObject);
ULONG MyHostControl_AddRef(MyHostControl* This);
ULONG MyHostControl_Release(MyHostControl* This);

HRESULT MyHostControl_GetHostManager(MyHostControl* This, REFIID riid, void** ppObject);
HRESULT MyHostControl_SetAppDomainManager(MyHostControl* This, DWORD dwAppDomainID, IUnknown* pUnkAppDomainManager);

static const MyHostControlVtbl MyHostControl_Vtbl = {
	MyHostControl_QueryInterface,
	MyHostControl_AddRef,
	MyHostControl_Release,
	MyHostControl_GetHostManager,
	MyHostControl_SetAppDomainManager
};