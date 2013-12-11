//---------------------------------------------------------------------------

#include <vcl.h>
#include <gnugettext.hpp>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("Main.cpp", FormNotify);
//---------------------------------------------------------------------------
#pragma link "gnugettext.lib"
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		TP_GlobalIgnoreClassProperty(__classid(TFont), L"Name");
	#ifdef _DEBUG
		UnicodeString path = ExtractFilePath(ExtractFileDir(ParamStr(0))) + L"locale";
	#else
		UnicodeString path = ExtractFilePath(ParamStr(0)) + L"locale";
	#endif
		bindtextdomain(L"wphfgui", path);
		//default domain for text translations
		textdomain(L"wphfgui");

		Application->Initialize();
		Application->MainFormOnTaskBar = false;
		Application->CreateForm(__classid(TFormNotify), &FormNotify);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
