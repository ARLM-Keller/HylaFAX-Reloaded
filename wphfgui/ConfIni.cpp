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
#include <IniFiles.hpp>
#include <gnugettext.hpp>
#pragma hdrstop

#include "ConfIni.h"
#include "Config.h"

#pragma package(smart_init)

//---------------------------------------------------------------------------
TConfigIni *ConfigIni = NULL;
static const UnicodeString Section = L"PARAMETERS";
static const UnicodeString ServerIdent = L"Server";
static const UnicodeString UseIPIdent = L"UseIP";
static const UnicodeString IPIdent = L"IP";
static const UnicodeString TimeoutIdent = L"Timeout";
static const UnicodeString UsernameIdent = L"Username";
static const UnicodeString PasswordIdent = L"Password";
static const UnicodeString EmailIdent = L"Email";
static const UnicodeString PassiveIPIgnoreIdent = L"PassiveIPIgnore";
static const UnicodeString ActiveModeIdent = L"ActiveMode";
static const UnicodeString PortIdent = L"Port";
static const UnicodeString ResolutionIdent = L"Resolution";
static const UnicodeString PageSizeIdent = L"PageSize";
static const UnicodeString NotificationTypeIdent = L"NotificationType";
static const UnicodeString ModemIdent = L"Modem";
static const UnicodeString MaxDialsIdent = L"MaxDials";
static const UnicodeString AddrBookTypeIdent = L"AddrBookType";
static const UnicodeString AddrBookPathIdent = L"AddrBookPath";
static const UnicodeString MAPIUseDefProfileIdent = L"MAPIUseDefProfile";
static const UnicodeString MAPIProfileIdent = L"MAPIProfile";
static const UnicodeString LanguageIdent = L"Language";
static const UnicodeString ODBCDSNIdent = L"ODBCDSN";
static const UnicodeString ODBCTableIdent = L"ODBCTable";
static const UnicodeString ODBCNameFieldIdent = L"ODBCNameField";
static const UnicodeString ODBCFaxFieldIdent = L"ODBCFaxField";
static const UnicodeString ODBCAuthIdent = L"ODBCAuth";
static const UnicodeString ODBCUidIdent = L"ODBCUid";
static const UnicodeString ODBCPwdIdent = L"ODBCPwd";
static const UnicodeString NumberRegExIdent = L"NumberRegEx";
static const UnicodeString RegExEnabledIdent = L"RegExEnabled";
static const UnicodeString FirstPageOnlyIdent = L"FirstPageOnly";
static const UnicodeString SendSilentlyIdent = L"SendSilently";
bool TConfigIni::MovingConfigFiles = false;
UnicodeString TConfigIni::WPHFUserDir = L"";
UnicodeString TConfigIni::WPHFTempDir = L"";
UnicodeString TConfigIni::oldWPHFDir = L"";
//---------------------------------------------------------------------------

__fastcall TConfigIni::TConfigIni(const UnicodeString& IniFile)
	: TObject(),
	FLanguageChanged(false),
	FAddrBookTypeChanged(false),
	FAddrBookLocationChanged(false),
	FRegExChanged(false),
	FAddrBookType(abNone),
	FODBCAuth(false),
	FRegExEnabled(true),
	FFirstPageOnly(true),
	FSendSilently(false)
{
	FIniFile = IniFile;
}
//---------------------------------------------------------------------------

