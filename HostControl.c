#include "HostControl.h"
#include "AssemblyManager.h"
#include "MemoryManager.h"

HRESULT STDMETHODCALLTYPE MyHostControl_QueryInterface(MyHostControl* this, REFIID vTableGuid, void** ppv) {
	if (!IsEqualIID(vTableGuid, &IID_IUnknown) && !IsEqualIID(vTableGuid, &xIID_IHostControl)) {
		*ppv = 0;
		return E_NOINTERFACE;
	}
	*ppv = this;
	this->lpVtbl->AddRef(this);
	return S_OK;
}

ULONG STDMETHODCALLTYPE MyHostControl_AddRef(MyHostControl* this) {
	return(++((MyHostControl*)this)->count);
}

ULONG STDMETHODCALLTYPE MyHostControl_Release(MyHostControl* this) {
	if (--((MyHostControl*)this)->count == 0) {
		GlobalFree(this);
		return 0;
	}
	return ((MyHostControl*)this)->count;
}

//This is responsible for returning all of our manager implementations
//If you want to disable an interface just comment out the if statement
HRESULT STDMETHODCALLTYPE MyHostControl_GetHostManager(MyHostControl* this, REFIID riid, void** ppObject)
{
	//We actually create this manager already in the main code but you can also do it here
	if (IsEqualIID(riid, &IID_IHostMemoryManager))
	{
		*ppObject = this->memoryManager;
		
		return S_OK;
	}

	if (IsEqualIID(riid,&IID_IHostAssemblyManager))
	{
		//Create our IHostAssemblyManager interface and return it
		MyAssemblyManager* assemblyManager = GlobalAlloc(GMEM_FIXED, sizeof(MyAssemblyManager));
		assemblyManager->lpVtbl = &MyAssemblyManager_Vtbl;
		assemblyManager->targetAssembly = this->targetAssembly;
		assemblyManager->assemblyStore = NULL;
		*ppObject = assemblyManager;
		return S_OK;
	}

	*ppObject = NULL;
	return E_NOINTERFACE;
}

//This has some fun uses left as an exercise for the reader :) 
HRESULT MyHostControl_SetAppDomainManager(IHostControl* This, DWORD dwAppDomainID, IUnknown* pUnkAppDomainManager) {
	return E_NOTIMPL;
}

