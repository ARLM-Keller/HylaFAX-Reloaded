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
#include <JclDateTime.hpp>
#include <DateUtils.hpp>
#include <Ansistrings.hpp>
#include <shlobj.h>
#include <gnugettext.hpp>
#include <stdio.h>
#include "pcre.h"
#pragma hdrstop

#include "SendFAX.h"
#include "Recipient.h"
#include "ConfIni.h"
#include "AddrBookCSV.h"
#include "AddrBookMAPI.h"
#include "AddrBookWC.h"
#include "AddrBookODBC.h"
#include "Select.h"
#include "ipc.h"
#include "iapi.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "IdBaseComponent"
#pragma link "IdComponent"
#pragma link "IdTCPClient"
#pragma link "IdTCPConnection"
#pragma link "IdAntiFreeze"
#pragma link "IdAntiFreezeBase"
#pragma link "JvAppInst"
#pragma link "Spin"
#pragma link "IdContext"
#pragma link "IdCustomTCPServer"
#pragma link "IdTCPServer"
#pragma link "IdHTTP"
#pragma resource "*.dfm"

#pragma link "gsdll32.lib"

TFAXSend *FAXSend;

enum {			// data file FORMats
	FORM_UNKNOWN = 0,	// unknown, initial setting
	FORM_PS   = 1,		// PostScript Level I
	FORM_PS2  = 2,		// PostScript Level II
	FORM_TIFF = 3,		// TIFF
	FORM_PCL  = 4,		// HP PCL5
	FORM_PDF  = 5		// Portable Document Format
};

