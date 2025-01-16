#pragma once
#include "iea.h"


HRESULT MyAssemblyStore_QueryInterface(IHostAssemblyStore* This, REFIID riid, void** ppvObject);
ULONG MyAssemblyStore_AddRef(IHostAssemblyStore* This);
ULONG MyAssemblyStore_Release(IHostAssemblyStore* This);
HRESULT MyAssemblyStore_ProvideAssembly(IHostAssemblyStore* This, AssemblyBindInfo* pBindInfo, UINT64* pAssemblyId, UINT64* pContext, IStream** ppStmAssemblyImage, IStream** ppStmPDB);
HRESULT MyAssemblyStore_ProvideModule(IHostAssemblyStore* This, ModuleBindInfo* pBindInfo, DWORD* pdwModuleId, IStream** ppStmModuleImage, IStream** ppStmPDB);

#undef DUMMY_METHOD
#define DUMMY_METHOD(x) HRESULT ( STDMETHODCALLTYPE *dummy_##x )(IHostAssemblyStore *This)
typedef struct _MyAssemblyStore {
	BEGIN_INTERFACE

	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IHostAssemblyStore* This, REFIID riid, void** ppvObject);
	ULONG(STDMETHODCALLTYPE* AddRef)(IHostAssemblyStore* This);
	ULONG(STDMETHODCALLTYPE* Release)(IHostAssemblyStore* This);
	HRESULT(STDMETHODCALLTYPE* ProvideAssembly)(IHostAssemblyStore* This, AssemblyBindInfo* pBindInfo, UINT64* pAssemblyId, UINT64* pContext, IStream** ppStmAssemblyImage, IStream** ppStmPDB);
	HRESULT(STDMETHODCALLTYPE* ProvideModule)(IHostAssemblyStore* This, ModuleBindInfo* pBindInfo, DWORD* pdwModuleId, IStream** ppStmModuleImage, IStream** ppStmPDB);

	END_INTERFACE

} MyAssemblyStoreVtbl;

typedef struct __MyAssemblyStore {
	MyAssemblyStoreVtbl* lpVtbl;
	TargetAssembly* targetAssembly;
	DWORD count;
} MyAssemblyStore;

static const MyAssemblyStoreVtbl MyAssemblyStore_Vtbl = {
	MyAssemblyStore_QueryInterface,
	MyAssemblyStore_AddRef,
	MyAssemblyStore_Release,
	MyAssemblyStore_ProvideAssembly,
	MyAssemblyStore_ProvideModule
};
