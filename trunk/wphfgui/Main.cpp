//---------------------------------------------------------------------------

#include <vcl.h>
#include <gnugettext.hpp>
#pragma hdrstop

#include "Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "JvExControls"
#pragma link "JvGradient"
#pragma resource "*.dfm"
TFormNotify *FormNotify;
static HANDLE hClose;
//---------------------------------------------------------------------------

__fastcall TFormNotify::TFormNotify(TComponent* Owner)
	: TForm(Owner)
{
	hClose = CreateEvent(NULL, FALSE, FALSE, NULL);
}
//---------------------------------------------------------------------------

__fastcall TFormNotify::~TFormNotify()
{
	CloseHandle(hClose);
}
//---------------------------------------------------------------------------

void __fastcall TFormNotify::btnCloseClick(TObject *Sender)
{
	SetEvent(hClose);
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TFormNotify::CreateParams(Controls::TCreateParams &Params)
{
	TForm::CreateParams(Params);

	Params.Style |= WS_BORDER;
}
//---------------------------------------------------------------------------

void __fastcall TFormNotify::FormCreate(TObject *Sender)
{
	ShowWindow(Application->Handle, SW_HIDE);

	TranslateComponent(this);

	if (ParamCount() > 0)
		lblNumber->Caption = ParamStr(1);

	SystemParametersInfo(SPI_GETWORKAREA, 0, &FDesktop, 0);

	Left = FDesktop.right + 4;
	Top = FDesktop.bottom - Height - 4;

	new TFadeinThread(Handle);
}
//---------------------------------------------------------------------------

void __fastcall TFormNotify::HandleWMFadein(TMessage& Message)
{
	if (Message.WParam > 0)
		Left = FDesktop.right - 4 - (Width * Message.WParam / STEPS);
	else
		AlphaBlendValue = 255 + (255 * Message.WParam / STEPS);
}
//---------------------------------------------------------------------------

__fastcall TFadeinThread::TFadeinThread(HWND AWnd)
	: TThread(false)
{
	FreeOnTerminate = true;
	FWnd = AWnd;
}
//---------------------------------------------------------------------------

void __fastcall TFadeinThread::Execute()
{
	for (int i = 0; i <= STEPS; i++) {
		SendMessage(FWnd, WM_FADEIN, i, 0);
		if (WaitForSingleObject(hClose, 400 / STEPS) == WAIT_OBJECT_0)
			return;
	}

	if (WaitForSingleObject(hClose, 3000) == WAIT_OBJECT_0)
		return;

	for (int i = 0; i <= STEPS; i++) {
		SendMessage(FWnd, WM_FADEIN, -i, 0);
		if (WaitForSingleObject(hClose, 400 / STEPS) == WAIT_OBJECT_0)
			return;
	}

	PostMessage(FWnd, WM_CLOSE, 0, 0);
}
//---------------------------------------------------------------------------

void __fastcall TFormNotify::FormActivate(TObject *Sender)
{
	ShowWindow(Application->Handle, SW_HIDE);
}
//---------------------------------------------------------------------------

void __fastcall TFormNotify::FormShow(TObject *Sender)
{
	ShowWindow(Application->Handle, SW_HIDE);
}
//---------------------------------------------------------------------------

