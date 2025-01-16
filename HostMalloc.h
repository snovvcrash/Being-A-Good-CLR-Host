#pragma once
#include <Windows.h>
#include <mscoree.h>
#include <metahost.h>

//Definitions for our allocation tracker
typedef enum {
	MEM_ALLOC_LIST_HEAD,
	MEM_ALLOC_MALLOC,
	MEM_ALLOC_VIRTUALALLOC,
	MEM_ALLOC_MAPPED_FILE
} memAllocTracker;

typedef struct _MemAllocEntry {
	SLIST_ENTRY allocEntry;
	void* Address;
	SIZE_T size;
	memAllocTracker type;
} MemAllocEntry;

#undef DUMMY_METHOD
#define DUMMY_METHOD(x) HRESULT ( STDMETHODCALLTYPE *dummy_##x )(IHostMalloc *This)
typedef struct _HostMallocVtbl {
	BEGIN_INTERFACE

	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IHostMalloc* This, REFIID riid, void** ppvObject);
	ULONG(STDMETHODCALLTYPE* AddRef)(IHostMalloc* This);
	ULONG(STDMETHODCALLTYPE* Release)(IHostMalloc* This);

	HRESULT(STDMETHODCALLTYPE* Alloc)(IHostMalloc* This, SIZE_T cbSize, EMemoryCriticalLevel eCriticalLevel, void** ppMem);
	HRESULT(STDMETHODCALLTYPE* DebugAlloc)(IHostMalloc* This, SIZE_T cbSize, EMemoryCriticalLevel       eCriticalLevel, char* pszFileName, int         iLineNo, void** ppMem);
	HRESULT(STDMETHODCALLTYPE* Free)(IHostMalloc* This, void* pMem);

	END_INTERFACE

} HostMallocVtbl;

typedef struct _HostMalloc {
	HostMallocVtbl* lpVtbl;
	DWORD count;
	HANDLE hHeap;
	MemAllocEntry* memAllocList;
} HostMalloc;

HRESULT HostMalloc_QueryInterface(HostMalloc* This, REFIID riid, void** ppvObject);
ULONG HostMalloc_AddRef(HostMalloc* This);
ULONG HostMalloc_Release(HostMalloc* This);

HRESULT HostMalloc_Alloc(HostMalloc* This, SIZE_T cbSize, EMemoryCriticalLevel eCriticalLevel, void** ppMem);
HRESULT HostMalloc_DebugAlloc(HostMalloc* This, SIZE_T cbSize, EMemoryCriticalLevel       eCriticalLevel, char* pszFileName, int         iLineNo, void** ppMem);
HRESULT HostMalloc_Free(HostMalloc* This, void* pMem);

static const HostMallocVtbl HostMalloc_Vtbl = {
	HostMalloc_QueryInterface,
	HostMalloc_AddRef,
	HostMalloc_Release,
	HostMalloc_Alloc,
	HostMalloc_DebugAlloc,
	HostMalloc_Free
};
