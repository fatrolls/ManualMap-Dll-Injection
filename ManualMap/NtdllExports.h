#pragma once
#include <windows.h>
#include <winternl.h>


typedef NTSTATUS(*PNTQueryInformationProcess)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
typedef BOOLEAN(*PRtlEqualUnicodeString)(PCUNICODE_STRING, PCUNICODE_STRING, BOOLEAN);
typedef VOID(*PRtlInitUnicodeString) (PUNICODE_STRING, PCWSTR);
typedef NTSTATUS(*PRtlAnsiStringToUnicodeString)(PUNICODE_STRING, PCANSI_STRING, BOOLEAN);
typedef void (*PRtlFreeUnicodeString)(PUNICODE_STRING);


class Ntdll {
public:
	Ntdll();
	~Ntdll();

private:
	void LoadFunctions();

public:
	NTSTATUS NtQueryInformationProcess(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);
	BOOLEAN RtlEqualUnicodeString(PCUNICODE_STRING String1, PCUNICODE_STRING String2, BOOLEAN CaseInSensitive);
	VOID RtlInitUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString);
	NTSTATUS RtlAnsiStringToUnicodeString(PUNICODE_STRING DestinationString, PCANSI_STRING SourceString, BOOLEAN AllocateDestinationString);
	void RtlFreeUnicodeString(PUNICODE_STRING UnicodeString);

private:
	HMODULE hNtdll;
	PNTQueryInformationProcess NtQueryInformation;
	PRtlEqualUnicodeString _RtlEqualUnicodeString;
	PRtlInitUnicodeString _RtlInitUnicodeString;
	PRtlAnsiStringToUnicodeString _RtlAnsiStringToUnicodeString;
	PRtlFreeUnicodeString _RtlFreeUnicodeString;
};