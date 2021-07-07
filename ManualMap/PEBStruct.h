#pragma once
#include <Windows.h>


#define PTR_32 uint32_t
#define PTR_64 uint64_t

namespace PEBStruct {

	template<typename T>
	struct UNICODE_STRING {
		USHORT Length;
		USHORT MaximumLength;
		T  Buffer; //PCWSTR type pointer
	};

	template<typename T>
	struct LIST_ENTRY {
		T Flink;
		T Blink;
	};

	template<typename T>
	struct PROCESS_BASIC_INFORMATION {
		T Reserved1;
		T PebBaseAddress; //PEB pointer type
		T Reserved2[2];
		T UniqueProcessId; //ULONG_PTR pointer type
		T Reserved3;
	};

	template<typename T>
	struct LDR_DATA_TABLE_ENTRY
	{
		LIST_ENTRY<T> InLoadOrderLinks;
		LIST_ENTRY<T> InMemoryOrderLinks;
		LIST_ENTRY<T> InInitializationOrderLinks;
		T DllBase;
		T EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING<T> FullDllName;
		UNICODE_STRING<T> BaseDllName;
		ULONG Flags;
		WORD LoadCount;
		WORD TlsIndex;
		union
		{
			LIST_ENTRY<T> HashLinks;
			struct
			{
				T SectionPointer;
				ULONG CheckSum;
			};
		};
		union
		{
			ULONG TimeDateStamp;
			T LoadedImports;
		};
		T EntryPointActivationContext; //_ACTIVATION_CONTEXT type pointer
		T PatchInformation;
		LIST_ENTRY<T> ForwarderLinks;
		LIST_ENTRY<T> ServiceTagLinks;
		LIST_ENTRY<T> StaticLinks;
	};

	template<typename T>
	struct PEB_LDR_DATA
	{
		ULONG Length;
		BOOLEAN Initialized;
		T SsHandle;
		LIST_ENTRY<T> InLoadOrderModuleList;
		LIST_ENTRY<T> InMemoryOrderModuleList;
		LIST_ENTRY<T> InInitializationOrderModuleList;
		T EntryInProgress;
		BOOLEAN ShutdownInProgress;
		HANDLE ShutdownThreadId;
	};

	template <class T>
	struct _PEB
	{
		BYTE InheritedAddressSpace;
		BYTE ReadImageFileExecOptions;
		BYTE BeingDebugged;
		union {
			UCHAR BitField;
			struct {
				/*  bit fields, follow link  */
			};
		};
		T Mutant;
		T ImageBaseAddress;
		T Ldr;
		T ProcessParameters;
		T SubSystemData;
		T ProcessHeap;
		T FastPebLock;
		T AtlThunkSListPtr;
		T IFEOKey;
		union {
			ULONG CrossProcessFlags;
			struct {
				/*  bit fields, follow link  */
			};
		};
		union
		{
			T KernelCallbackTable;
			T UserSharedInfoPtr;
		};
		DWORD SystemReserved;
		DWORD AtlThunkSListPtr32;
		T ApiSetMap;
		T TlsExpansionCounter;
		T TlsBitmap;
	};




};