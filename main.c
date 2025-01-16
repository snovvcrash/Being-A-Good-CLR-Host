
#include <Windows.h>
#include <stdio.h>
#include <mscoree.h>
#include <metahost.h>

#pragma comment(lib, "mscoree.lib")

#include "HostControl.h"
#include "AssemblyManager.h"
#include "AssemblyStore.h"
#include "MemoryManager.h"

#include "iea.h"

/*CLR GUIDS, Stucts -> Mostly from https://github.com/TheWover/donut*/
static GUID xIID_AppDomain = { 0x05F696DC, 0x2B29, 0x3663, {0xAD, 0x8B, 0xC4,0x38, 0x9C, 0xF2, 0xA7, 0x13} };
static GUID xCLSID_CLRMetaHost = {  0x9280188d, 0xe8e, 0x4867, {0xb3, 0xc, 0x7f, 0xa8, 0x38, 0x84, 0xe8, 0xde} };
static GUID xIID_ICLRMetaHost = {  0xD332DB9E, 0xB9B3, 0x4125, {0x82, 0x07, 0xA1, 0x48, 0x84, 0xF5, 0x32, 0x16} };
static GUID xIID_ICLRRuntimeInfo = {  0xBD39D1D2, 0xBA2F, 0x486a, {0x89, 0xB0, 0xB4, 0xB0, 0xCB, 0x46, 0x68, 0x91} };
static GUID xIID_ICorRuntimeHost = {  0xcb2f6722, 0xab3a, 0x11d2, {0x9c, 0x40, 0x00, 0xc0, 0x4f, 0xa3, 0x0a, 0x3e} };
static GUID xCLSID_CorRuntimeHost = {  0xcb2f6723, 0xab3a, 0x11d2, {0x9c, 0x40, 0x00, 0xc0, 0x4f, 0xa3, 0x0a, 0x3e} };
static GUID xCLSID_ICLRRuntimeHost = { 0x90F1A06E, 0x7712, 0x4762, {0x86, 0xB5, 0x7A, 0x5E, 0xBA, 0x6B, 0xDB, 0x02} };
static GUID xIID_ICLRRuntimeHost = { 0x90F1A06C, 0x7712, 0x4762, {0x86, 0xB5, 0x7A, 0x5E, 0xBA, 0x6B, 0xDB, 0x02 }};
static GUID xIID_ICLRAssemblyIdentityManager = { 0x15f0a9da, 0x3ff6, 0x4393, {0x9d, 0xa9, 0xfd, 0xfd, 0x28, 0x4e, 0x69, 0x72} };
static GUID xIID_IAssemblyName = { 0xB42B6AAC, 0x317E, 0x34D5, {0x9F, 0xA9, 0x09, 0x3B, 0xB4, 0x16, 0x0C, 0x50 } };

//This is used to encrypt memory allocations if you choose to enable that
char mask[4] = { 0xa, 0xa, 0xa, 0xa };
int maskSize = 4;
void XOREncrypt(char* baseAddress, SIZE_T beaconSize) {
	DWORD start = 0;
	while (start < beaconSize) {
		*(baseAddress + start) ^= mask[start % maskSize];
		start++;
	}
}

