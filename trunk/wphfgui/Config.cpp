/*
Winprint HylaFAX Reloaded
Copyright (C) 2011-2013 Monti Lorenzo

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

//---------------------------------------------------------------------------

#include <vcl.h>
#include <shlobj.h>
#include <gnugettext.hpp>
#pragma hdrstop

#include "Config.h"
#include "ConfIni.h"
#include "Utils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Spin"
#pragma link "JvExStdCtrls"
#pragma link "JvHtControls"
#pragma resource "*.dfm"

TConfigForm *ConfigForm;
//---------------------------------------------------------------------------

__fastcall TConfigForm::TConfigForm(TComponent* Owner)
	: TForm(Owner),
	Pattern(NULL),
	Hints(NULL),
	Chartable(NULL)
{
}
//---------------------------------------------------------------------------

__fastcall TConfigForm::~TConfigForm()
{
	CleanupRE();
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::btnBrowseClick(TObject *Sender)
{
	WCHAR buf[MAX_PATH];
	BROWSEINFOW bi;
	PIDLIST_ABSOLUTE pidl;

	wcscpy_s(buf, MAX_PATH, hAddressBook->Text.c_str());

	bi.hwndOwner = this->Handle;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = buf;
	bi.lpszTitle = _(L"Choose address book location").c_str();
	bi.ulFlags = BIF_EDITBOX;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	bi.iImage = 0;

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if ((pidl = SHBrowseForFolderW(&bi)) != NULL) {
		if (SHGetPathFromIDListW(pidl, buf))
			hAddressBook->Text = buf;
		IMalloc *pMalloc = NULL;
		SHGetMalloc(&pMalloc);
		pMalloc->Free(pidl);
		pMalloc->Release();
	}
	CoUninitialize();
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::btnDefaultClick(TObject *Sender)
{
	hAddressBook->Text = ConfigIni->DefaultAddrBookPath;
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	UnicodeString Path = Trim(hAddressBook->Text);
	if (Path.Length() > 0 &&
	!DirectoryExists(Path)) {
		MessageDlg(_(L"Specify a valid path for the Address Book"),
			mtError, TMsgDlgButtons() << mbOK, 0);
		CanClose = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::FormConstrainedResize(TObject *Sender, int &MinWidth,
          int &MinHeight, int &MaxWidth, int &MaxHeight)
{
	MinHeight = 580;
	MinWidth = 466;
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::FormCreate(TObject *Sender)
{
	TranslateComponent(this, L"wphfgui");

	lblVersion->Caption = _("rel. ") + GetVersionDescription(NULL);

	TStringList *langs = new TStringList();
	try {
		DefaultInstance->GetListOfLanguages(L"wphfgui", langs);
		hLanguage->Items->Add(L"<auto>");
		hLanguage->Items->Add(L"en");
		hLanguage->Items->AddStrings(langs);
	}
	__finally {
		delete langs;
	}

	DefaultInstance->TranslateProperties(hLanguage, L"languages");
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::hMAPIProfileChange(TObject *Sender)
{
	hMAPIUseProfile->Checked = true;
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::hODBCAuthClick(TObject *Sender)
{
	hODBCUser->ReadOnly = !hODBCAuth->Checked;
	hODBCPassword->ReadOnly = !hODBCAuth->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::CleanupRE()
{
	//free PCRE objects
	if (Chartable)
		pcre16_free((void *)Chartable);
	if (Pattern)
		pcre16_free(Pattern);
	if (Hints)
		pcre16_free(Hints);

	Chartable = NULL;
	Pattern = NULL;
	Hints = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::PerformMatch()
{
	if (!Pattern)
		return;

	int offsets[10];

	int matches = pcre16_exec(Pattern, Hints, (const unsigned short *)hInputText->Text.c_str(),
		hInputText->Text.Length(), 0, 0, offsets, 10);

	if (matches < 2) {
		hMatch->Text = _(L"No match!");
		return;
	}

	hMatch->Text = hInputText->Text.SubString(
		offsets[2] + 1,
		offsets[3] - offsets[2]
	);
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::hRegExChange(TObject *Sender)
{
	CleanupRE();

	if ((Chartable = pcre16_maketables()) != NULL)
	{
		const char *Error = NULL;
		int ErrorOffset;

		if ((Pattern = pcre16_compile((const unsigned short *)hRegEx->Text.c_str(),
			0, &Error, &ErrorOffset, Chartable)) != NULL)
		{
			Hints = pcre16_study(Pattern, 0, &Error);
		}

		if (Error)
		{
			TVarRec args[2] = {
				ErrorOffset,
				UnicodeString(Error)
			};

			UnicodeString msg = Sysutils::Format(L"PCRE error @ position %d: %s", args, 2);
			hMatch->Text = msg;
		}
		else
		{
            PerformMatch();
        }
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::hInputTextChange(TObject *Sender)
{
	PerformMatch();
}
//---------------------------------------------------------------------------


