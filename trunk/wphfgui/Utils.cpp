//---------------------------------------------------------------------------

#include <vcl.h>
#include <tchar.h>
#include <StrUtils.hpp>
#pragma hdrstop

#include "Utils.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

typedef struct _TVersion {
	union {
		struct {
			DWORD verMS;
			DWORD verLS;
		} v1;
		struct {
			WORD verW2;
			WORD verW1;
			WORD verW4;
			WORD verW3;
		} v2;
	};
} TVersion;

UnicodeString GetVersionDescription(HMODULE AModule)
{
	UnicodeString Result, attr;
	try {
		HRSRC rsrc = FindResource(AModule, MAKEINTRESOURCE(1), RT_VERSION);
		if (rsrc) {
			HGLOBAL glob = LoadResource(AModule, rsrc);
			if (glob) {
				try {
					LPVOID ptr = LockResource(glob);
					if (ptr) {
						TMemoryStream *mem = new TMemoryStream();
						try {
							VS_FIXEDFILEINFO *ffi;
							UINT len;
							TVersion ver;

							mem->Position = 0;
							mem->WriteBuffer(ptr, SizeofResource(AModule, rsrc));
							if (VerQueryValue(mem->Memory, _T("\\"), (LPVOID*)&ffi, &len)) {
								ver.v1.verMS = ffi->dwFileVersionMS;
								ver.v1.verLS = ffi->dwFileVersionLS;
								TVarRec args[] = {
									ver.v2.verW1,
									ver.v2.verW2,
									ver.v2.verW3,
									ver.v2.verW4
								};
								Result = Format(L"%d.%d.%d.%d", args, 4);
								if ((ffi->dwFileFlagsMask & VS_FF_DEBUG) &&
								(ffi->dwFileFlags & VS_FF_DEBUG))
									attr = L",debug";
								if ((ffi->dwFileFlagsMask & VS_FF_PRERELEASE) &&
								(ffi->dwFileFlags & VS_FF_PRERELEASE))
									attr += L",prerelease";
								if ((ffi->dwFileFlagsMask & VS_FF_PATCHED) &&
								(ffi->dwFileFlags & VS_FF_PATCHED))
									attr += L",patched";
								if ((ffi->dwFileFlagsMask & VS_FF_SPECIALBUILD) &&
								(ffi->dwFileFlags & VS_FF_SPECIALBUILD))
									attr += L",special build";
								if ((ffi->dwFileFlagsMask & VS_FF_PRIVATEBUILD) &&
								(ffi->dwFileFlags & VS_FF_PRIVATEBUILD))
									attr += L",private build";
								if (attr.Length() > 0) {
									Result += L" ";
									Result += attr.SubString(2, MaxInt);
								}
							}
						}
						__finally {
							delete mem;
						}
					}
				}
				__finally {
					FreeResource(glob);
				}
			}
		} else
			return L"<no version info available>";
	} catch(...) {
		return L"";
	}

	return Result;
}
//---------------------------------------------------------------------------

UnicodeString PurgeNumber(UnicodeString Number)
{
	//remove trailing and leading spaces and semicolons
	int i, j;

	i = 1;
	while (i <= Number.Length() &&
	(Number[i] == L';' || Number[i] == L' ' || Number[i] == L'\t' ||
	Number[i] == L'\r' || Number[i] == L'\n'))
		i++;

	j = Number.Length();
	while (j >= 1 &&
	(Number[j] == L';' || Number[j] == L' ' || Number[j] == L'\t' ||
	Number[j] == L'\r' || Number[j] == L'\n'))
		j--;

	if (i >= j)
		return L"";

	Number = Number.SubString(i, j - i + 1);

	//remove surrounding quotes, if any
	if (Number[1] == L'"') {
		if (Number.Length() < 3 || Number[Number.Length()] != L'"')
			L"";

		Number = Number.SubString(2, Number.Length() - 2);
	} else if (Number.Length() == 0)
		L"";

	//remove inner double quotes, we don't handle them well for the moment
	Number = ReplaceText(Number, L"\"", L"");

	//add surrounding double quotes back if needed
	if (Number.Pos(L";") != 0)
		Number = L"\"" + Number + L"\"";

	return Number;
}

