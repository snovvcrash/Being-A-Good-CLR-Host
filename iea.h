#pragma once
#include <Windows.h>
#include <mscoree.h>
#include <metahost.h>

#include <stdio.h>

#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

static GUID xIID_IHostAssemblyStore = { 0x7b102a88, 0x3f7f, 0x496d, {0x8f, 0xa2, 0xc3, 0x53, 0x74, 0xe0, 0x1a, 0xf3} };
static GUID xIID_IHostAssemblyManager = { 0x613dabd7, 0x62b2, 0x493e, {0x9e, 0x65, 0xc1, 0xe3, 0x2a, 0x1e, 0x0c, 0x5e} };

typedef struct __TargetAssembly {
    LPWSTR assemblyInfo;
    unsigned char* assemblyBytes;
    DWORD assemblySize;
    IStream* iAssemblyStream;
} TargetAssembly;

typedef struct _AppDomain               IAppDomain;
typedef struct _Assembly                IAssembly;
typedef struct _Type                    IType;
typedef struct _Binder                  IBinder;
typedef struct _MethodInfo              IMethodInfo;

#undef DUMMY_METHOD
#define DUMMY_METHOD(x) HRESULT ( STDMETHODCALLTYPE *dummy_##x )(IBinder *This)

typedef struct _BinderVtbl {
    HRESULT(STDMETHODCALLTYPE* QueryInterface)(
        IBinder* This,
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void** ppvObject);

    ULONG(STDMETHODCALLTYPE* AddRef)(
        IBinder* This);

    ULONG(STDMETHODCALLTYPE* Release)(
        IBinder* This);

    DUMMY_METHOD(GetTypeInfoCount);
    DUMMY_METHOD(GetTypeInfo);
    DUMMY_METHOD(GetIDsOfNames);
    DUMMY_METHOD(Invoke);
    DUMMY_METHOD(ToString);
    DUMMY_METHOD(Equals);
    DUMMY_METHOD(GetHashCode);
    DUMMY_METHOD(GetType);
    DUMMY_METHOD(BindToMethod);
    DUMMY_METHOD(BindToField);
    DUMMY_METHOD(SelectMethod);
    DUMMY_METHOD(SelectProperty);
    DUMMY_METHOD(ChangeType);
    DUMMY_METHOD(ReorderArgumentArray);
} BinderVtbl;

typedef struct _Binder {
    BinderVtbl* lpVtbl;
} Binder;

#undef DUMMY_METHOD
#define DUMMY_METHOD(x) HRESULT ( STDMETHODCALLTYPE *dummy_##x )(IAppDomain *This)

