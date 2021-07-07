#include "NtdllExports.h"
#include <exception>
#include "Tracer.h"

Ntdll::Ntdll()
{
	debugInformation("Loading Ntdll Functions\n");
	hNtdll = 0;
	NtQueryInformation = 0;
	_RtlEqualUnicodeString = 0;
	_RtlInitUnicodeString = 0;
	_RtlAnsiStringToUnicodeString = 0;
	_RtlFreeUnicodeString = 0;
	LoadFunctions();
}

Ntdll::~Ntdll()
{
	FreeLibrary(hNtdll);
}

void Ntdll::LoadFunctions()
{
		if (hNtdll) {
			return;
		}
		hNtdll = LoadLibrary(L"ntdll");

		if (!hNtdll)
			throw std::exception("Fail to load ntdll.dll");
		NtQueryInformation = (PNTQueryInformationProcess)GetProcAddress(hNtdll, "NtQueryInformationProcess");
		if (!NtQueryInformation) {
			throw std::exception("Fail to get NtQueryInformationProcess");
		}
		_RtlEqualUnicodeString = (PRtlEqualUnicodeString)GetProcAddress(hNtdll, "RtlEqualUnicodeString");
		if (!_RtlEqualUnicodeString) {
			throw std::exception("Fail to get RtlEqualUnicodeString");
		}

		_RtlInitUnicodeString = (PRtlInitUnicodeString)GetProcAddress(hNtdll, "RtlInitUnicodeString");
		if (!_RtlInitUnicodeString) {
			throw std::exception("Fail to get RtlInitUnicodeString");
		}

		_RtlAnsiStringToUnicodeString = (PRtlAnsiStringToUnicodeString)GetProcAddress(hNtdll, "RtlAnsiStringToUnicodeString");
		if (!_RtlAnsiStringToUnicodeString) {
			throw std::exception("Fail to get RtlAnsiStringToUnicodeString");
		}
		_RtlFreeUnicodeString = (PRtlFreeUnicodeString)GetProcAddress(hNtdll, "RtlFreeUnicodeString");
		if (!_RtlFreeUnicodeString) {
			throw std::exception("Fail to get RtlFreeUnicodeString");
		}

}

NTSTATUS Ntdll::NtQueryInformationProcess(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength)
{
	return NtQueryInformation(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
}

BOOLEAN Ntdll::RtlEqualUnicodeString(PCUNICODE_STRING String1, PCUNICODE_STRING String2, BOOLEAN CaseInSensitive)
{
	return _RtlEqualUnicodeString(String1, String2, CaseInSensitive);
}

VOID Ntdll::RtlInitUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString)
{
	return _RtlInitUnicodeString(DestinationString, SourceString);
}

NTSTATUS Ntdll::RtlAnsiStringToUnicodeString(PUNICODE_STRING DestinationString, PCANSI_STRING SourceString, BOOLEAN AllocateDestinationString)
{
	return _RtlAnsiStringToUnicodeString(DestinationString, SourceString, AllocateDestinationString);
}

void Ntdll::RtlFreeUnicodeString(PUNICODE_STRING UnicodeString)
{
	return _RtlFreeUnicodeString(UnicodeString);
}
