//---------------------------------------------------------------------------

#include <vcl.h>
#include <gnugettext.hpp>
#pragma hdrstop

#include "Confirm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TConfirmation *Confirmation;
//---------------------------------------------------------------------------
__fastcall TConfirmation::TConfirmation(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TConfirmation::FormCreate(TObject *Sender)
{
	TranslateComponent(this, L"wphfgui");
}
//---------------------------------------------------------------------------