typedef struct _AppDomainVtbl {
    BEGIN_INTERFACE

        HRESULT(STDMETHODCALLTYPE* QueryInterface)(
            IAppDomain* This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void** ppvObject);

    ULONG(STDMETHODCALLTYPE* AddRef)(
        IAppDomain* This);

    ULONG(STDMETHODCALLTYPE* Release)(
        IAppDomain* This);

    DUMMY_METHOD(GetTypeInfoCount);
    DUMMY_METHOD(GetTypeInfo);
    DUMMY_METHOD(GetIDsOfNames);
    DUMMY_METHOD(Invoke);

    DUMMY_METHOD(ToString);
    DUMMY_METHOD(Equals);
    DUMMY_METHOD(GetHashCode);
    DUMMY_METHOD(GetType);
    DUMMY_METHOD(InitializeLifetimeService);
    DUMMY_METHOD(GetLifetimeService);
    DUMMY_METHOD(Evidence);
    DUMMY_METHOD(add_DomainUnload);
    DUMMY_METHOD(remove_DomainUnload);
    DUMMY_METHOD(add_AssemblyLoad);
    DUMMY_METHOD(remove_AssemblyLoad);
    DUMMY_METHOD(add_ProcessExit);
    DUMMY_METHOD(remove_ProcessExit);
    DUMMY_METHOD(add_TypeResolve);
    DUMMY_METHOD(remove_TypeResolve);
    DUMMY_METHOD(add_ResourceResolve);
    DUMMY_METHOD(remove_ResourceResolve);
    DUMMY_METHOD(add_AssemblyResolve);
    DUMMY_METHOD(remove_AssemblyResolve);
    DUMMY_METHOD(add_UnhandledException);
    DUMMY_METHOD(remove_UnhandledException);
    //DUMMY_METHOD(DefineDynamicAssembly);
    HRESULT (STDMETHODCALLTYPE* DefineDynamicAssembly)(
        IAppDomain* This,
        /*[in]*/ struct _AssemblyName* name,
        /*[in]*/ enum AssemblyBuilderAccess access,
        /*[out,retval]*/ struct _AssemblyBuilder** pRetVal);
    DUMMY_METHOD(DefineDynamicAssembly_2);
    DUMMY_METHOD(DefineDynamicAssembly_3);
    DUMMY_METHOD(DefineDynamicAssembly_4);
    DUMMY_METHOD(DefineDynamicAssembly_5);
    DUMMY_METHOD(DefineDynamicAssembly_6);
    DUMMY_METHOD(DefineDynamicAssembly_7);
    DUMMY_METHOD(DefineDynamicAssembly_8);
    DUMMY_METHOD(DefineDynamicAssembly_9);
    DUMMY_METHOD(CreateInstance);
    DUMMY_METHOD(CreateInstanceFrom);
    DUMMY_METHOD(CreateInstance_2);
    DUMMY_METHOD(CreateInstanceFrom_2);
    DUMMY_METHOD(CreateInstance_3);
    DUMMY_METHOD(CreateInstanceFrom_3);
    /*
    HRESULT(STDMETHODCALLTYPE* Load) (
        IAppDomain* This,
        IAssemblyName* assemblyRef,
        IAssembly** pRetVal);
        */
    DUMMY_METHOD(Load);
    //DUMMY_METHOD(Load_2);
    HRESULT(STDMETHODCALLTYPE* Load_2) (
        IAppDomain* This,
            BSTR assemblyString, 
            IAssembly** pRetVal
            );

    HRESULT(STDMETHODCALLTYPE* Load_3)(
        IAppDomain* This,
        SAFEARRAY* rawAssembly,
        IAssembly** pRetVal);

    //DUMMY_METHOD(Load_4);
    HRESULT(STDMETHODCALLTYPE* Load_4)(
        IAppDomain* This,
        SAFEARRAY* rawAssembly,
        SAFEARRAY* rawSymbolStore,
        IAssembly** pRetVal);
    DUMMY_METHOD(Load_5);
    DUMMY_METHOD(Load_6);
    DUMMY_METHOD(Load_7);
    DUMMY_METHOD(ExecuteAssembly);
    DUMMY_METHOD(ExecuteAssembly_2);
    DUMMY_METHOD(ExecuteAssembly_3);
    DUMMY_METHOD(FriendlyName);
    DUMMY_METHOD(BaseDirectory);
    DUMMY_METHOD(RelativeSearchPath);
    DUMMY_METHOD(ShadowCopyFiles);
    //DUMMY_METHOD(GetAssemblies);
    HRESULT (STDMETHODCALLTYPE* GetAssemblies)(
        IAppDomain* This,
        SAFEARRAY** pRetVal);
    DUMMY_METHOD(AppendPrivatePath);
    DUMMY_METHOD(ClearPrivatePath);
    DUMMY_METHOD(SetShadowCopyPath);
    DUMMY_METHOD(ClearShadowCopyPath);
    DUMMY_METHOD(SetCachePath);
    DUMMY_METHOD(SetData);
    DUMMY_METHOD(GetData);
    DUMMY_METHOD(SetAppDomainPolicy);
    DUMMY_METHOD(SetThreadPrincipal);
    DUMMY_METHOD(SetPrincipalPolicy);
    DUMMY_METHOD(DoCallBack);
    DUMMY_METHOD(DynamicDirectory);

    END_INTERFACE
} AppDomainVtbl;

typedef struct _AppDomain {
    AppDomainVtbl* lpVtbl;
} AppDomain;

#undef DUMMY_METHOD
#define DUMMY_METHOD(x) HRESULT ( STDMETHODCALLTYPE *dummy_##x )(IAssembly *This)