int main()
{

	ICLRRuntimeHost* myCustomHost = NULL;
	DWORD dwReturn;
	HRESULT hr;

	//Standard CLR starting stuff
	ICLRMetaHost* pMetaHost = NULL;
	ICLRRuntimeInfo* pRuntimeInfo = NULL;
	hr = CLRCreateInstance(&xCLSID_CLRMetaHost, &xIID_ICLRMetaHost, (LPVOID*)&pMetaHost);
	hr = pMetaHost->lpVtbl->GetRuntime(pMetaHost, L"v4.0.30319", &xIID_ICLRRuntimeInfo, &pRuntimeInfo);
	hr = pRuntimeInfo->lpVtbl->GetInterface(pRuntimeInfo, &xCLSID_ICLRRuntimeHost, &xIID_ICLRRuntimeHost, (LPVOID*)&myCustomHost);

	// Load the executable that we're going to load - loading from disk for testing. Set the path to your assembly
	HANDLE seatbeltDll = CreateFileA("C:\\Users\\User\\Assembly.exe", GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	const unsigned int seatbeltSize = GetFileSize(seatbeltDll, NULL);
	LPVOID seatbeltBytes = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, seatbeltSize);
	DWORD seatbeltOutSize = 0;
	ReadFile(seatbeltDll, seatbeltBytes, seatbeltSize, &seatbeltOutSize, NULL);
	CloseHandle(seatbeltDll);

	//This grabs a pointer to the ICLRAssemblyIdentityManager interface, which we will use below
	typedef HRESULT(__stdcall* CLRIdentityManagerProc)(REFIID, IUnknown**);
	CLRIdentityManagerProc pIdentityManagerProc = NULL;
	ICLRAssemblyIdentityManager* pIdentityManager = NULL; 
	hr = pRuntimeInfo->lpVtbl->GetProcAddress(pRuntimeInfo, "GetCLRIdentityManager", (void**)&pIdentityManagerProc);
	hr = (pIdentityManagerProc)(&xIID_ICLRAssemblyIdentityManager, (IUnknown**)&pIdentityManager);

	//This calls GetBindingIdentityFromStream from the ICLRAssemblyIdentityManager interface to get the identity string for our assembly byte array
	IStream* assemblyStream = SHCreateMemStream((const byte*)seatbeltBytes, seatbeltSize);
	LPWSTR identityBuffer = (LPWSTR)malloc(4096);
	DWORD identityBufferSize = 4096;
	pIdentityManager->lpVtbl->GetBindingIdentityFromStream(pIdentityManager, assemblyStream, CLR_ASSEMBLY_IDENTITY_FLAGS_DEFAULT, identityBuffer, &identityBufferSize);
	//Debugging printf to show the assembly identity
	printf("[*] GetBindingIdentityFromStream: %ls\n", identityBuffer);
	
	//This is my implementation of passing the information for the desired assembly into our IHostAssemblyStore implementation
	//There are cleaner/easier ways of doing this but this is a POC :)
	TargetAssembly* targetAssembly = GlobalAlloc(GMEM_FIXED, sizeof(TargetAssembly));
	//The identity string that we need to check against
	targetAssembly->assemblyInfo = identityBuffer;
	//Size and byte array for the assembly we want to run
	targetAssembly->assemblySize = seatbeltSize;
	targetAssembly->assemblyBytes = seatbeltBytes;

	//Set the target assembly to the one we just created
	MyHostControl customHostControl;
	customHostControl.lpVtbl = &MyHostControl_Vtbl;
	customHostControl.targetAssembly = targetAssembly;

	//Create our IHostMemoryManager
	MemoryManager* memoryManager = GlobalAlloc(GMEM_FIXED, sizeof(MemoryManager));
	memoryManager->lpVtbl = &MemoryManager_Vtbl;
	MemAllocEntry* memAllocListHead = malloc(sizeof(MemAllocEntry));
	((PSLIST_ENTRY)memAllocListHead)->Next = NULL;

	memoryManager->memAllocList = &memAllocListHead;
	customHostControl.memoryManager = memoryManager;

	//Set the host control
	hr = myCustomHost->lpVtbl->SetHostControl(myCustomHost, &customHostControl);
	printf("[+] Set custom host control\n");

	//Start the CLR
	hr = myCustomHost->lpVtbl->Start(myCustomHost);
	printf("[+] Started CLR\n");

	ICLRControl* clrControl;
	ICorRuntimeHost* pCorHost = NULL;
	AppDomain* pDefaultAppDomain = NULL;
	Assembly* pAssembly = NULL;
	IUnknown* pAppDomainThunk = NULL;
	MethodInfo* pMethodInfo = NULL;

	//Now we get the ICorRuntimeHost interface so we can use the normal (deprecated) assembly load API calls
	hr = pRuntimeInfo->lpVtbl->GetInterface(pRuntimeInfo, &xCLSID_CorRuntimeHost, &xIID_ICorRuntimeHost, (VOID**)&pCorHost);
	
	//Get a handle to our default App Domain
	hr = pCorHost->lpVtbl->GetDefaultDomain(pCorHost, &pAppDomainThunk);

	/* Equivalent of System.AppDomain.CurrentDomain in C# */
	hr = pAppDomainThunk->lpVtbl->QueryInterface(pAppDomainThunk, &xIID_AppDomain, (VOID**)&pDefaultAppDomain);

	//We don't actually need this safearray to load using Load_2, but I left this code in so you can easily switch to Load_3 for comparison
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].cElements = seatbeltSize;
	rgsabound[0].lLbound = 0;
	SAFEARRAY* pSafeArray = SafeArrayCreate(VT_UI1, 1, rgsabound);
	void* pvData = NULL;
	hr = SafeArrayAccessData(pSafeArray, &pvData);
	if (FAILED(hr))
	{
		printf("[!] SafeArrayAccessData failed\n");
		return -1;
	}

	memcpy(pvData, seatbeltBytes, seatbeltSize);

	hr = SafeArrayUnaccessData(pSafeArray);

	if (FAILED(hr))
	{
		printf("[!] SafeArrayUnaccessData failed\n");
		return -1;
	}
	
	//This allocates a BSTR with our identity string and uses Load_2 to load it
	//Load_2 will pass execution to our ProvideAssembly method of our custom IHostAssemblyStore interface, which will return the bytes for the assembly that we want to load
	BSTR assemblyName = SysAllocString(identityBuffer);
	hr = pDefaultAppDomain->lpVtbl->Load_2(pDefaultAppDomain, assemblyName, &pAssembly);

	//Below is the Load_3 call if you want to uncomment and compare the Load_2 method with traditional reflective loading
	//hr = pDefaultAppDomain->lpVtbl->Load_3(pDefaultAppDomain, pSafeArray, &pAssembly);

	if (FAILED(hr))
	{
		printf("[+] pDefaultAppDomain->Load_2 failed\n");
		return -1;
	}

	printf("[+] pDefaultAppDomain->Load_2 succeeded\n");

	/* Assembly.EntryPoint Property */
	hr = pAssembly->lpVtbl->EntryPoint(pAssembly, &pMethodInfo);

	VARIANT retVal;
	ZeroMemory(&retVal, sizeof(VARIANT));
	VARIANT obj;
	ZeroMemory(&obj, sizeof(VARIANT));
	obj.vt = VT_NULL;

	//This is where you put your arguments
	//Single argument is hardcoded since this is a proof-of-concept
	VARIANT args;
	args.vt = VT_ARRAY | VT_BSTR;
	SAFEARRAYBOUND argsBound[1];
	argsBound[0].lLbound = 0;
	argsBound[0].cElements = 1;
	args.parray = SafeArrayCreate(VT_BSTR, 1, argsBound);
	long idx[1];
	for (int i = 0; i < 1; i++)
	{
		idx[0] = i;
		SafeArrayPutElement(args.parray, idx, SysAllocString(L"help"));
	}
	SAFEARRAY* params = NULL;
	SAFEARRAYBOUND paramsBound[1];
	paramsBound[0].lLbound = 0;
	paramsBound[0].cElements = 1;
	params = SafeArrayCreate(VT_VARIANT, 1, paramsBound);
	idx[0] = 0;
	SafeArrayPutElement(params, idx, &args);

	/* Equivalent to EntryPoint.Invoke(null, new object[0]) */
	hr = pMethodInfo->lpVtbl->Invoke_3(pMethodInfo, obj, params, &retVal);

	//Below is code for memory management tracking, this gets the head of the list and then the while loop will walk through all entries
	MemAllocEntry* allocEntry = memoryManager->memAllocList;
	PSLIST_ENTRY next = ((PSLIST_ENTRY)allocEntry)->Next;
	MEMORY_BASIC_INFORMATION memInfo;
	DWORD oldProtect;
	
	while (next != allocEntry) {
		//This will encrypt any assemblies that have been mapped into memory
		//If you try to run the assembly again while encrypted it will crash
		if (((MemAllocEntry*)next)->type == MEM_ALLOC_MAPPED_FILE) {
			//XOREncrypt(((MemAllocEntry*)next)->Address, ((MemAllocEntry*)next)->size);
		}

		//This will try to encrypt all values in our memory tracker
		//THIS IS VERY UNSTABLE AND WILL PROBABLY CRASH YOUR PROCESS!

		//char* baseAddress = ((MemAllocEntry*)next)->Address;
		//VirtualQuery(baseAddress, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));
		//if (memInfo.AllocationProtect != 0 && memInfo.State != 0x2000 && memInfo.State != 0x10000) {
			//if (memInfo.Protect != PAGE_READWRITE) {
				//VirtualProtect(memInfo.BaseAddress, memInfo.RegionSize, PAGE_READWRITE, &oldProtect);
				//XOREncrypt(memInfo.BaseAddress, memInfo.RegionSize);
				//VirtualProtect(memInfo.BaseAddress, memInfo.RegionSize, oldProtect, &oldProtect);
			//}
			//else {
				//XOREncrypt(memInfo.BaseAddress, memInfo.RegionSize);
			//}

		//}
		
		
		if (next->Next == NULL) {
			break;
		}
		next = next->Next;
	}

	//Clean up everything
	RtlZeroMemory(seatbeltBytes, seatbeltSize);
	assemblyStream->lpVtbl->Release(assemblyStream);

	if (pAssembly) {
		pAssembly->lpVtbl->Release(pAssembly);
	}
	if (myCustomHost) {
		myCustomHost->lpVtbl->Release(myCustomHost);
	}
	if (pCorHost) {
		pCorHost->lpVtbl->Release(pCorHost);
	}
	if (pDefaultAppDomain) {
		pDefaultAppDomain->lpVtbl->Release(pDefaultAppDomain);
	}
	if (&customHostControl) {
		customHostControl.lpVtbl->Release(&customHostControl);
	}
	if (pAppDomainThunk) {
		pAppDomainThunk->lpVtbl->Release(pAppDomainThunk);
	}
	if (pIdentityManager) {
		pIdentityManager->lpVtbl->Release(pIdentityManager);
	}
	if (pSafeArray) {
		SafeArrayDestroy(pSafeArray);
	}
	
	VariantClear(&retVal);
	VariantClear(&obj);
	VariantClear(&args);
	//End cleanup

	return 1;

}