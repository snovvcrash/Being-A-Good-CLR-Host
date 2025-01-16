#pragma once
#include "iea.h"
#include "AssemblyStore.h"

HRESULT MyAssemblyManager_QueryInterface(IHostAssemblyManager* This, REFIID riid, void** ppvObject);
ULONG MyAssemblyManager_AddRef(IHostAssemblyManager* This);
ULONG MyAssemblyManager_Release(IHostAssemblyManager* This);
HRESULT MyAssemblyManager_GetNonHostStoreAssemblies(IHostAssemblyManager* This, ICLRAssemblyReferenceList** ppReferenceList);
HRESULT MyAssemblyManager_GetAssemblyStore(IHostAssemblyManager* This, IHostAssemblyStore** ppAssemblyStore);

#undef DUMMY_METHOD
#define DUMMY_METHOD(x) HRESULT ( STDMETHODCALLTYPE *dummy_##x )(IHostAssemblyManager *This)
typedef struct _MyAssemblyManagerVtbl {
	BEGIN_INTERFACE

	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IHostAssemblyManager* This, REFIID riid, void** ppvObject);
	ULONG(STDMETHODCALLTYPE* AddRef)(IHostAssemblyManager* This);
	ULONG(STDMETHODCALLTYPE* Release)(IHostAssemblyManager* This);

	HRESULT(STDMETHODCALLTYPE* GetNonHostStoreAssemblies)(IHostAssemblyManager* This, ICLRAssemblyReferenceList** ppReferenceList);
	HRESULT(STDMETHODCALLTYPE* GetAssemblyStore)(IHostAssemblyManager* This, IHostAssemblyStore** ppAssemblyStore);

	END_INTERFACE

} MyAssemblyManagerVtbl;

//lpVtbl and count are required items in this struct, the other two are my implementation
typedef struct __MyAssemblyManager {
	MyAssemblyManagerVtbl* lpVtbl;
	IHostAssemblyStore* assemblyStore;
	TargetAssembly* targetAssembly;
	DWORD count;
} MyAssemblyManager;

static const MyAssemblyManagerVtbl MyAssemblyManager_Vtbl = {
	MyAssemblyManager_QueryInterface,
	MyAssemblyManager_AddRef,
	MyAssemblyManager_Release,
	MyAssemblyManager_GetNonHostStoreAssemblies,
	MyAssemblyManager_GetAssemblyStore
};