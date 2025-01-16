#pragma once
#include "iea.h"
#include "HostMalloc.h"



typedef struct _MemoryManagerVtbl {
	BEGIN_INTERFACE

	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IHostMemoryManager* This, REFIID riid, void** ppvObject);
	ULONG(STDMETHODCALLTYPE* AddRef)(IHostMemoryManager* This);
	ULONG(STDMETHODCALLTYPE* Release)(IHostMemoryManager* This);

	HRESULT(STDMETHODCALLTYPE* CreateMalloc)(IHostMemoryManager* This, DWORD dwMallocType,	IHostMalloc** ppMalloc);
	HRESULT(STDMETHODCALLTYPE* VirtualAlloc)(IHostMemoryManager* This, void* pAddress,SIZE_T dwSize,DWORD flAllocationType,DWORD flProtect,EMemoryCriticalLevel eCriticalLevel,void** ppMem);
	HRESULT(STDMETHODCALLTYPE* VirtualFree)(IHostMemoryManager* This, LPVOID lpAddress,SIZE_T dwSize,DWORD dwFreeType);
	HRESULT(STDMETHODCALLTYPE* VirtualQuery)(IHostMemoryManager* This, void* lpAddress, void* lpBuffer, SIZE_T dwLength, SIZE_T* pResult);
	HRESULT(STDMETHODCALLTYPE* VirtualProtect)(IHostMemoryManager* This, void* lpAddress,SIZE_T dwSize,DWORD flNewProtect,DWORD* pflOldProtect);
	HRESULT(STDMETHODCALLTYPE* GetMemoryLoad)(IHostMemoryManager* This, DWORD* pMemoryLoad,SIZE_T* pAvailableBytes);
	HRESULT(STDMETHODCALLTYPE* RegisterMemoryNotificationCallback)(IHostMemoryManager* This, ICLRMemoryNotificationCallback* pCallback);
	HRESULT(STDMETHODCALLTYPE* NeedsVirtualAddressSpace)(IHostMemoryManager* This, LPVOID startAddress,SIZE_T size);
	HRESULT(STDMETHODCALLTYPE* AcquiredVirtualAddressSpace)(IHostMemoryManager* This, LPVOID startAddress,SIZE_T size);
	HRESULT(STDMETHODCALLTYPE* ReleasedVirtualAddressSpace)(IHostMemoryManager* This, LPVOID startAddress);

	END_INTERFACE

} MemoryManagerVtbl;

//The first two items in this struct are actually needed, the second two are just my implementation
typedef struct _MemoryManager {
	MemoryManagerVtbl* lpVtbl;
	DWORD count;
	HostMalloc* mallocManager;
	MemAllocEntry* memAllocList;
} MemoryManager;

HRESULT MemoryManager_QueryInterface(MemoryManager* This, REFIID riid, void** ppvObject);
ULONG MemoryManager_AddRef(MemoryManager* This);
ULONG MemoryManager_Release(MemoryManager* This);
HRESULT MemoryManager_CreateMalloc(MemoryManager* This, DWORD dwMallocType, IHostMalloc** ppMalloc);
HRESULT MemoryManager_VirtualAlloc(MemoryManager* This, void* pAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect, EMemoryCriticalLevel eCriticalLevel, void** ppMem);
HRESULT MemoryManager_VirtualFree(MemoryManager* This, LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
HRESULT MemoryManager_VirtualQuery(MemoryManager* This, void* lpAddress, void* lpBuffer, SIZE_T dwLength, SIZE_T* pResult);
HRESULT MemoryManager_VirtualProtect(MemoryManager* This, void* lpAddress, SIZE_T dwSize, DWORD flNewProtect, DWORD* pflOldProtect);
HRESULT MemoryManager_GetMemoryLoad(MemoryManager* This, DWORD* pMemoryLoad, SIZE_T* pAvailableBytes);
HRESULT MemoryManager_RegisterMemoryNotificationCallback(MemoryManager* This, ICLRMemoryNotificationCallback* pCallback);
HRESULT MemoryManager_NeedsVirtualAddressSpace(MemoryManager* This, LPVOID startAddress, SIZE_T size);
HRESULT MemoryManager_AcquiredVirtualAddressSpace(MemoryManager* This, LPVOID startAddress, SIZE_T size);
HRESULT MemoryManager_ReleasedVirtualAddressSpace(MemoryManager* This, LPVOID startAddress);

static const MemoryManagerVtbl MemoryManager_Vtbl = {
	MemoryManager_QueryInterface,
	MemoryManager_AddRef,
	MemoryManager_Release,
	MemoryManager_CreateMalloc,
	MemoryManager_VirtualAlloc,
	MemoryManager_VirtualFree,
	MemoryManager_VirtualQuery,
	MemoryManager_VirtualProtect,
	MemoryManager_GetMemoryLoad,
	MemoryManager_RegisterMemoryNotificationCallback,
	MemoryManager_NeedsVirtualAddressSpace,
	MemoryManager_AcquiredVirtualAddressSpace,
	MemoryManager_ReleasedVirtualAddressSpace
};