typedef struct _AssemblyVtbl {
    BEGIN_INTERFACE

        HRESULT(STDMETHODCALLTYPE* QueryInterface)(
            IAssembly* This,
            REFIID riid,
            void** ppvObject);

    ULONG(STDMETHODCALLTYPE* AddRef)(
        IAssembly* This);

    ULONG(STDMETHODCALLTYPE* Release)(
        IAssembly* This);

    DUMMY_METHOD(GetTypeInfoCount);
    DUMMY_METHOD(GetTypeInfo);
    DUMMY_METHOD(GetIDsOfNames);

    DUMMY_METHOD(Invoke);
    DUMMY_METHOD(ToString);
    DUMMY_METHOD(Equals);
    DUMMY_METHOD(GetHashCode);
    DUMMY_METHOD(GetType);
    DUMMY_METHOD(CodeBase);
    DUMMY_METHOD(EscapedCodeBase);
    DUMMY_METHOD(GetName);
    DUMMY_METHOD(GetName_2);
    DUMMY_METHOD(FullName);

    HRESULT(STDMETHODCALLTYPE* EntryPoint)(
        IAssembly* This,
        IMethodInfo** pRetVal);

    HRESULT(STDMETHODCALLTYPE* GetType_2)(
        IAssembly* This,
        BSTR        name,
        IType** pRetVal);

    DUMMY_METHOD(GetType_3);
    DUMMY_METHOD(GetExportedTypes);
    DUMMY_METHOD(GetTypes);
    DUMMY_METHOD(GetManifestResourceStream);
    DUMMY_METHOD(GetManifestResourceStream_2);
    DUMMY_METHOD(GetFile);
    DUMMY_METHOD(GetFiles);
    DUMMY_METHOD(GetFiles_2);
    DUMMY_METHOD(GetManifestResourceNames);
    DUMMY_METHOD(GetManifestResourceInfo);
    DUMMY_METHOD(Location);
    DUMMY_METHOD(Evidence);
    DUMMY_METHOD(GetCustomAttributes);
    DUMMY_METHOD(GetCustomAttributes_2);
    DUMMY_METHOD(IsDefined);
    DUMMY_METHOD(GetObjectData);
    DUMMY_METHOD(add_ModuleResolve);
    DUMMY_METHOD(remove_ModuleResolve);
    DUMMY_METHOD(GetType_4);
    DUMMY_METHOD(GetSatelliteAssembly);
    DUMMY_METHOD(GetSatelliteAssembly_2);
    DUMMY_METHOD(LoadModule);
    DUMMY_METHOD(LoadModule_2);
    DUMMY_METHOD(CreateInstance);
    DUMMY_METHOD(CreateInstance_2);
    DUMMY_METHOD(CreateInstance_3);
    DUMMY_METHOD(GetLoadedModules);
    DUMMY_METHOD(GetLoadedModules_2);
    DUMMY_METHOD(GetModules);
    DUMMY_METHOD(GetModules_2);
    DUMMY_METHOD(GetModule);
    DUMMY_METHOD(GetReferencedAssemblies);
    DUMMY_METHOD(GlobalAssemblyCache);

    END_INTERFACE
} AssemblyVtbl;

typedef enum _BindingFlags {
    BindingFlags_Default = 0,
    BindingFlags_IgnoreCase = 1,
    BindingFlags_DeclaredOnly = 2,
    BindingFlags_Instance = 4,
    BindingFlags_Static = 8,
    BindingFlags_Public = 16,
    BindingFlags_NonPublic = 32,
    BindingFlags_FlattenHierarchy = 64,
    BindingFlags_InvokeMethod = 256,
    BindingFlags_CreateInstance = 512,
    BindingFlags_GetField = 1024,
    BindingFlags_SetField = 2048,
    BindingFlags_GetProperty = 4096,
    BindingFlags_SetProperty = 8192,
    BindingFlags_PutDispProperty = 16384,
    BindingFlags_PutRefDispProperty = 32768,
    BindingFlags_ExactBinding = 65536,
    BindingFlags_SuppressChangeType = 131072,
    BindingFlags_OptionalParamBinding = 262144,
    BindingFlags_IgnoreReturn = 16777216
} BindingFlags;

