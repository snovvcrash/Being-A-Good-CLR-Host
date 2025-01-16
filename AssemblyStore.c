#include "AssemblyStore.h"
#include <objbase.h>

HRESULT STDMETHODCALLTYPE MyAssemblyStore_QueryInterface(MyAssemblyStore* this, REFIID vTableGuid, void** ppv) {
	if (!IsEqualIID(vTableGuid, &IID_IUnknown) && !IsEqualIID(vTableGuid, &xIID_IHostAssemblyStore)) {
		*ppv = 0;
		return E_NOINTERFACE;
	}
	*ppv = this;
	this->lpVtbl->AddRef(this);
	return S_OK;
}

ULONG STDMETHODCALLTYPE MyAssemblyStore_AddRef(MyAssemblyStore* this) {
	return(++((MyAssemblyStore*)this)->count);
}

ULONG STDMETHODCALLTYPE MyAssemblyStore_Release(MyAssemblyStore* this) {
	if (--((MyAssemblyStore*)this)->count == 0) {
		GlobalFree(this);
		return 0;
	}
	return ((MyAssemblyStore*)this)->count;
}

HRESULT STDMETHODCALLTYPE MyAssemblyStore_ProvideAssembly(MyAssemblyStore* this, AssemblyBindInfo* pBindInfo, UINT64* pAssemblyId, UINT64* pContext, IStream** ppStmAssemblyImage, IStream** ppStmPDB) {

	//Check if the identity of the assembly being loaded is the one we want
	if (wcscmp(this->targetAssembly->assemblyInfo, pBindInfo->lpPostPolicyIdentity) == 0) {
		
		//This isn't used for anything here so just set it to 0
		*pContext = 0;
		//This just can't be set to 0 or the CLR will start running into issues finding types
		UINT64 id = 50000;
		*pAssemblyId = id;

		//Create an IStream using our in-memory assembly bytes and return it to the CLR
		IStream* assemblyStream = SHCreateMemStream(this->targetAssembly->assemblyBytes, this->targetAssembly->assemblySize);
		*ppStmAssemblyImage = assemblyStream;
		return S_OK;

	}

	//If it's not our assembly then tell the CLR to handle it
	return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
}

//This shouldn't really get called but if it does we'll just tell the CLR to find it
HRESULT STDMETHODCALLTYPE MyAssemblyStore_ProvideModule(MyAssemblyStore* this,	ModuleBindInfo* pBindInfo,	DWORD* pdwModuleId,	IStream** ppStmModuleImage,	IStream** ppStmPDB) {

	printf("Loading module: %ls\n", pBindInfo->lpAssemblyIdentity);

	//Tell the CLR to handle this
	return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
}