//---------------------------------------------------------------------------
int __cdecl IpcCallback(LPCSTR szFile, LPVOID param)
{
	//data arived through pipe
	TFAXSend *form = static_cast<TFAXSend *>(param);

	//do it via a message to synchronize with the main thread
	SendMessage(form->Handle, WM_ADDFAX, 0, (LPARAM)szFile);

	return 0;
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::ParseCommandLine(TStrings* pArgs)
{
	int i;
	bool bToSaw = false;
	bool bAtSaw = false;
	bool bNotifySaw = false;
	bool bRetrySaw = false;
	bool bDialsSaw = false;
	bool bMMSaw = false;
	bool bFileSaw = false;
	UnicodeString filename, arg;

	for (i = 0; i < pArgs->Count; i++) {
		arg = pArgs->Strings[i];

		if (!bMMSaw && SameText(arg, L"--"))
			bMMSaw = true;
		else if (bToSaw) {
			rbSendTogether->Checked = true;
			if (Trim(hFAXNumber->Text).Length() > 0)
				hFAXNumber->Text = hFAXNumber->Text + L";";
			hFAXNumber->Text = hFAXNumber->Text + arg;
			bToSaw = false;
		} else if (bAtSaw) {
			if (SameText(arg, L"now"))
				rbSendNow->Checked = true;
			else {
				TDateTime dt;
				try {
					dt = StrToDateTime(arg);
					hDate->Date = DateOf(dt);
					hTime->Time = TimeOf(dt);
					rbPostpone->Checked = true;
				}
				catch(EConvertError& err) {
					try {
						dt = StrToTime(arg);
						hTime->Time = TimeOf(dt);
						rbPostpone->Checked = true;
					}
					catch(EConvertError& err) {
					}
				}
			}
			bAtSaw = false;
		} else if (bNotifySaw) {
			hNotifyEmail->Text = arg;
			bNotifySaw = false;
		} else if (bRetrySaw) {
			int pos;
			UnicodeString days, hours;
			if ((pos = Pos(L",", arg)) != 0) {
				days = arg.SubString(1, pos - 1);
				hours = arg.SubString(pos + 1, arg.Length() - pos);
				hDays->Value = StrToIntDef(days, 0);
				hHours->Time = StrToTimeDef(hours, hHours->Time);
			} else {
				if (Pos(L":", arg) != 0) {
					hHours->Time = StrToTimeDef(arg, hHours->Time);
					hDays->Value = 0;
				} else {
					hHours->Time = EncodeTime(0, 0, 0, 0);
					hDays->Value = StrToIntDef(arg, hDays->Value);
				}
            }
			bRetrySaw = false;
		} else if (bDialsSaw) {
			hDials->Value = StrToIntDef(arg, 0);
			bDialsSaw = false;
		} else if (!bMMSaw && SameText(arg, L"-to"))
			bToSaw = true;
		else if (!bMMSaw && SameText(arg, L"-at"))
			bAtSaw = true;
		else if (!bMMSaw && SameText(arg, L"-notify"))
			bNotifySaw = true;
		else if (!bMMSaw && SameText(arg, L"-retry"))
			bRetrySaw = true;
		else if (!bMMSaw && SameText(arg, L"-dials"))
			bDialsSaw = true;
		else if (!bMMSaw && SameText(arg, L"-send"))
			FImmediateSend = true;
		else if (!bMMSaw && SameText(arg, L"-autoclose"))
			FAutoClose = true;
		else if (bFileSaw) {
			AddFileToList(arg, filename);
			bFileSaw = false;
		}
		else if (Sysutils::FileExists(filename = ExpandUNCFileName(arg)))
			bFileSaw = true;
	}
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::AddFileToList(const UnicodeString& Title,
	const UnicodeString& FileName)
{
	TFileData *data = new TFileData();
	data->FileName = FileName;

	UnicodeString number;
	if (RegExProcessDocument(FileName, number)) {
		data->FaxNumber = number;
		data->HasNumber = GetNumbersCount(number) > 0;

		if (rbSendTogether->Checked) {
			UnicodeString curnum = hFAXNumber->Text.Trim();
			if (curnum.Length() > 0 &&
			curnum.CompareIC(number) != 0) {
				//current number is different from that of previously added
				//documents so change send mode to "individually"
				rbSendIndividually->Checked = true;
			} else {
				//same number, or first submitted document
				FSettingNumberEdit = true;
				hFAXNumber->Text = number;
				FSettingNumberEdit = false;
			}
		}
	} else
		data->HasNumber = false;

	LockDocuments();
	lbDocuments->Items->BeginUpdate();
	try {
		lbDocuments->AddItem(Title, data);
		lbDocuments->Checked[lbDocuments->Items->Count - 1] = true;
	}
	__finally {
		lbDocuments->Items->EndUpdate();
		UnlockDocuments();
	}
}

//---------------------------------------------------------------------------
typedef struct _GSDATA {
	HANDLE hPipe;
    TStringList *pLines;
} GSDATA, *LPGSDATA;

//---------------------------------------------------------------------------
static DWORD WINAPI PipeProc(void *param) {
	LPGSDATA data = (LPGSDATA)param;
	const unsigned int bufsize = 8192;
	wchar_t *buf;
	char extra;
	DWORD dwRead, dwRead2;
	UnicodeString line;

	if ((buf = (wchar_t *)VirtualAlloc(NULL, bufsize, MEM_RESERVE | MEM_COMMIT,
	PAGE_READWRITE)) == NULL)
		return 1;

	try {
		for (;;) {
			BOOL bRes = ReadFile(data->hPipe, buf, bufsize - 2, &dwRead, NULL);

			if (bRes && (dwRead & 1)) {
				//odd number of bytes? try to read another one
				BOOL bRes2 = ReadFile(data->hPipe, &extra, 1, &dwRead2, NULL);

				if (bRes2 && dwRead2 != 0)
					((char *)buf)[dwRead++] = extra;
			}

			if (!bRes || dwRead == 0) {
				//is there any data pending?
				if (line.Length() > 0)
					data->pLines->Add(line);
				break; //done with reading
			}

			wchar_t *ptr = buf;
			DWORD n;
			DWORD len = dwRead / sizeof(wchar_t);

			//copy data line by line
			for (n = 0; n < len; n++) {
				if (buf[n] == L'\n') {
					//strip (\r)\n
					buf[n] = L'\0';
					if (n > 0 && buf[n - 1] == L'\r')
						buf[n - 1] = L'\0';
					//append data
					line += ptr;
					line = line.Trim();
					if (line.Length() > 0)
						data->pLines->Add(line);
					//reset and start from next byte
					line = L"";
					ptr = &(buf[n + 1]);
				}
			}

			//did we reach the end of file and still have data aside?
			if (n == len - 1) {
				//strip (\r)\n
				if (buf[n - 1] == L'\r')
					buf[n - 1] = L'\0';
				buf[len - 1] = L'\0';
				//append data
				line += ptr;
				line = line.Trim();
			}
		}
	}
	__finally {
		VirtualFree(buf, 0, MEM_RELEASE);
    }

	return 0;
}

//---------------------------------------------------------------------------
bool __fastcall TFAXSend::FindMatches(const UnicodeString& Line, UnicodeString& Numbers)
{
	int offsets[10];
	int start = 0;
	bool ret = false;

	//match line against regex repeatedly until no matches are found
	for (;;) {
		int matches = pcre16_exec(Fpattern, Fhints, (const unsigned short *)Line.c_str(),
			Line.Length(), start, 0, offsets, 10);

		if (matches < 2)
			break;

		if (Numbers.Length() > 0)
			Numbers += L';';

		//add number to the list
		Numbers += Line.SubString(
			offsets[2] + 1,
			offsets[3] - offsets[2]
		).Trim();

		start = offsets[1];

		ret = true;
	}

	return ret;
}

//---------------------------------------------------------------------------
bool __fastcall TFAXSend::RegExProcessDocument(const UnicodeString& FileName,
	UnicodeString& Numbers)
{
	if (!Fpattern)
		return false;

	//start ghostscript if needed
	if (!Fgsinitialized)
		Fgsinitialized = (gsapi_new_instance(&Fgsinst, NULL) >= 0);

	if (!Fgsinitialized)
		return false;

	int ff = GetFileFormat(FileName);
	if (ff != FORM_PS &&
	ff != FORM_PS2 &&
	ff != FORM_PDF)
		return false;

	bool ret = false;

	HANDLE hRead, hWrite, hThread;

	//create the pipe; ghostscript will write data to it
	if (!CreatePipe(&hRead, &hWrite, NULL, 0))
		return false;

	try {
		GSDATA data;
		data.hPipe = hRead;
		data.pLines = new TStringList();

		try {
			//read ghostscript output in a separate thread
			//so we avoid deadlocks that may be caused by
			//ghostscript waiting for us to read its output, while we're
			//waiting for ghostscript to finish :)
			hThread = CreateThread(NULL, 0, PipeProc, &data, 0, NULL);

			if (hThread == NULL)
				return false;

			try {
				AnsiString sPipe;
				sPipe.sprintf("-sOutputFile=%%handle%%%08x", hWrite);
				AnsiString sFile = FileName;
				AnsiString sExePath = ExtractFilePath(Application->ExeName);
				TVarRec rec[] = {
					sExePath
				};
				AnsiString sPath = Ansistrings::Format("-I\"%s\"", rec, 1);
				char *args[] = {
					"",
					"-q",
					"-dNOPAUSE",
					"-dBATCH",
					"-dSAFER",
					"-sDEVICE=txtwrite",
					"-dTextFormat=2",
					sPath.c_str(),
					sPipe.c_str(),
					sFile.c_str()
				};

				//go, ghostscript, go!!!
				int gscode = gsapi_init_with_args(Fgsinst, 10, args);
				gsapi_exit(Fgsinst);

				//wait for our thread
				if (WaitForSingleObject(hThread, 5000) == WAIT_TIMEOUT)
					CloseHandle(hWrite); //ghostscript died?

				if (WaitForSingleObject(hThread, 5000) == WAIT_TIMEOUT)
					TerminateThread(hThread, 255); //kill it if it hangs

				DWORD dwRet;
				GetExitCodeThread(hThread, &dwRet);

				//check if ghostscript failed or our thread hung
				if (gscode < 0 || dwRet != 0)
					return false;

				//our fax is converted to plain text.
				//search numbers line by line :)
				for (int n = 0; n < data.pLines->Count; n++)
					ret = FindMatches(data.pLines->Strings[n], Numbers) || ret;
			}
			__finally {
				CloseHandle(hThread);
			}
		}
		__finally {
			delete data.pLines;
		}
	}
	__finally {
		CloseHandle(hRead);
	}

	return ret;
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::BringFaxWndToFront()
{
	//restore window if it was minimized
	if (this->WindowState == wsMinimized)
		this->WindowState = wsNormal;
	Application->BringToFront();
	//trick to force window to appear
	//we bring it topmost...
	SetWindowPos(this->Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	//...then we restore its non-topmost state; the window keeps its Z-order
	SetWindowPos(this->Handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

//---------------------------------------------------------------------------
MESSAGE void __fastcall TFAXSend::HandleWMAddFax(TMessage& Message)
{
	UnicodeString data = (LPCWSTR)Message.LParam;

	int pos = data.Pos(L"|");
	UnicodeString filename = ExpandUNCFileName(data.SubString(1, pos - 1));
	UnicodeString title = data.SubString(pos + 1, data.Length() - pos);

	//check parameter sanity
	if (!Sysutils::FileExists(filename))
		return;

	//ok add file to list of files to send
	AddFileToList(title, filename);

	BringFaxWndToFront();
}

//---------------------------------------------------------------------------
__fastcall TFAXSend::TFAXSend(TComponent* Owner)
	: TForm(Owner),
	FHasNumber(false),
	FHasManyNumbers(false),
	FImmediateSend(false),
	FAutoClose(false),
	Fpattern(NULL),
	Fhints(NULL),
	Fchartable(NULL),
	Fgsinst(NULL),
	Fgsinitialized(false)
{
	WCHAR buf[MAX_PATH] = { 0 };

	SHGetFolderPathW(this->Handle, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buf);
	TConfigIni::WPHFUserDir = Sysutils::IncludeTrailingPathDelimiter(buf) + L"Winprint HylaFAX Reloaded\\";

	SHGetFolderPathW(this->Handle, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, buf);
	UnicodeString LocalDir = Sysutils::IncludeTrailingPathDelimiter(buf) + L"Winprint HylaFAX Reloaded\\";

	TConfigIni::WPHFTempDir = LocalDir + L"faxtmp\\";

	TConfigIni::oldWPHFDir = LocalDir;

	//use this often, so translate only once
	SelectFromAB = _(L"Select from address book");

	//ensure directory exists
	Sysutils::ForceDirectories(TConfigIni::WPHFUserDir);

	//move files from previous location
	if (Sysutils::FileExists(TConfigIni::oldWPHFDir + L"wphfgui.ini")) {
		RenameFile(TConfigIni::oldWPHFDir + L"wphfgui.ini",
			TConfigIni::WPHFUserDir + L"wphfgui.ini");
		RenameFile(TConfigIni::oldWPHFDir + L"addressbook.csv",
			TConfigIni::WPHFUserDir + L"addressbook.csv");
		TConfigIni::MovingConfigFiles = true;
		MessageDlg(_(L"Configuration files have been moved to the user profile."),
			mtWarning, TMsgDlgButtons() << mbOK, 0);
	}

	//synchronize access to the documents list box
	InitializeCriticalSection(&CSDocuments);

	//first translate the form
	TranslateComponent(this, L"wphfgui");

	//create config ini
	UnicodeString Ini = TConfigIni::WPHFUserDir + L"wphfgui.ini";
	ConfigIni = new TConfigIni(Ini);
	//attach event handlers
	ConfigIni->OnAddrBookTypeChanged = AddrBookTypeChanged;
	ConfigIni->OnAddrBookLocationChanged = AddrBookLocationChanged;
	ConfigIni->OnLanguageChanged = LanguageChanged;
	ConfigIni->OnRegExChanged = RegExChanged;

	//load config ini
	ConfigIni->Load();

	//first time install?
	if (!Sysutils::FileExists(Ini) &&
	MessageDlg(_(L"Client is not configured. Would you like to configure it now?"),
	mtWarning, TMsgDlgButtons() << mbYes << mbNo, 0) == mrYes) {
		ConfigIni->Configure();
	}
}

//---------------------------------------------------------------------------
__fastcall TFAXSend::~TFAXSend()
{
	//stop pipe
	StopIpc();

	//cleanup
	DeleteCriticalSection(&CSDocuments);

	for (int i = 0; i < lbDocuments->Items->Count; i++) {
		TFileData *file = static_cast<TFileData *>(lbDocuments->Items->Objects[i]);
		//delete file only if it belongs to us
		if (SameFileName(ExtractFilePath(file->FileName), TConfigIni::WPHFTempDir))
			DeleteFile(file->FileName);
		delete file;
	}

	delete ConfigIni;

	if (AddressBook)
		delete AddressBook;

	//free PCRE objects
	if (Fchartable)
		pcre16_free((void *)Fchartable);
	if (Fpattern)
		pcre16_free(Fpattern);
	if (Fhints)
		pcre16_free(Fhints);
	if (Fgsinitialized)
		gsapi_delete_instance(Fgsinst);
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::AddrBookTypeChanged(TObject *Sender, TAddressBookType AType)
{
	//already have one? delete it
	if (AddressBook) {
		AddressBook->Clear();
		delete AddressBook;
	}

	//create new address book
	switch (AType) {
	case abCSV:
		AddressBook = new TAddressBookCSV();
		break;
	case abMAPI:
		AddressBook = new TAddressBookMAPI();
		break;
	case abWinContacts:
		AddressBook = new TAddressBookWC();
		break;
	case abODBC:
		AddressBook = new TAddressBookODBC();
		break;
	default:
		AddressBook = NULL;
	}

	if (AddressBook) {
		//attach event handlers
		AddressBook->OnAddressBookChanged = AddressBookChanged;
		AddressBook->OnAddressBookDuplicate = AddressBookDuplicate;
		//load address book
		try {
			AddressBook->Load();
		}
		catch(EAddressBookException &E) {
			Application->ShowException(&E);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::AddrBookLocationChanged(TObject *Sender)
{
	if (AddressBook) {
		AddressBook->Clear();
		//reload address book from new location
		try {
			AddressBook->Load();
		}
		catch(EAddressBookException &E) {
			Application->ShowException(&E);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::AddressBookChanged(TObject *Sender)
{
	//reload combo
	ABNames->Items->Assign(AddressBook);
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::AddressBookDuplicate(TObject *Sender, const UnicodeString& Name,
	bool& ChangeNumber)
{
	//ask user what to do with duplicate name
	UnicodeString Msg = Name +
		_(L" is already present in the address book.\nDo you want to modify the associated number?");
	ChangeNumber = MessageDlg(Msg, mtConfirmation,
	TMsgDlgButtons() << mbYes << mbNo, 0, mbNo) == mrYes;
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::LanguageChanged(TObject *Sender)
{
	//reload
	SelectFromAB = _(L"Select from address book");

	//translate again
	RetranslateComponent(this, L"wphfgui");
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::RegExChanged(TObject *Sender)
{
	CleanupRE();

	//initialize PCRE objects
	if (ConfigIni->NumberRegEx.Length() > 0)
	{
		if ((Fchartable = pcre16_maketables()) != NULL)
		{
			const char *Error = NULL;
			int ErrorOffset;

			if ((Fpattern = pcre16_compile((const unsigned short *)ConfigIni->NumberRegEx.c_str(),
				0, &Error, &ErrorOffset, Fchartable)) != NULL)
			{
				Fhints = pcre16_study(Fpattern, 0, &Error);
			}

			if (Error && !FAutoClose)
			{
				TVarRec args[2] = {
					ErrorOffset,
					UnicodeString(Error)
				};

				UnicodeString msg = Format(_(L"PCRE error @ position %d: %s"), args, 2);
				MessageDlg(msg, mtError, TMsgDlgButtons() << mbOK, 0);
			}
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::CleanupRE()
{
	//free PCRE objects
	if (Fchartable)
		pcre16_free((void *)Fchartable);
	if (Fpattern)
		pcre16_free(Fpattern);
	if (Fhints)
		pcre16_free(Fhints);

	Fchartable = NULL;
	Fpattern = NULL;
	Fhints = NULL;
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::FormCreate(TObject *Sender)
{
	int i, n;

	hDate->Date = Today();
	hTime->Time = Now();

	hNotifyEmail->Text = ConfigIni->Email;
	hDials->Value = ConfigIni->MaxDials;

	//got parameters from the command line?
	if ((n = ParamCount()) > 0) {
		TStringList* pArgs = new TStringList();
		try {
			for (i = 1; i <= n; i++)
				pArgs->Add(ParamStr(i));
			ParseCommandLine(pArgs);
		}
		__finally {
			delete pArgs;
		}
	}

	if (FImmediateSend)
		actSendExecute(NULL);

	//put window on bottom-right of the screen
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	if (!FAutoClose) {
		Left = rect.right - Width - 8;
		Top = rect.bottom - Height - 8;

		//start pipe
		StartIpc(IpcCallback, this);

		BringFaxWndToFront();
	} else {
		//trick to avoid the window blink
		//if autoclose is true, we put the window outside our desktop area
		Left = rect.right + 100;
		Top = rect.bottom + 100;
	}
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::ABNamesChange(TObject *Sender)
{
	if (AddressBook)
		hFAXNumber->Text = AddressBook->Numbers[ABNames->ItemIndex];
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::EnableFields(bool Enable)
{
	lbDocuments->Enabled = Enable;
	hFAXNumber->Enabled = Enable;
	ABNames->Enabled = Enable;
	hNotifyEmail->Enabled = Enable;
	rbSendNow->Enabled = Enable;
	rbPostpone->Enabled = Enable;
	hDate->Enabled = Enable && rbPostpone->Checked;
	hTime->Enabled = Enable && rbPostpone->Checked;
	hDays->Enabled = Enable;
	hHours->Enabled = Enable;
}

//---------------------------------------------------------------------------
int __fastcall TFAXSend::GetFileFormat(unsigned int magic)
{
	if (magic == 0x53502125) // %!PS
		return FORM_PS;
	else if (magic == 0x002A4949 || magic == 0x2A004D4D) // little endian / big endian
		return FORM_TIFF;
	else if (magic == 0x46445025) // %PDF
		return FORM_PDF;
	else if (magic & 0x00FFFFFF == 0x00214521) // !E!
		return FORM_PCL;

	return FORM_UNKNOWN;
}

//---------------------------------------------------------------------------
int __fastcall TFAXSend::GetFileFormat(const UnicodeString& FileName)
{
	int ret = FORM_UNKNOWN;
	TFileStream *Stream = new TFileStream(FileName, fmOpenRead);

	try {
		try {
			int magic;
			Stream->ReadBuffer(&magic, sizeof(magic));
			ret = GetFileFormat(magic);
		}
		__finally {
			delete Stream;
		}
	}
	catch (...) {
		return FORM_UNKNOWN;
	}

	return ret;
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::actSendExecute(TObject *Sender)
{
	UnicodeString Cmd, JobConfirm;
	TStringList *RemoteFileNames, *Numbers;
	int LineCode, pos;
	unsigned int PassivePort;
	int i, nrdoc, nrdest;
	short int Status, Resp[2];
	bool dummy;

	bool hasDoc = false;

	for (i = 0; i < lbDocuments->Items->Count; i++)
		if (lbDocuments->Checked[i]) {
			hasDoc = true;
			break;
		}

	bool numbersOk = true;

	if (rbSendIndividually->Checked) {
		for (int i = 0; i < lbDocuments->Items->Count; i++) {
			if (!lbDocuments->Checked[i])
				continue;

			TFileData *data = static_cast<TFileData *>(lbDocuments->Items->Objects[i]);
			if (!data->HasNumber) {
				numbersOk = false;
				break;
			}
		}
	} else
		numbersOk = FHasNumber;

	try {
		if (FSending || !hasDoc || !numbersOk || Trim(ConfigIni->Server).Length() == 0)
			return;

		try {
			//lock documents list
			LockDocuments();
			try
			{
				//we're working, disable user interface
				EnableFields(false);
				FSending = true;
				//force actions to reflect state
				//we can't wait for actionsUpdate to be called by the VCL
				//because the call takes place in application idle time.
				//the user might click on some button in the meantime
				actionsUpdate(NULL, dummy);

				try {
					//for remote file names
					RemoteFileNames = new TStringList();
					Numbers = new TStringList();

					try {
						Numbers->Delimiter = L';';
						Numbers->StrictDelimiter = true;

						//not really needed, since we're closing connections in a finally block...
						if (FTPctrl->Connected())
							FTPctrl->Disconnect();

						if (FTPdata->Connected())
							FTPdata->Disconnect();

						//timeouts are user configurable now
						FTPctrl->ConnectTimeout = ConfigIni->Timeout * 1000;
						FTPctrl->ReadTimeout = ConfigIni->Timeout * 1000;

						FTPdata->ConnectTimeout = ConfigIni->Timeout * 1000;
						FTPdata->ReadTimeout = ConfigIni->Timeout * 1000;

						//extract host:port from server
						const UnicodeString& hHost = ConfigIni->Server;
						if ((pos = hHost.Pos(L":")) != 0) {
							FTPctrl->Port = hHost.SubString(pos + 1, hHost.Length()).ToIntDef(4559);
							FTPctrl->Host = hHost.SubString(1, pos - 1);
						} else {
							FTPctrl->Port = 4559;
							FTPctrl->Host = hHost;
						}

						//connect
						FTPctrl->Connect();

						if (ConfigIni->ActiveMode) {
							FTPActivedata->DefaultPort = ConfigIni->Port;
							FTPActivedata->Active = true;
						}

						try {
							//discard greetings
							do {
								LineCode = FTPctrl->GetResponse(-1, TEncoding::ASCII);
							} while (LineCode == 130);

							//authenticate
							Cmd = L"USER " + ConfigIni->Username;
							Resp[0] = 230;
							Resp[1] = 331;
							Status = FTPctrl->SendCmd(Cmd, Resp, 2, TEncoding::ASCII);

							//need password?
							if (Status == 331) {
								Cmd = L"PASS " + ConfigIni->Password;
								FTPctrl->SendCmd(Cmd, 230, TEncoding::ASCII);
							}

							//set time zone to local
							FTPctrl->SendCmd(L"TZONE LOCAL", 200, TEncoding::ASCII);

							//set operating modes to image / stream
							FTPctrl->SendCmd(L"TYPE I", 200, TEncoding::ASCII);
							FTPctrl->SendCmd(L"MODE S", 200, TEncoding::ASCII);

							// Upload files
							for (nrdoc = 0; nrdoc < lbDocuments->Items->Count; nrdoc++) {
								//for each document...
								if (!lbDocuments->Checked[nrdoc])
									continue;

								if (ConfigIni->ActiveMode) {
									UnicodeString myIP, ans;
									switch (ConfigIni->UseIP) {
									case ipUseCurrent:
										myIP = FTPctrl->Socket->Binding->IP;
										break;
									case ipUseThis:
										myIP.sprintf(L"%d.%d.%d.%d",
											(ConfigIni->IP & 0xFF000000) >> 24,
											(ConfigIni->IP & 0x00FF0000) >> 16,
											(ConfigIni->IP & 0x0000FF00) >> 8,
											(ConfigIni->IP & 0x000000FF)
										);
										break;
									case ipUseDynDNS:
										ans = checkip->Get(L"http://checkip.dyndns.com/");
										if ((pos = ans.Pos(L"Current IP Address: ")) != 0) {
											ans = ans.SubString(pos + 20, ans.Length());
											if ((pos = ans.Pos(L"<")) != 0) {
												myIP = ans.SubString(1, pos - 1);
											}
										}
										break;
									default:
										myIP = FTPctrl->Socket->Binding->IP;
										break;
									}
									Cmd = L"EPRT |1|" + myIP + L"|" + IntToStr(ConfigIni->Port) + L"|";
									FTPctrl->SendCmd(Cmd, 200, TEncoding::ASCII);
								} else {
									//send PASV command
									FTPctrl->SendCmd(L"PASV", 227, TEncoding::ASCII);

									//process answer
									UnicodeString pIP = FTPctrl->LastCmdResult->Text->Strings[0];
									pIP = pIP.SubString(pIP.Pos(L"(") + 1, pIP.Length());
									UnicodeString pHost = L"";
									//first 4 numbers are IP address
									for (i = 0; i < 4; i++) {
										pos = pIP.Pos(L",");
										pHost += pIP.SubString(1, pos - 1) + L".";
										pIP = pIP.SubString(pos + 1, pIP.Length());
									}
									//remove trailing dot
									pHost.SetLength(pHost.Length() - 1);
									//now the port
									//high byte
									pos = pIP.Pos(L",");
									UnicodeString pPort = pIP.SubString(1, pos - 1);
									PassivePort = pPort.ToInt() * 256;
									//low byte
									pPort = pIP.SubString(pos + 1, pIP.Length()).TrimRight();
									pPort.SetLength(pPort.Length() - 1);
									PassivePort += pPort.ToInt();

									//setup data connection
									FTPdata->Port = PassivePort;
									if (ConfigIni->PassiveIPIgnore)
										FTPdata->Host = FTPctrl->Host;
									else
										FTPdata->Host = pHost;
								}

								TFileData *data = static_cast<TFileData *>(lbDocuments->Items->Objects[nrdoc]);

								//try to get file format from magic number
								switch (GetFileFormat(data->FileName)) {
								case FORM_PS:
									FTPctrl->SendCmd(L"FORM PS", 200, TEncoding::ASCII);
									break;
								case FORM_TIFF:
									FTPctrl->SendCmd(L"FORM TIFF", 200, TEncoding::ASCII);
									break;
								case FORM_PCL:
									FTPctrl->SendCmd(L"FORM PCL", 200, TEncoding::ASCII);
									break;
								case FORM_PDF:
									FTPctrl->SendCmd(L"FORM PDF", 200, TEncoding::ASCII);
									break;
								default:
									;
								}

								if (!ConfigIni->ActiveMode) {
									//connect
									FTPdata->Connect();
								}

								try {
									if (ConfigIni->ActiveMode)
										FFileBeingSent = data->FileName;

									//send STOT command
									FTPctrl->SendCmd(L"STOT", 150, TEncoding::ASCII);

									if (!ConfigIni->ActiveMode)
										//perform file transfer
										FTPdata->IOHandler->WriteFile(data->FileName, false);

									//get answer: skip "FILE: " and keep till "(Opening"
									UnicodeString temp = FTPctrl->LastCmdResult->Text->Strings[0];
									temp = temp.SubString(7, temp.Pos(L" (Opening") - 7);

									if (rbSendTogether->Checked)
										RemoteFileNames->Add(temp);
									else
										data->RemoteFileName = temp;
								}
								__finally {
									if (!ConfigIni->ActiveMode)
										FTPdata->Disconnect();
								}

								FTPctrl->GetResponse(226, TEncoding::ASCII); // Discard
							} // end for loop upload documents

							bool first = true;

							for (nrdoc = 0; nrdoc < lbDocuments->Items->Count; nrdoc++) {
								if (!lbDocuments->Checked[nrdoc])
									continue;

								TFileData *data = static_cast<TFileData *>(lbDocuments->Items->Objects[nrdoc]);

								Numbers->DelimitedText = rbSendTogether->Checked
									? hFAXNumber->Text
									: data->FaxNumber;

								//setup FAX
								for (nrdest = 0; nrdest < Numbers->Count; nrdest++) {
									UnicodeString Number = Numbers->Strings[nrdest].Trim();

									//check number is not empty
									if (Number.Length() == 0)
										continue;

									//extract cover name and subaddress
									UnicodeString Recipient;
									if ((pos = Number.Pos(L"@")) != 0) {
										Recipient = Number.SubString(1, pos - 1).Trim();
										Number = Number.SubString(pos + 1, Number.Length()).Trim();
									} else {
										if (AddressBook) {
											//try reverse lookup: number -> name
											int idx = AddressBook->IndexOfNumber(Number);
											Recipient = AddressBook->Names[idx];
										} else
											Recipient = Number;
									}

									UnicodeString SubAddress;
									if ((pos = Number.Pos(L"#")) != 0) {
										SubAddress = Number.SubString(pos + 1, Number.Length()).Trim();
										Number = Number.SubString(1, pos - 1).Trim();
									}

									//check again
									if (Number.Length() == 0)
										continue;

									if (!first)
										FTPctrl->SendCmd(L"JOB DEFAULT", 200, TEncoding::ASCII);

									FTPctrl->SendCmd(L"JNEW", 200, TEncoding::ASCII);

									//send at specified date / time
									if (rbPostpone->Checked) {
										TDateTime dt = DateOf(hDate->Date) + TimeOf(hTime->Time);
										TDateTime utc = LocalDateTimeToDateTime(dt);
										Cmd = L"JPARM SENDTIME " +
											  Sysutils::FormatDateTime(L"yyyymmddHHnn", utc);
										FTPctrl->SendCmd(Cmd, 213, TEncoding::ASCII);
									}

									TVarRec args[3] = {
										StrToIntDef(hDays->Text, 0),
										HourOf(hHours->Time),
										MinuteOf(hHours->Time)
									};
									Cmd = Sysutils::Format(L"JPARM LASTTIME %.2d%.2d%.2d", args, 3);
									FTPctrl->SendCmd(Cmd, 213, TEncoding::ASCII);

									//override server's max dials
									if (hDials->Value != 0) {
										Cmd = L"JPARM MAXDIALS " + IntToStr(hDials->Value);
										FTPctrl->SendCmd(Cmd, 213, TEncoding::ASCII);
									}

									//dial string, cover name and subaddress
									Cmd = L"JPARM DIALSTRING \"" + Number + L"\"";
									FTPctrl->SendCmd(Cmd, 213, TEncoding::ASCII);

									if (Recipient.Length() > 0) {
										Cmd = L"JPARM TOUSER \"" + Recipient + L"\"";
										//FTPctrl->SendCmd(Cmd, 213, TEncoding::ASCII);
										FTPctrl->SendCmd(Cmd, 213, TEncoding::UTF8);
									}

									if (SubAddress.Length() > 0) {
										Cmd = L"JPARM SUBADDR \"" + SubAddress + L"\"";
										FTPctrl->SendCmd(Cmd, 213, TEncoding::ASCII);
									}

									//Resolution
									switch (ConfigIni->Resolution) {
									case frFine:
										FTPctrl->SendCmd(L"JPARM VRES 196", 213, TEncoding::ASCII);
										break;
									case frSuperfine:
										FTPctrl->SendCmd(L"JPARM USEXVRES YES", 213, TEncoding::ASCII);
										FTPctrl->SendCmd(L"JPARM VRES 391", 213, TEncoding::ASCII);
										break;
									case frUltrafine:
										FTPctrl->SendCmd(L"JPARM USEXVRES YES", 213, TEncoding::ASCII);
										FTPctrl->SendCmd(L"JPARM VRES 300", 213, TEncoding::ASCII);
										break;
									case frHyperfine:
										FTPctrl->SendCmd(L"JPARM USEXVRES YES", 213, TEncoding::ASCII);
										FTPctrl->SendCmd(L"JPARM VRES 400", 213, TEncoding::ASCII);
										break;
									case frStandard:
									default:
										FTPctrl->SendCmd(L"JPARM VRES 98", 213, TEncoding::ASCII);
										break;
									}

									//paper size
									if (ConfigIni->PageSize == fsA4) {
										FTPctrl->SendCmd(L"JPARM PAGEWIDTH 209", 213, TEncoding::ASCII);
										FTPctrl->SendCmd(L"JPARM PAGELENGTH 296", 213, TEncoding::ASCII);
									}

									//notification
									if (Trim(hNotifyEmail->Text).Length() > 0) {
										Cmd = L"JPARM NOTIFYADDR " + hNotifyEmail->Text;
										FTPctrl->SendCmd(Cmd, 213, TEncoding::ASCII);

										switch (ConfigIni->NotificationType) {
										case fnSuccessOnly:
											FTPctrl->SendCmd(L"JPARM NOTIFY done", 213, TEncoding::ASCII);
											break;
										case fnFailureOnly:
											FTPctrl->SendCmd(L"JPARM NOTIFY requeue", 213, TEncoding::ASCII);
											break;
										case fnNone:
											FTPctrl->SendCmd(L"JPARM NOTIFY none", 213, TEncoding::ASCII);
											break;
										case fnFailureSuccess:
										default:
											FTPctrl->SendCmd(L"JPARM NOTIFY done+requeue", 213, TEncoding::ASCII);
											break;
										}
									} else {
										FTPctrl->SendCmd(L"JPARM NOTIFY none", 213, TEncoding::ASCII);
									}

									//modem
									if (Trim(ConfigIni->Modem).Length() > 0) {
										Cmd = L"JPARM MODEM " + ConfigIni->Modem;
										FTPctrl->SendCmd(Cmd, 213, TEncoding::ASCII);
									}

									//documents to send
									if (rbSendTogether->Checked) {
										for (i = 0; i < RemoteFileNames->Count; i++) {
											Cmd = L"JPARM DOCUMENT " + RemoteFileNames->Strings[i];
											FTPctrl->SendCmd(Cmd, 200, TEncoding::ASCII);
										}
									} else {
										Cmd = L"JPARM DOCUMENT " + data->RemoteFileName;
										FTPctrl->SendCmd(Cmd, 200, TEncoding::ASCII);
									}

									//go!
									FTPctrl->SendCmd(L"JSUBM", 200, TEncoding::ASCII);

									JobConfirm += (first ? L"" : L"\r\n") + FTPctrl->LastCmdResult->Text->Strings[0];

									first = false;
								} // end for loop numbers

								// if all documents sent as a single fax, exit loop,
								// otherwise pass to the next document
								if (rbSendTogether->Checked)
									break;
							}
						}
						__finally {
							FTPctrl->Disconnect();
							if (ConfigIni->ActiveMode)
								FTPActivedata->Active = false;
						}

						//all fine
						if (!FAutoClose)
							MessageDlg(JobConfirm, mtInformation, TMsgDlgButtons() << mbOK, 0);
					}
					__finally
					{
						delete RemoteFileNames;
						delete Numbers;
					}

					i = 0;
					for (;;) {
						if (i >= lbDocuments->Items->Count)
							break;
						if (lbDocuments->Checked[i]) {
							TFileData *file = static_cast<TFileData *>(lbDocuments->Items->Objects[i]);
							//delete file only if it belongs to us
							if (SameFileName(ExtractFilePath(file->FileName), TConfigIni::WPHFTempDir))
								DeleteFile(file->FileName);
							delete file;
							lbDocuments->Items->Delete(i);
						} else {
							i++;
						}
					}
				}
				__finally {
					//whatever happened, enable UI again
					FSending = false;
					EnableFields(true);
				}
			}
			__finally {
				UnlockDocuments();
			}
		}
		catch(...) {
			if (!FAutoClose)
				throw;
			else
				ExitCode = 1;
		}
	}
	__finally {
		if (FAutoClose) {
			Close();
			Application->Terminate();
		}
		FImmediateSend = false;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::actionsUpdate(TBasicAction *Action, bool &Handled)
{
	bool hasDoc = false;

	//do we have at least a checked document in the list?
	for (int i = 0; i < lbDocuments->Items->Count; i++)
		if (lbDocuments->Checked[i]) {
			hasDoc = true;
			break;
		}

	//if we're sending individually, checked documents have associated numbers?
	bool numbersOk = true;

	if (rbSendIndividually->Checked) {
		for (int i = 0; i < lbDocuments->Items->Count; i++) {
			if (!lbDocuments->Checked[i])
				continue;

			TFileData *data = static_cast<TFileData *>(lbDocuments->Items->Objects[i]);
			if (!data->HasNumber) {
				numbersOk = false;
				break;
			}
		}
	} else
		numbersOk = FHasNumber;

	actSend->Enabled = !FSending &&
					   hasDoc &&
					   numbersOk &&
					   Trim(ConfigIni->Server).Length() > 0;
	actUp->Enabled = !FSending &&
					 lbDocuments->Items->Count > 1 &&
					 lbDocuments->ItemIndex > 0;
	actDown->Enabled = !FSending &&
					   lbDocuments->Items->Count > 1 &&
					   lbDocuments->ItemIndex >= 0 &&
					   lbDocuments->ItemIndex < lbDocuments->Items->Count - 1;
	actSave->Enabled = !FSending &&
					   AddressBook &&
					   AddressBook->OnLine &&
					   !AddressBook->ReadOnly &&
					   FHasNumber &&
					   !FHasManyNumbers;
	actDelete->Enabled = !FSending &&
						 AddressBook &&
						 AddressBook->OnLine &&
						 !AddressBook->ReadOnly &&
						 Trim(ABNames->Text).Length() > 0 &&
						 ABNames->Text != SelectFromAB;
	actClose->Enabled = !FSending;
	actConfigure->Enabled = !FSending;
	actSelect->Enabled = !FSending &&
						 AddressBook &&
						 AddressBook->OnLine;
	Handled = true;
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	if (!FAutoClose && lbDocuments->Items->Count > 0 &&
	MessageDlg(_(L"Unsent faxes will be lost. Are you sure you want to exit?"),
	mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0, mbNo) == mrNo)
	{
        CanClose = false;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::actUpExecute(TObject *Sender)
{
	int i = lbDocuments->ItemIndex;
	lbDocuments->Items->Move(i, i - 1);
	lbDocuments->ItemIndex = i - 1;
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::actDownExecute(TObject *Sender)
{
	int i = lbDocuments->ItemIndex;
	lbDocuments->Items->Move(i, i + 1);
	lbDocuments->ItemIndex = i + 1;
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::actSaveExecute(TObject *Sender)
{
	if (!AddressBook || !FHasNumber || FHasManyNumbers)
		return;

	UnicodeString Number = ReplaceStr(hFAXNumber->Text, L";", L"").Trim();

	if (Number.Length() == 0)
		return;

	UnicodeString RecipientName;

	TRecipientName *FRecipientName;

	FRecipientName = new TRecipientName(this);
	try {
		FRecipientName->FAXNumber->Text = Number;
		if (FRecipientName->ShowModal() == mrOk)
			RecipientName = FRecipientName->RecipientName->Text.Trim();
	}
	__finally {
		delete FRecipientName;
	}

	if (RecipientName.Length() > 0) {
		try {
			AddressBook->SetRecipient(RecipientName, Number);
			ABNames->Text = RecipientName;
		}
		catch (EAddressBookUnchanged& E)
		{
			//silently catch EAddressBookUnchanged
        }
	}
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::actDeleteExecute(TObject *Sender)
{
	if (!AddressBook || MessageDlg(_(L"Delete recipient?"), mtConfirmation,
	TMsgDlgButtons() << mbYes << mbNo, 0, mbNo) == mrNo)
    	return;
	AddressBook->DeleteRecipient(ABNames->Text);
	ABNames->Text = _(L"Select from address book");
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::actConfigureExecute(TObject *Sender)
{
	ConfigIni->Configure();
}

//---------------------------------------------------------------------------
void __fastcall TFAXSend::actCloseExecute(TObject *Sender)
{
	Close();
}

//---------------------------------------------------------------------------

void __fastcall TFAXSend::FormActivate(TObject *Sender)
{
	hFAXNumber->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::FormConstrainedResize(TObject *Sender, int &MinWidth, int &MinHeight,
          int &MaxWidth, int &MaxHeight)
{
	MinHeight = 536;
	MinWidth = 446;
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::AppInstInstanceCreated(TObject *Sender, DWORD ProcessId)
{
	BringFaxWndToFront();
}
//---------------------------------------------------------------------------


void __fastcall TFAXSend::AppInstCmdLineReceived(TObject *Sender, TStrings *CmdLine)
{
	ParseCommandLine(CmdLine);

	if (FImmediateSend)
		actSendExecute(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::lbDocumentsMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	//drag'n'drop starts here
	DragPoint.x = X;
	DragPoint.y = Y;
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::lbDocumentsDragOver(TObject *Sender, TObject *Source, int X,
		  int Y, TDragState State, bool &Accept)
{
	//only accept dragging from ourselves
	Accept = (Source == lbDocuments);
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::lbDocumentsDragDrop(TObject *Sender, TObject *Source, int X,
		  int Y)
{
	//move elements
	TCheckListBox *lb = dynamic_cast<TCheckListBox *>(Sender);
	TPoint DropPoint;

	DropPoint.x = X;
	DropPoint.y = Y;
	int DropSource = lb->ItemAtPos(DragPoint, true);
	int DropTarget = lb->ItemAtPos(DropPoint, true);

	if (DropSource >= 0 && DropTarget >= 0 && DropSource != DropTarget) {
		lb->Items->Move(DropSource, DropTarget);
		lb->ItemIndex = DropTarget;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::mnuAddfileClick(TObject *Sender)
{
	if (opendlg->Execute()) {
		AddFileToList(opendlg->FileName, opendlg->FileName);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::actSelectExecute(TObject *Sender)
{
	if (!AddressBook)
		return;

	TSelectRcpt *frm = new TSelectRcpt(this);

	try {
		frm->Numbers = hFAXNumber->Text;
		if (frm->ShowModal() == mrOk)
			hFAXNumber->Text = frm->Numbers;
	}
	__finally {
		delete frm;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::mnuSelectAllClick(TObject *Sender)
{
	lbDocuments->CheckAll(cbChecked, false, false);
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::mnuSelectNoneClick(TObject *Sender)
{
	lbDocuments->CheckAll(cbUnchecked, false, false);
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::rbPostponeClick(TObject *Sender)
{
	hDate->Enabled = rbPostpone->Checked;
	hTime->Enabled = rbPostpone->Checked;
}
//---------------------------------------------------------------------------

int __fastcall TFAXSend::GetNumbersCount(const UnicodeString& Numbers)
{
	int i, count = 0;
	bool bSemicolonSaw = false;
	bool bDigitSaw = false;

	for (i = 1; i <= Numbers.Length(); i++) {
		if (Numbers[i] >= L'0' && Numbers[i] <= L'9') {
			if (bSemicolonSaw) {
				count++;
				break;
			}

			if (!bDigitSaw) {
				count++;
				bDigitSaw = true;
			}
		} else if (Numbers[i] == L';') {
			bDigitSaw = false;
			bSemicolonSaw = true;
		}
	}

	return count;
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::hFAXNumberChange(TObject *Sender)
{
	UnicodeString Number = hFAXNumber->Text.Trim();
	int count = GetNumbersCount(Number);

	FHasNumber = count > 0;
	FHasManyNumbers = count > 1;

	if (!FSettingNumberEdit) {
		int idx = lbDocuments->ItemIndex;
		if (idx < 0)
			return;

		TFileData *data = static_cast<TFileData *>(lbDocuments->Items->Objects[idx]);
		data->FaxNumber = Number;
		data->HasNumber = FHasNumber;

		lbDocuments->Invalidate();
	}
}

//---------------------------------------------------------------------------

void __fastcall TFAXSend::FTPActivedataExecute(TIdContext *AContext)
{
	AContext->Connection->IOHandler->WriteFile(FFileBeingSent, false);
	AContext->Connection->Disconnect();
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::lbDocumentsClick(TObject *Sender)
{
	if (rbSendIndividually->Checked) {
		int idx = lbDocuments->ItemIndex;
		if (idx < 0)
			return;

		FSettingNumberEdit = true; // prevent infinite loop
		hFAXNumber->Text = ((TFileData*)lbDocuments->Items->Objects[idx])->FaxNumber;
		FSettingNumberEdit = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::lbDocumentsDrawItem(TWinControl *Control, int Index, TRect &Rect,
		  TOwnerDrawState State)
{
	TFileData *data = static_cast<TFileData *>(lbDocuments->Items->Objects[Index]);
	SIZE s;
	UnicodeString out;
	int w;

	if (rbSendIndividually->Checked)
		w = Rect.Width() / 2;
	else
		w = Rect.Width();

	lbDocuments->Canvas->FillRect(Rect);

	UnicodeString text;

	text = lbDocuments->Items->Strings[Index].Trim();
	s = lbDocuments->Canvas->TextExtent(text);
	if (s.cx > w) {
		int newlen = (text.Length() * w / s.cx) - 3;
		if (newlen < 0)
			newlen = 0;
		out = text.SubString(1, newlen) + L"...";
	} else
		out = text;

	if (lbDocuments->Checked[Index]) {
		if (State.Contains(odSelected))
			lbDocuments->Canvas->Font->Color = clBlack;
		else
			lbDocuments->Canvas->Font->Color = clNavy;
	} else
		lbDocuments->Canvas->Font->Color = clGray;

	lbDocuments->Canvas->TextOutW(Rect.Left, Rect.Top, out);

	if (rbSendIndividually->Checked) {
		lbDocuments->Canvas->MoveTo(Rect.Left + w + 1, Rect.Top);
		lbDocuments->Canvas->LineTo(Rect.Left + w + 1, Rect.Bottom);

		text = data->FaxNumber.Trim();
		s = lbDocuments->Canvas->TextExtent(text);
		if (s.cx > w) {
			int newlen = (text.Length() * w / s.cx) - 3;
			if (newlen < 0)
				newlen = 0;
			out = text.SubString(1, newlen) + L"...";
		} else
			out = text;

		if (lbDocuments->Checked[Index]) {
			if (State.Contains(odSelected))
				lbDocuments->Canvas->Font->Color = clWhite;
			else
				lbDocuments->Canvas->Font->Color = clGreen;
		} else
			lbDocuments->Canvas->Font->Color = clGray;

		lbDocuments->Canvas->TextOutW(Rect.Left + w + 6, Rect.Top, out);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::FormResize(TObject *Sender)
{
	//gosh darn it, why the f@*! it does not repaint correctly,
	//unless I make this horrible call???
	lbDocuments->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TFAXSend::rbSendModeClick(TObject *Sender)
{
	lbDocuments->Invalidate();

	if (Sender == rbSendIndividually) {
		FSettingNumberEdit = true;
		hFAXNumber->Clear();
		FSettingNumberEdit = false;
	}
}
//---------------------------------------------------------------------------

