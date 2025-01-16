#include "AssemblyManager.h"

HRESULT STDMETHODCALLTYPE MyAssemblyManager_QueryInterface(IHostAssemblyManager* this, REFIID vTableGuid, void** ppv) {
	if (!IsEqualIID(vTableGuid, &IID_IUnknown) && !IsEqualIID(vTableGuid, &xIID_IHostAssemblyManager)) {
		*ppv = 0;
		return E_NOINTERFACE;
	}
	*ppv = this;
	this->lpVtbl->AddRef(this);
	return S_OK;
}

ULONG STDMETHODCALLTYPE MyAssemblyManager_AddRef(IHostAssemblyManager* this) {
	return(++((MyAssemblyManager*)this)->count);
}

ULONG STDMETHODCALLTYPE MyAssemblyManager_Release(IHostAssemblyManager* this) {
	if (--((MyAssemblyManager*)this)->count == 0) {
		GlobalFree(this);
		return 0;
	}
	return ((MyAssemblyManager*)this)->count;
}

//This returns a list of assemblies that we are telling the CLR that we want it to handle loading (when/if a load is requested for them)
//We can just return NULL and we will always be asked to load the assembly, but we can tell the CLR to load it in our ProvideAssembly implementation
HRESULT STDMETHODCALLTYPE MyAssemblyManager_GetNonHostStoreAssemblies(MyAssemblyManager* this, ICLRAssemblyReferenceList** ppReferenceList) {
	*ppReferenceList = NULL;
	return S_OK;
}


//This is responsible for returning our IHostAssemblyStore implementation
HRESULT STDMETHODCALLTYPE MyAssemblyManager_GetAssemblyStore(MyAssemblyManager* this, IHostAssemblyStore** ppAssemblyStore) {
	MyAssemblyStore* assemblyStore = GlobalAlloc(GMEM_FIXED, sizeof(MyAssemblyStore));
	assemblyStore->lpVtbl = &MyAssemblyStore_Vtbl;
	assemblyStore->targetAssembly = this->targetAssembly;
	this->assemblyStore = assemblyStore;
	*ppAssemblyStore = this->assemblyStore;

	return S_OK;
}

