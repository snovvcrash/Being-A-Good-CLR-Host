#include "MemoryManager.h"
#include "HostMalloc.h"

//Taken from ReactOS
FORCEINLINE
VOID
InsertHeadList(
	_Inout_ PLIST_ENTRY ListHead,
	_Inout_ __drv_aliasesMem PLIST_ENTRY Entry)
{
	PLIST_ENTRY OldFlink;

	OldFlink = ListHead->Flink;
	Entry->Flink = OldFlink;
	Entry->Blink = ListHead;
	OldFlink->Blink = Entry;
	ListHead->Flink = Entry;
}

HRESULT STDMETHODCALLTYPE MemoryManager_QueryInterface(MemoryManager* this, REFIID vTableGuid, void** ppv) {
	if (!IsEqualIID(vTableGuid, &IID_IUnknown) && !IsEqualIID(vTableGuid, &IID_IHostMemoryManager)) {
		*ppv = 0;
		return E_NOINTERFACE;
	}
	*ppv = this;
	this->lpVtbl->AddRef(this);
	return S_OK;
}

ULONG STDMETHODCALLTYPE MemoryManager_AddRef(MemoryManager* this) {
	return(++((MemoryManager*)this)->count);
}

ULONG STDMETHODCALLTYPE MemoryManager_Release(MemoryManager* this) {
	if (--((MemoryManager*)this)->count == 0) {
		GlobalFree(this);
		return 0;
	}
	return ((MemoryManager*)this)->count;
}

//This is called when the CLR wants to do heap allocations, it's responsible for returning our implementation of IHostMalloc
HRESULT MemoryManager_CreateMalloc(MemoryManager* This, DWORD dwMallocType, IHostMalloc** ppMalloc) {

	//Create our interface
	HostMalloc* mallocManager = GlobalAlloc(GMEM_FIXED, sizeof(HostMalloc));
	mallocManager->lpVtbl = &HostMalloc_Vtbl;

	//Create a heap and add it to our interface struct
	HANDLE hHeap = NULL;
	if (dwMallocType & MALLOC_EXECUTABLE) {
		hHeap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 0, 0);
	}
	else {
		hHeap = HeapCreate(0, 0, 0);
	}
	mallocManager->hHeap = hHeap;
	mallocManager->memAllocList = This->memAllocList;

	*ppMalloc = mallocManager;
	This->mallocManager = mallocManager;

	return S_OK;

}

//The Virtual* API calls are responsible for non-heap memory management, you can just call the Virtual* APIs as intended or implement your own routines
HRESULT MemoryManager_VirtualAlloc(MemoryManager* This, void* pAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect, EMemoryCriticalLevel eCriticalLevel, void** ppMem) {
	LPVOID allocAddress = VirtualAlloc(pAddress, dwSize, flAllocationType, flProtect);
	*ppMem = allocAddress;

	//This will add the entry to our linked list of memory allocations
	/*
	MemAllocEntry* allocEntry = malloc(sizeof(MemAllocEntry));
	allocEntry->Address = allocAddress;
	allocEntry->size = dwSize;
	allocEntry->type = MEM_ALLOC_VIRTUALALLOC;
	((PSLIST_ENTRY)allocEntry)->Next = ((PSLIST_ENTRY)This->memAllocList)->Next;
	((PSLIST_ENTRY)This->memAllocList)->Next = (PSLIST_ENTRY)allocEntry;
	*/
	//End of memory tracking code

	return S_OK;
}
HRESULT MemoryManager_VirtualFree(MemoryManager* This, LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType) {
	VirtualFree(lpAddress, dwSize, dwFreeType);
	return S_OK;
}
HRESULT MemoryManager_VirtualQuery(MemoryManager* This, void* lpAddress, void* lpBuffer, SIZE_T dwLength, SIZE_T* pResult) {
	*pResult = VirtualQuery(lpAddress, (PMEMORY_BASIC_INFORMATION)lpBuffer, dwLength);
	return S_OK;
}
HRESULT MemoryManager_VirtualProtect(MemoryManager* This, void* lpAddress, SIZE_T dwSize, DWORD flNewProtect, DWORD* pflOldProtect) {
	VirtualProtect(lpAddress, dwSize, flNewProtect, pflOldProtect);
	return S_OK;
}

HRESULT MemoryManager_GetMemoryLoad(MemoryManager* This, DWORD* pMemoryLoad, SIZE_T* pAvailableBytes) {
	//Just returning arbitrary values
	*pMemoryLoad = 30;
	*pAvailableBytes = 100 * 1024 * 1024;
	return S_OK;
}

HRESULT MemoryManager_RegisterMemoryNotificationCallback(MemoryManager* This, ICLRMemoryNotificationCallback* pCallback) {
	return S_OK;
}
HRESULT MemoryManager_NeedsVirtualAddressSpace(MemoryManager* This, LPVOID startAddress, SIZE_T size) {
	return S_OK;
}

//This is a notification callback that will be triggered whenever a .NET assembly is loaded into the process
HRESULT MemoryManager_AcquiredVirtualAddressSpace(MemoryManager* This, LPVOID startAddress, SIZE_T size) {
	printf("[*] Mapped file with size %d bytes into memory at 0x%p\n", size, startAddress);

	//This is used to track the assemblies that are mapped into the process
	MemAllocEntry* allocEntry = malloc(sizeof(MemAllocEntry));
	allocEntry->Address = startAddress;
	allocEntry->size = size;
	allocEntry->type = MEM_ALLOC_MAPPED_FILE;
	((PSLIST_ENTRY)allocEntry)->Next = ((PSLIST_ENTRY)This->memAllocList)->Next;
	((PSLIST_ENTRY)This->memAllocList)->Next = (PSLIST_ENTRY)allocEntry;

	return S_OK;
}

HRESULT MemoryManager_ReleasedVirtualAddressSpace(MemoryManager* This, LPVOID startAddress) {
	return S_OK;
}