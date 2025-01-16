# Being a Good CLR Host
This repository contains a proof-of-concept for using Common Language Runtime (CLR) customizations in offensive .NET tradecraft. CLR customizations allow operators to take control over aspects of the CLR and implement more opsec friendly behavior. This repository contains examples for two manager interfaces: memory management and assembly loading.

## CLR Hosting and Customization
Red teams have been using CLR Hosting to reflectively load .NET tools for a long time now. Historically this has meant using the deprecated [ICorRuntimeHost](https://learn.microsoft.com/en-us/dotnet/framework/unmanaged-api/hosting/icorruntimehost-interface) interface to start the CLR, and subsequent interfaces (IAppDomain, IAssembly) are used to create app domains and load assemblies from disk or from in-memory byte arrays. 

The ICorRuntimeHost interface has been deprecated in favor of the [ICLRRuntimeHost](https://learn.microsoft.com/en-us/dotnet/framework/unmanaged-api/hosting/iclrruntimehost-interface) interface. The ICLRRuntimeHost interface does not include any of the interfaces that allow for reflectively loading .NET assemblies, but it does provide interfaces that allow for implementing custom CLR managers. In order to take control over aspects of the CLR, developers can implement their own [IHostControl](https://learn.microsoft.com/en-us/dotnet/framework/unmanaged-api/hosting/ihostcontrol-interface) interface and call the SetHostControl method of the ICLRRuntimeHost interface before starting the CLR.

This IHostControl implementation is responsible for telling the CLR what managers the developer would like to implement. Any interfaces that have not been implemented will use the CLR's default behavior. For implemented interfaces, the developer will have their IHostControl implementation create and return an instance of their custom interface.

Operators can take advantage of both CLR customizations and reflective assembly loads by calling SetHostControl and starting the CLR using an ICLRRuntimeHost interface, and then getting an interface for the ICorRuntimeHost interface. The methods from the ICorRuntimeHost interface can then be called normally.

## Memory Management
Developers can take control over all memory management performed by the CLR by implementing the [IHostMemoryManager](https://learn.microsoft.com/en-us/dotnet/framework/unmanaged-api/hosting/ihostmemorymanager-interface) interface. This interface allows us to implement custom routines for any VirtualAlloc/VirtualProtect/VirtualQuery/VirtualFree calls. This proof-of-concept simply calls the intended APIs, but one application of this would be using indirect syscalls to perform these memory management API calls instead. The IHostMemoryManager interface is also responsible for returning an IHostMalloc interface which is responsible for any heap allocations made by the CLR. 

The [AcquiredVirtualAddressSpace](https://learn.microsoft.com/en-us/dotnet/framework/unmanaged-api/hosting/ihostmemorymanager-acquiredvirtualaddressspace-method) method is a notification callback that the CLR will call when an assembly is mapped into memory. This can be used to keep track of where assemblies have been mapped into the process.

By taking control over all memory management routines, we can also keep track of all memory allocations made by the CLR. This can be used to encrypt these allocations while a C2 beacon is sleeping, although this can cause instability if the CLR performs any actions while the memory is encrypted. A simple proof-of-concept implementation of tracking memory allocations in a linked list structure is included in this repo. There is also a commented block of code that will handle XOR encrypting all allocations made by the beacon. Since this is unstable you will have to uncomment the code to use it.

## Assembly Loading
Developers can take control over some assembly loading functionality performed by the CLR using the [IHostAssemblyStore](https://learn.microsoft.com/en-us/dotnet/framework/unmanaged-api/hosting/ihostassemblystore-interface) and [IHostAssemblyManager](https://learn.microsoft.com/en-us/dotnet/framework/unmanaged-api/hosting/ihostassemblymanager-interface) interfaces. The IHostAssemblyManager interface is responsible for two things: telling the CLR what assemblies that the CLR should load, and for returning an instance of the IHostAssemblyStore interface. For the former, we can tell the CLR that it should always load certain assemblies that we're not interested in (mscorlib, for example) or we can just return NULL and it will ask us for every assembly.

The IHostAssemblyStore manager has two methods: ProvideAssembly and ProvideModule. ProvideAssembly allows us to take control over how assemblies are loaded by the CLR. When an assembly load is requested, the CLR will call our ProvideAssembly implementation and provide the identity of the assembly as an argument. The method is expected to set an output argument to a pointer to an IStream interface that contains the assembly bytes. 

Inserting our own code at this stage of CLR assembly loading enables a novel bypass for the Anti-Malware Scan Interface (AMSI). In the context of the CLR, AMSI is responsible for scanning assembly content during reflective assembly loads. Traditionally the Load_3 API call, which takes a byte array as an argument, has been used to load .NET assemblies reflectively (this triggers an AMSI scan). By using the Load_2 API call, which takes an assembly identity string as an argument, the CLR believes we are loading an assembly from the file system. When we call Load_2 with our target assembly identity (ex: Seatbelt) our ProvideAssembly implementation will get called, and we can return an IStream containing the bytes for our in-memory assembly. The CLR will load the provided assembly bytes and an AMSI scan is never performed. In fact, AMSI is never even loaded into the process.

According to an [CLR Inside Out](https://learn.microsoft.com/en-us/archive/msdn-magazine/2006/august/clr-inside-out-clr-hosting-apis) article from August 2006, these assembly loading managers were used to implement SQL Server 2005's ability to load .NET assemblies from a database.

## Notes
If you choose to implement this into your C2 there are some things to be aware of. 

First is that if your implant also uses sleep masking then you may run into problems if your custom managers are called while the implement is masked, such as during asynchronous assembly execution. Since the CLR uses unbacked RWX memory for JIT execution, it should not be as much of a concern to have small unbacked RWX regions in your process, should you choose to leave the managers unencrypted during .NET execution.

When implementing ProvideAssembly, the assembly that you return must have the same identity string as the assembly that was requested. If the CLR asks you for mscorlib and you try to return Seatbelt then the CLR will throw an error and not load your assembly.

## Usage
The main.c file contains a hardcoded filepath for your assembly and a single hardcoded argument passed to the assembly. Change these to your desired values, compile, and execute.

## Related Works and Resources
[HostingCLR](https://github.com/etormadiv/HostingCLR) - Original implementation of CLR hosting
[InlineExecute-Assembly](https://github.com/anthemtotheego/InlineExecute-Assembly) - Original implementation of executing .NET assemblies as a BOF
[Dealing with Failure: Failure Escalation Policy in CLR Hosts](https://posts.specterops.io/dealing-with-failure-failure-escalation-policy-in-clr-hosts-54ca8b728faa) – This is the only real example I could find of offensive tradecraft using CLR Customizations when I was initially doing this research.
[Hosted Pumpkin](https://github.com/ldematte/HostedPumpkin) – A GitHub repository containing a proof-of-concept for implementing several CLR Customizations.
[Shellcode: Loading .NET Assemblies From Memory](https://modexp.wordpress.com/2019/05/10/dotnet-loader-shellcode/) – Donut was a great deal of help in wrangling all of the relevant data structures and definitions in C.
Customizing the Microsoft .NET Framework Common Language Runtime by Steven Pratschner – This is the definitive text on CLR Customizations. Simply a must-read if you have any interest in this area.
