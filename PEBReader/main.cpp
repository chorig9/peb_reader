#include <windows.h>
#include <winternl.h>
#include <iostream>

HRESULT UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA) {
	ULONG cbAnsi, cCharacters;
	DWORD dwError;
	// If input is null then just return the same.    
	if (pszW == NULL)
	{
		*ppszA = NULL;
		return NOERROR;
	}
	cCharacters = wcslen(pszW) + 1;
	cbAnsi = cCharacters * 2;

	*ppszA = (LPSTR)CoTaskMemAlloc(cbAnsi);
	if (NULL == *ppszA)
		return E_OUTOFMEMORY;

	if (0 == WideCharToMultiByte(CP_ACP, 0, pszW, cCharacters, *ppszA, cbAnsi, NULL, NULL))
	{
		dwError = GetLastError();
		CoTaskMemFree(*ppszA);
		*ppszA = NULL;
		return HRESULT_FROM_WIN32(dwError);
	}
	return NOERROR;
}

int main()
{

	PTEB tebPtr = reinterpret_cast<PTEB>(__readfsdword(reinterpret_cast<DWORD_PTR>(&static_cast<NT_TIB*>(nullptr)->Self)));
	PPEB pebPtr = tebPtr->ProcessEnvironmentBlock;
	PPEB_LDR_DATA pebLdr = pebPtr->Ldr;

	PLIST_ENTRY le = (PLIST_ENTRY)pebLdr->InMemoryOrderModuleList.Flink;
	PLDR_DATA_TABLE_ENTRY mainModule = CONTAINING_RECORD(le, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

	PLDR_DATA_TABLE_ENTRY module = nullptr;

	while (module != mainModule)
	{
		le = le->Flink;
		module = CONTAINING_RECORD(le, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

		LPSTR name;
		UnicodeToAnsi(module->FullDllName.Buffer, &name);
		std::cout << name << " " << module->DllBase << " " << std::endl;
	}



	getchar();
}