typedef struct _Assembly {
    AssemblyVtbl* lpVtbl;
} Assembly;

#undef DUMMY_METHOD
#define DUMMY_METHOD(x) HRESULT ( STDMETHODCALLTYPE *dummy_##x )(IMethodInfo *This)

typedef struct _MethodInfoVtbl {
    BEGIN_INTERFACE

        HRESULT(STDMETHODCALLTYPE* QueryInterface)(
            IMethodInfo* This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */
            __RPC__deref_out  void** ppvObject);

    ULONG(STDMETHODCALLTYPE* AddRef)(
        IMethodInfo* This);

    ULONG(STDMETHODCALLTYPE* Release)(
        IMethodInfo* This);

    DUMMY_METHOD(GetTypeInfoCount);
    DUMMY_METHOD(GetTypeInfo);
    DUMMY_METHOD(GetIDsOfNames);
    DUMMY_METHOD(Invoke);

    DUMMY_METHOD(ToString);
    DUMMY_METHOD(Equals);
    DUMMY_METHOD(GetHashCode);
    DUMMY_METHOD(GetType);
    DUMMY_METHOD(MemberType);
    DUMMY_METHOD(name);
    DUMMY_METHOD(DeclaringType);
    DUMMY_METHOD(ReflectedType);
    DUMMY_METHOD(GetCustomAttributes);
    DUMMY_METHOD(GetCustomAttributes_2);
    DUMMY_METHOD(IsDefined);

    HRESULT(STDMETHODCALLTYPE* GetParameters)(
        IMethodInfo* This,
        SAFEARRAY** pRetVal);

    DUMMY_METHOD(GetMethodImplementationFlags);
    DUMMY_METHOD(MethodHandle);
    DUMMY_METHOD(Attributes);
    DUMMY_METHOD(CallingConvention);
    DUMMY_METHOD(Invoke_2);
    DUMMY_METHOD(IsPublic);
    DUMMY_METHOD(IsPrivate);
    DUMMY_METHOD(IsFamily);
    DUMMY_METHOD(IsAssembly);
    DUMMY_METHOD(IsFamilyAndAssembly);
    DUMMY_METHOD(IsFamilyOrAssembly);
    DUMMY_METHOD(IsStatic);
    DUMMY_METHOD(IsFinal);
    DUMMY_METHOD(IsVirtual);
    DUMMY_METHOD(IsHideBySig);
    DUMMY_METHOD(IsAbstract);
    DUMMY_METHOD(IsSpecialName);
    DUMMY_METHOD(IsConstructor);

    HRESULT(STDMETHODCALLTYPE* Invoke_3)(
        IMethodInfo* This,
        VARIANT     obj,
        SAFEARRAY* parameters,
        VARIANT* ret);

    DUMMY_METHOD(returnType);
    DUMMY_METHOD(ReturnTypeCustomAttributes);
    DUMMY_METHOD(GetBaseDefinition);

    END_INTERFACE
} MethodInfoVtbl;

typedef struct _MethodInfo {
    MethodInfoVtbl* lpVtbl;
} MethodInfo;



#define APPDOMAIN_SECURITY_DEFAULT 0
#define APPDOMAIN_SECURITY_SANDBOXED 0x1
#define APPDOMAIN_SECURITY_FORBID_CROSSAD_REVERSE_PINVOKE 0x2
#define APPDOMAIN_IGNORE_UNHANDLED_EXCEPTIONS 0x4
#define APPDOMAIN_FORCE_TRIVIAL_WAIT_OPERATIONS 0x8
#define APPDOMAIN_ENABLE_PINVOKE_AND_CLASSIC_COMINTEROP 0x10
#define APPDOMAIN_SET_TEST_KEY 0x20
#define APPDOMAIN_ENABLE_PLATFORM_SPECIFIC_APPS 0x40
#define APPDOMAIN_ENABLE_ASSEMBLY_LOADFILE 0x80
#define APPDOMAIN_DISABLE_TRANSPARENCY_ENFORCEMENT 0x100

