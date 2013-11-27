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

#ifndef SendFAXH
#define SendFAXH

#include <ActnList.hpp>
#include <Buttons.hpp>
#include <CheckLst.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
#include <StdCtrls.hpp>
#include "IdBaseComponent.hpp"
#include "IdComponent.hpp"
#include "IdTCPClient.hpp"
#include "IdTCPConnection.hpp"
#include "IdAntiFreeze.hpp"
#include "IdAntiFreezeBase.hpp"
#include "JvAppInst.hpp"
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include "Spin.hpp"

#include "ConfIni.h"
#include "IdContext.hpp"
#include "IdCustomTCPServer.hpp"
#include "IdTCPServer.hpp"
#include "IdHTTP.hpp"
#include "pcre.h"

//---------------------------------------------------------------------------

class TFileData : public TObject
{
public:
	UnicodeString FileName;
	UnicodeString FaxNumber;
	bool HasNumber;
	UnicodeString RemoteFileName;
};

extern int __cdecl IpcCallback(LPCSTR szFile, LPVOID param);

#define WM_ADDFAX WM_USER + 2000
#define WM_IMMEDIATESEND WM_USER + 2001

class TFAXSend : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TImage *Image1;
	TPanel *Panel2;
	TPanel *Panel3;
	TButton *Cancel;
	TButton *Send;
	TLabel *Label4;
	TButton *Configure;
	TLabel *Label2;
	TActionList *actions;
	TAction *actSend;
	TAction *actUp;
	TAction *actDown;
	TAction *actSave;
	TAction *actDelete;
	TIdTCPClient *FTPctrl;
	TIdTCPClient *FTPdata;
	TIdAntiFreeze *AntiFreeze;
	TAction *actConfigure;
	TAction *actClose;
	TLabel *lblVersion;
	TJvAppInstances *AppInst;
	TPopupMenu *popup;
	TMenuItem *mnuAddfile;
	TOpenDialog *opendlg;
	TAction *actSelect;
	TMenuItem *mnuSelectAll;
	TMenuItem *mnuSelectNone;
	TLabel *Label12;
	TIdTCPServer *FTPActivedata;
	TIdHTTP *checkip;
	TPageControl *PageControl1;
	TTabSheet *tsFax;
	TTabSheet *tsSchedule;
	TRadioButton *rbSendTogether;
	TRadioButton *rbSendIndividually;
	TLabel *Label1;
	TEdit *hFAXNumber;
	TComboBox *ABNames;
	TLabel *Label3;
	TEdit *hNotifyEmail;
	TButton *btnDelete;
	TButton *btnSave;
	TButton *btnSelect;
	TRadioButton *rbSendNow;
	TRadioButton *rbPostpone;
	TDateTimePicker *hDate;
	TDateTimePicker *hTime;
	TLabel *Label9;
	TSpinEdit *hDays;
	TLabel *Label11;
	TDateTimePicker *hHours;
	TLabel *Label10;
	TLabel *Label13;
	TSpinEdit *hDials;
	TPanel *Panel4;
	TLabel *Label5;
	TCheckListBox *lbDocuments;
	TBitBtn *btnUp;
	TBitBtn *btnDown;
	TLabel *Label7;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ABNamesChange(TObject *Sender);
	void __fastcall actSendExecute(TObject *Sender);
	void __fastcall actionsUpdate(TBasicAction *Action, bool &Handled);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall actUpExecute(TObject *Sender);
	void __fastcall actDownExecute(TObject *Sender);
	void __fastcall actSaveExecute(TObject *Sender);
	void __fastcall actDeleteExecute(TObject *Sender);
	void __fastcall actConfigureExecute(TObject *Sender);
	void __fastcall actCloseExecute(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormConstrainedResize(TObject *Sender, int &MinWidth, int &MinHeight,
          int &MaxWidth, int &MaxHeight);
	void __fastcall AppInstInstanceCreated(TObject *Sender, DWORD ProcessId);
	void __fastcall AppInstCmdLineReceived(TObject *Sender, TStrings *CmdLine);
	void __fastcall lbDocumentsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall lbDocumentsDragOver(TObject *Sender, TObject *Source, int X, int Y,
          TDragState State, bool &Accept);
	void __fastcall lbDocumentsDragDrop(TObject *Sender, TObject *Source, int X, int Y);
	void __fastcall mnuAddfileClick(TObject *Sender);
	void __fastcall actSelectExecute(TObject *Sender);
	void __fastcall mnuSelectAllClick(TObject *Sender);
	void __fastcall mnuSelectNoneClick(TObject *Sender);
	void __fastcall rbPostponeClick(TObject *Sender);
	void __fastcall hFAXNumberChange(TObject *Sender);
	void __fastcall FTPActivedataExecute(TIdContext *AContext);
	void __fastcall lbDocumentsClick(TObject *Sender);
	void __fastcall lbDocumentsDrawItem(TWinControl *Control, int Index, TRect &Rect,
          TOwnerDrawState State);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall rbSendModeClick(TObject *Sender);

private:	// User declarations
	CRITICAL_SECTION CSUserInterface;
	UnicodeString SelectFromAB;
	TPoint DragPoint;
	UnicodeString FFileBeingSent;
	bool FSending, FSettingNumberEdit;
	bool FHasNumber, FHasManyNumbers;
	bool FImmediateSend, FAutoClose;
	pcre16 *Fpattern;
	pcre16_extra *Fhints;
	const unsigned char *Fchartable;
	//void *Fgsinst;
	//bool Fgsinitialized;
	void __fastcall AddressBookChanged(TObject *Sender);
	void __fastcall AddressBookDuplicate(TObject *Sender, const UnicodeString& Name,
		bool& ChangeNumber);
	void __fastcall AddrBookTypeChanged(TObject *Sender, TAddressBookType AType);
	void __fastcall AddrBookLocationChanged(TObject *Sender);
	void __fastcall LanguageChanged(TObject *Sender);
	void __fastcall RegExChanged(TObject *Sender);
	void __fastcall CleanupRE();
	void __fastcall BringFaxWndToFront();
	void __fastcall EnableFields(bool Enable);
	int __fastcall GetFileFormat(unsigned int magic);
	int __fastcall GetFileFormat(const UnicodeString& FileName);
	int __fastcall GetNumbersCount(const UnicodeString& Numbers);
	bool __fastcall RegExProcessDocument(const UnicodeString& FileName,
		UnicodeString& Numbers);
	bool __fastcall FindMatches(const UnicodeString& Line, UnicodeString& Numbers);
	__inline void LockUI() { EnterCriticalSection(&CSUserInterface); }
	__inline void UnlockUI() { LeaveCriticalSection(&CSUserInterface); }
	void __fastcall ParseCommandLine(TStrings* pArgs);
	void __fastcall AddFileToList(const UnicodeString& Title,
		const UnicodeString& FileName, const UnicodeString& Number);
	void __fastcall AddFileToList(const UnicodeString& Title,
		const UnicodeString& FileName);
	MESSAGE void __fastcall HandleWMAddFax(TMessage& Message);
	MESSAGE void __fastcall HandleWMImmediateSend(TMessage& Message);
	MESSAGE void __fastcall HandleWMDropFiles(TWMDropFiles& Message);
	BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_ADDFAX, TMessage, HandleWMAddFax);
	MESSAGE_HANDLER(WM_IMMEDIATESEND, TMessage, HandleWMImmediateSend);
	MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, HandleWMDropFiles);
	END_MESSAGE_MAP(TForm);

public:		// User declarations
	__fastcall TFAXSend(TComponent* Owner);
	virtual __fastcall ~TFAXSend();
};
//---------------------------------------------------------------------------
extern PACKAGE TFAXSend *FAXSend;
//---------------------------------------------------------------------------
#endif