UnicodeString __fastcall TConfigIni::GetDefaultAddrBookPath()
{
 	return ExtractFilePath(FIniFile);
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::Load()
{
	TIniFile *Ini = new TIniFile(FIniFile);

	try {
		Server = Ini->ReadString(Section, ServerIdent, L"");
		UseIP = (TUseIP)Ini->ReadInteger(Section, UseIPIdent, ipUseCurrent);
		IP = Ini->ReadInteger(Section, IPIdent, 0);
		Timeout = Ini->ReadInteger(Section, TimeoutIdent, 10);
		Username = Ini->ReadString(Section, UsernameIdent, L"");
		Password = Ini->ReadString(Section, PasswordIdent, L"");
		Email = Ini->ReadString(Section, EmailIdent, L"");
		PassiveIPIgnore = Ini->ReadBool(Section, PassiveIPIgnoreIdent, false);
		ActiveMode = Ini->ReadBool(Section, ActiveModeIdent, false);
		Port = Ini->ReadInteger(Section, PortIdent, 1);
		Resolution = (TFaxResolution)Ini->ReadInteger(Section, ResolutionIdent, frStandard);
		PageSize = (TFaxPageSize)Ini->ReadInteger(Section, PageSizeIdent, fsUSLetter);
		NotificationType = (TFaxNotification)Ini->ReadInteger(Section, NotificationTypeIdent, fnFailureSuccess);
		Modem = Ini->ReadString(Section, ModemIdent, L"");
		MaxDials = Ini->ReadInteger(Section, MaxDialsIdent, 0);
		AddrBookType = (TAddressBookType)Ini->ReadInteger(Section, AddrBookTypeIdent, abCSV);
		UnicodeString temp = Ini->ReadString(Section, AddrBookPathIdent, DefaultAddrBookPath);
		if (MovingConfigFiles && temp.CompareIC(oldWPHFDir) == 0) {
			AddrBookPath = WPHFUserDir;
			Ini->WriteString(Section, AddrBookPathIdent, AddrBookPath);
		} else
			AddrBookPath = temp;
		Language = Ini->ReadString(Section, LanguageIdent, L"");
		MAPIUseDefProfile = Ini->ReadBool(Section, MAPIUseDefProfileIdent, true);
		MAPIProfile = Ini->ReadString(Section, MAPIProfileIdent, L"");
		ODBCDSN = Ini->ReadString(Section, ODBCDSNIdent, L"");
		ODBCTable = Ini->ReadString(Section, ODBCTableIdent, L"");
		ODBCNameField = Ini->ReadString(Section, ODBCNameFieldIdent, L"");
		ODBCFaxField = Ini->ReadString(Section, ODBCFaxFieldIdent, L"");
		ODBCAuth = Ini->ReadBool(Section, ODBCAuthIdent, false);
		ODBCUid = Ini->ReadString(Section, ODBCUidIdent, L"");
		ODBCPwd = Ini->ReadString(Section, ODBCPwdIdent, L"");
		NumberRegEx = Ini->ReadString(Section, NumberRegExIdent,
			L"#{3}((?:[^#]|#(?!##))+)#{3}");
		RegExEnabled = Ini->ReadBool(Section, RegExEnabledIdent, true);
		FirstPageOnly = Ini->ReadBool(Section, FirstPageOnlyIdent, true);
		SendSilently = Ini->ReadBool(Section, SendSilentlyIdent, false);
		FireEvents();
	}
	__finally {
		delete Ini;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::Configure()
{
	TStringList *langs = new TStringList();
	TConfigForm *ConfForm = new TConfigForm(NULL);

	try {
		DefaultInstance->GetListOfLanguages(L"wphfgui", langs);
		langs->Insert(0, L"en");

		ConfForm->hServer->Text = Server;
		ConfForm->hPassiveIPIgnore->Checked = PassiveIPIgnore;
		ConfForm->hActiveMode->Checked = ActiveMode;
		ConfForm->hPort->Value = Port;
		switch (UseIP) {
		case ipUseCurrent:
			ConfForm->hUseCurrentIP->Checked = true;
			break;
		case ipUseThis:
			ConfForm->hUseThisIP->Checked = true;
			break;
		case ipUseDynDNS:
			ConfForm->hUseDynDNS->Checked = true;
			break;
		default:
			ConfForm->hUseCurrentIP->Checked = true;
			break;
		}
		ConfForm->hIPAddress->Address = IP;
		ConfForm->hTimeout->Value = Timeout;
		ConfForm->hUsername->Text = Username;
		ConfForm->hPassword->Text = Password;
		ConfForm->hDefaultEmail->Text = Email;
		ConfForm->hNotificationType->ItemIndex = (int)NotificationType;
		ConfForm->hModem->Text = Modem;
		ConfForm->hDials->Value = MaxDials;
		ConfForm->hPageSize->ItemIndex = (int)PageSize;
		ConfForm->hResolution->ItemIndex = (int)Resolution;
		if (Language.Length() == 0) {
			ConfForm->hLanguage->ItemIndex = 0;
		} else {
			int idx = langs->IndexOf(Language);
			if (idx >= 0)
				ConfForm->hLanguage->ItemIndex = idx + 1;
		}
		ConfForm->hAddrBookType->ItemIndex = (int)AddrBookType;
		ConfForm->hAddressBook->Text = AddrBookPath;
		ConfForm->hMAPIDefProfile->Checked = MAPIUseDefProfile;
		ConfForm->hMAPIUseProfile->Checked = !MAPIUseDefProfile;
		ConfForm->hMAPIProfile->Text = MAPIProfile;
		ConfForm->hODBCDSN->Text = ODBCDSN;
		ConfForm->hODBCUser->Text = ODBCUid;
		ConfForm->hODBCPassword->Text = ODBCPwd;
		ConfForm->hODBCTable->Text = ODBCTable;
		ConfForm->hODBCName->Text = ODBCNameField;
		ConfForm->hODBCFax->Text = ODBCFaxField;
		ConfForm->hODBCAuth->Checked = ODBCAuth;
		ConfForm->hODBCUser->ReadOnly = !ODBCAuth;
		ConfForm->hODBCPassword->ReadOnly = !ODBCAuth;
		ConfForm->hRegEx->Text = NumberRegEx;
		ConfForm->hRegExEnabled->Checked = RegExEnabled;
		ConfForm->hFirstPageOnly->Checked = FirstPageOnly;
		ConfForm->hSendSilently->Checked = SendSilently;

		if (ConfForm->ShowModal() == mrOk) {
			try {
				Server = Trim(ConfForm->hServer->Text);
				PassiveIPIgnore = ConfForm->hPassiveIPIgnore->Checked;
				ActiveMode = ConfForm->hActiveMode->Checked;
				Port = ConfForm->hPort->Value;
				if (ConfForm->hUseCurrentIP->Checked)
					UseIP = ipUseCurrent;
				else if (ConfForm->hUseThisIP->Checked)
					UseIP = ipUseThis;
				else if (ConfForm->hUseDynDNS->Checked)
					UseIP = ipUseDynDNS;
				else
					UseIP = ipUseCurrent;
				IP = ConfForm->hIPAddress->Address;
				Timeout = ConfForm->hTimeout->Value;
				Username = ConfForm->hUsername->Text;
				Password = ConfForm->hPassword->Text;
				Email = Trim(ConfForm->hDefaultEmail->Text);
				NotificationType = (TFaxNotification)ConfForm->hNotificationType->ItemIndex;
				Modem = Trim(ConfForm->hModem->Text);
				MaxDials = ConfForm->hDials->Value;
				PageSize = (TFaxPageSize)ConfForm->hPageSize->ItemIndex;
				Resolution = (TFaxResolution)ConfForm->hResolution->ItemIndex;
				AddrBookType = (TAddressBookType)ConfForm->hAddrBookType->ItemIndex;
				AddrBookPath = Trim(ConfForm->hAddressBook->Text);
				if (ConfForm->hLanguage->ItemIndex > 0)
					Language = langs->Strings[ConfForm->hLanguage->ItemIndex - 1];
				else
					Language = L"";
				MAPIUseDefProfile = ConfForm->hMAPIDefProfile->Checked;
				MAPIProfile = ConfForm->hMAPIProfile->Text;
				ODBCDSN = ConfForm->hODBCDSN->Text;
				ODBCAuth = ConfForm->hODBCAuth->Checked;
				ODBCUid = ConfForm->hODBCUser->Text;
				ODBCPwd = ConfForm->hODBCPassword->Text;
				ODBCTable = ConfForm->hODBCTable->Text;
				ODBCNameField = ConfForm->hODBCName->Text;
				ODBCFaxField = ConfForm->hODBCFax->Text;
				NumberRegEx = ConfForm->hRegEx->Text;
				RegExEnabled = ConfForm->hRegExEnabled->Checked;
				FirstPageOnly = ConfForm->hFirstPageOnly->Checked;
				SendSilently = ConfForm->hSendSilently->Checked;

				FireEvents();
			}
			__finally {
				Save();
			}
		}
	}
	__finally {
		delete ConfForm;
		delete langs;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::Save()
{
	TIniFile *Ini = new TIniFile(FIniFile);

	try {
		Ini->WriteString(Section, ServerIdent, Server);
		Ini->WriteString(Section, UsernameIdent, Username);
		Ini->WriteString(Section, PasswordIdent, Password);
		Ini->WriteString(Section, EmailIdent, Email);
		Ini->WriteBool(Section, PassiveIPIgnoreIdent, PassiveIPIgnore);
		Ini->WriteBool(Section, ActiveModeIdent, ActiveMode);
		Ini->WriteInteger(Section, PortIdent, Port);
		Ini->WriteInteger(Section, UseIPIdent, UseIP);
		Ini->WriteInteger(Section, IPIdent, IP);
		Ini->WriteInteger(Section, TimeoutIdent, Timeout);
		Ini->WriteInteger(Section, ResolutionIdent, Resolution);
		Ini->WriteInteger(Section, PageSizeIdent, PageSize);
		Ini->WriteInteger(Section, NotificationTypeIdent, NotificationType);
		Ini->WriteString(Section, ModemIdent, Modem);
		Ini->WriteInteger(Section, MaxDialsIdent, MaxDials);
		Ini->WriteString(Section, AddrBookTypeIdent, AddrBookType);
		Ini->WriteString(Section, AddrBookPathIdent, AddrBookPath);
		Ini->WriteString(Section, LanguageIdent, Language);
		Ini->WriteBool(Section, MAPIUseDefProfileIdent, MAPIUseDefProfile);
		Ini->WriteString(Section, MAPIProfileIdent, MAPIProfile);
		Ini->WriteString(Section, ODBCDSNIdent, ODBCDSN);
		Ini->WriteString(Section, ODBCTableIdent, ODBCTable);
		Ini->WriteString(Section, ODBCNameFieldIdent, ODBCNameField);
		Ini->WriteString(Section, ODBCFaxFieldIdent, ODBCFaxField);
		Ini->WriteBool(Section, ODBCAuthIdent, ODBCAuth);
		Ini->WriteString(Section, ODBCUidIdent, ODBCUid);
		Ini->WriteString(Section, ODBCPwdIdent, ODBCPwd);
		Ini->WriteString(Section, NumberRegExIdent, NumberRegEx);
		Ini->WriteBool(Section, RegExEnabledIdent, RegExEnabled);
		Ini->WriteBool(Section, FirstPageOnlyIdent, FirstPageOnly);
		Ini->WriteBool(Section, SendSilentlyIdent, SendSilently);
	}
	__finally {
		delete Ini;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::SetAddrBookType(TAddressBookType Value)
{
	if (Value != FAddrBookType) {
		FAddrBookType = Value;

		FAddrBookTypeChanged = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::SetAddrBookPath(const UnicodeString& Value)
{
	if (Value.CompareIC(FAddrBookPath) != 0) {
		FAddrBookPath = Value;

		if (FAddrBookType == abCSV)
			FAddrBookLocationChanged = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::SetLanguage(const UnicodeString& Value)
{
	if (Value.CompareIC(FLanguage) != 0) {
		FLanguage = Value;

		DefaultInstance->UseLanguage(FLanguage);

		FLanguageChanged = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::SetRegEx(const UnicodeString& Value)
{
	if (Value.Compare(FNumberRegEx) != 0) {
		FNumberRegEx = Value;

		FRegExChanged = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::SetMAPIUseDefProfile(bool Value)
{
	if (Value != FMAPIUseDefProfile) {
		FMAPIUseDefProfile = Value;

		if (FAddrBookType == abMAPI)
			FAddrBookLocationChanged = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::SetMAPIProfile(const UnicodeString& Value)
{
	if (Value.CompareIC(FMAPIProfile) != 0) {
		FMAPIProfile = Value;

		if (FAddrBookType == abMAPI)
			FAddrBookLocationChanged = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::SetODBCDSN(const UnicodeString& Value)
{
	if (Value.CompareIC(FODBCDSN) != 0) {
		FODBCDSN = Value;

		if (FAddrBookType == abODBC)
			FAddrBookLocationChanged = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::SetODBCTable(const UnicodeString& Value)
{
	if (Value.CompareIC(FODBCTable) != 0) {
		FODBCTable = Value;

		if (FAddrBookType == abODBC)
			FAddrBookLocationChanged = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::SetODBCNameField(const UnicodeString& Value)
{
	if (Value.CompareIC(FODBCNameField) != 0) {
		FODBCNameField = Value;

		if (FAddrBookType == abODBC)
			FAddrBookLocationChanged = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::SetODBCFaxField(const UnicodeString& Value)
{
	if (Value.CompareIC(FODBCFaxField) != 0) {
		FODBCFaxField = Value;

		if (FAddrBookType == abODBC)
			FAddrBookLocationChanged = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::SetODBCAuth(bool Value)
{
	if (Value != FODBCAuth) {
		FODBCAuth = Value;

		if (FAddrBookType == abODBC)
			FAddrBookLocationChanged = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::SetODBCUid(const UnicodeString& Value)
{
	if (Value.Compare(FODBCUid) != 0) {
		FODBCUid = Value;

		if (FAddrBookType == abODBC)
			FAddrBookLocationChanged = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::SetODBCPwd(const UnicodeString& Value)
{
	if (Value.Compare(FODBCPwd) != 0) {
		FODBCPwd = Value;

		if (FAddrBookType == abODBC)
			FAddrBookLocationChanged = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigIni::FireEvents()
{
	if (FLanguageChanged) {
		if (FOnLanguageChanged)
			FOnLanguageChanged(this);

		FLanguageChanged = false;
	}

	if (FAddrBookTypeChanged) {
		if (FOnAddrBookTypeChanged)
			FOnAddrBookTypeChanged(this, FAddrBookType);

		FAddrBookTypeChanged = false;
	} else if (FAddrBookLocationChanged) {
		if (FOnAddrBookLocationChanged && FAddrBookType != abNone)
			FOnAddrBookLocationChanged(this);

		FAddrBookLocationChanged = false;
	}

	if (FRegExChanged) {
		if (FOnRegExChanged)
			FOnRegExChanged(this);

		FRegExChanged = false;
    }
}

