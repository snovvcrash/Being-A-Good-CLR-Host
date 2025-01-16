#include "HostMalloc.h"
#include "MemoryManager.h"

HRESULT STDMETHODCALLTYPE HostMalloc_QueryInterface(HostMalloc* this, REFIID vTableGuid, void** ppv) {
	if (!IsEqualIID(vTableGuid, &IID_IUnknown) && !IsEqualIID(vTableGuid, &IID_IHostMalloc)) {
		*ppv = 0;
		return E_NOINTERFACE;
	}
	*ppv = this;
	this->lpVtbl->AddRef(this);
	return S_OK;
}

ULONG STDMETHODCALLTYPE HostMalloc_AddRef(HostMalloc* this) {
	return(++((HostMalloc*)this)->count);
}

ULONG STDMETHODCALLTYPE HostMalloc_Release(HostMalloc* this) {
	if (--((HostMalloc*)this)->count == 0) {
		GlobalFree(this);
		return 0;
	}
	return ((HostMalloc*)this)->count;
}

HRESULT HostMalloc_Alloc(HostMalloc* This, SIZE_T cbSize, EMemoryCriticalLevel eCriticalLevel, void** ppMem) {
	LPVOID allocAddress = HeapAlloc(This->hHeap, 0, cbSize);

	//This adds the allocation to our memory tracker linked list
	/*
	MemAllocEntry* allocEntry = malloc(sizeof(MemAllocEntry));
	allocEntry->Address = allocAddress;
	allocEntry->size = cbSize;
	allocEntry->type = MEM_ALLOC_MALLOC;
	((PSLIST_ENTRY)allocEntry)->Next = ((PSLIST_ENTRY)This->memAllocList)->Next;
	((PSLIST_ENTRY)This->memAllocList)->Next = (PSLIST_ENTRY)allocEntry;
	*/
	//End of memory tracking code

	//Set the output variable to our allocated memory or return an error
	*ppMem = allocAddress;
	if (*ppMem == NULL) {
		return E_OUTOFMEMORY;
	}
	else {
		return S_OK;
	}
}

HRESULT HostMalloc_DebugAlloc(HostMalloc* This, SIZE_T cbSize, EMemoryCriticalLevel       eCriticalLevel, char* pszFileName, int         iLineNo, void** ppMem) {
	*ppMem = HeapAlloc(This->hHeap, 0, cbSize);
	if (*ppMem == NULL) {
		return E_OUTOFMEMORY;
	}
	else {
		return S_OK;
	}
}

HRESULT HostMalloc_Free(HostMalloc* This, void* pMem) {
	HeapFree(This->hHeap, 0, pMem);
	return S_OK;
}