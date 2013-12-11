//---------------------------------------------------------------------------

#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "JvExControls.hpp"
#include "JvGradient.hpp"
#include <ExtCtrls.hpp>
#include <pngimage.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
#define WM_FADEIN (WM_USER + 2507)
#define STEPS 20
//---------------------------------------------------------------------------
class TFormNotify : public TForm
{
__published:	// IDE-managed Components
	TJvGradient *JvGradient1;
	TImage *Image1;
	TLabel *Label1;
	TLabel *lblNumber;
	TSpeedButton *btnClose;
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	RECT FDesktop;
	MESSAGE void __fastcall HandleWMFadein(TMessage& Message);
	BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_FADEIN, TMessage, HandleWMFadein);
	END_MESSAGE_MAP(TForm);
protected:
	virtual void __fastcall CreateParams(Controls::TCreateParams &Params);
public:		// User declarations
	__fastcall TFormNotify(TComponent* Owner);
	virtual __fastcall ~TFormNotify();
};
//---------------------------------------------------------------------------
class TFadeinThread : public TThread
{
private:
	HWND FWnd;
protected:
	virtual void __fastcall Execute();
public:
	__fastcall TFadeinThread(HWND AWnd);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormNotify *FormNotify;
//---------------------------------------------------------------------------
#endif
