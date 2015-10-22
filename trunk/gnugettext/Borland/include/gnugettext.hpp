// CodeGear C++Builder
// Copyright (c) 1995, 2009 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Gnugettext.pas' rev: 21.00

#ifndef GnugettextHPP
#define GnugettextHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <Sysinit.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Widestrings.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Strutils.hpp>	// Pascal unit
#include <Sysutils.hpp>	// Pascal unit
#include <Typinfo.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Gnugettext
{
//-- type declarations -------------------------------------------------------
typedef System::RawByteString RawUtf8String;

typedef System::UnicodeString DomainString;

typedef System::UnicodeString LanguageString;

typedef System::UnicodeString ComponentNameString;

typedef System::UnicodeString FilenameString;

typedef System::UnicodeString MsgIdString;

typedef System::UnicodeString TranslatedUnicodeString;

typedef void __fastcall (__closure *TTranslator)(System::TObject* obj);

class DELPHICLASS EGnuGettext;
class PASCALIMPLEMENTATION EGnuGettext : public Sysutils::Exception
{
	typedef Sysutils::Exception inherited;
	
public:
	/* Exception.Create */ inline __fastcall EGnuGettext(const System::UnicodeString Msg) : Sysutils::Exception(Msg) { }
	/* Exception.CreateFmt */ inline __fastcall EGnuGettext(const System::UnicodeString Msg, System::TVarRec const *Args, const int Args_Size) : Sysutils::Exception(Msg, Args, Args_Size) { }
	/* Exception.CreateRes */ inline __fastcall EGnuGettext(int Ident)/* overload */ : Sysutils::Exception(Ident) { }
	/* Exception.CreateResFmt */ inline __fastcall EGnuGettext(int Ident, System::TVarRec const *Args, const int Args_Size)/* overload */ : Sysutils::Exception(Ident, Args, Args_Size) { }
	/* Exception.CreateHelp */ inline __fastcall EGnuGettext(const System::UnicodeString Msg, int AHelpContext) : Sysutils::Exception(Msg, AHelpContext) { }
	/* Exception.CreateFmtHelp */ inline __fastcall EGnuGettext(const System::UnicodeString Msg, System::TVarRec const *Args, const int Args_Size, int AHelpContext) : Sysutils::Exception(Msg, Args, Args_Size, AHelpContext) { }
	/* Exception.CreateResHelp */ inline __fastcall EGnuGettext(int Ident, int AHelpContext)/* overload */ : Sysutils::Exception(Ident, AHelpContext) { }
	/* Exception.CreateResFmtHelp */ inline __fastcall EGnuGettext(System::PResStringRec ResStringRec, System::TVarRec const *Args, const int Args_Size, int AHelpContext)/* overload */ : Sysutils::Exception(ResStringRec, Args, Args_Size, AHelpContext) { }
	/* Exception.Destroy */ inline __fastcall virtual ~EGnuGettext(void) { }
	
};


class DELPHICLASS EGGProgrammingError;
class PASCALIMPLEMENTATION EGGProgrammingError : public EGnuGettext
{
	typedef EGnuGettext inherited;
	
public:
	/* Exception.Create */ inline __fastcall EGGProgrammingError(const System::UnicodeString Msg) : EGnuGettext(Msg) { }
	/* Exception.CreateFmt */ inline __fastcall EGGProgrammingError(const System::UnicodeString Msg, System::TVarRec const *Args, const int Args_Size) : EGnuGettext(Msg, Args, Args_Size) { }
	/* Exception.CreateRes */ inline __fastcall EGGProgrammingError(int Ident)/* overload */ : EGnuGettext(Ident) { }
	/* Exception.CreateResFmt */ inline __fastcall EGGProgrammingError(int Ident, System::TVarRec const *Args, const int Args_Size)/* overload */ : EGnuGettext(Ident, Args, Args_Size) { }
	/* Exception.CreateHelp */ inline __fastcall EGGProgrammingError(const System::UnicodeString Msg, int AHelpContext) : EGnuGettext(Msg, AHelpContext) { }
	/* Exception.CreateFmtHelp */ inline __fastcall EGGProgrammingError(const System::UnicodeString Msg, System::TVarRec const *Args, const int Args_Size, int AHelpContext) : EGnuGettext(Msg, Args, Args_Size, AHelpContext) { }
	/* Exception.CreateResHelp */ inline __fastcall EGGProgrammingError(int Ident, int AHelpContext)/* overload */ : EGnuGettext(Ident, AHelpContext) { }
	/* Exception.CreateResFmtHelp */ inline __fastcall EGGProgrammingError(System::PResStringRec ResStringRec, System::TVarRec const *Args, const int Args_Size, int AHelpContext)/* overload */ : EGnuGettext(ResStringRec, Args, Args_Size, AHelpContext) { }
	/* Exception.Destroy */ inline __fastcall virtual ~EGGProgrammingError(void) { }
	
};


class DELPHICLASS EGGComponentError;
class PASCALIMPLEMENTATION EGGComponentError : public EGnuGettext
{
	typedef EGnuGettext inherited;
	
public:
	/* Exception.Create */ inline __fastcall EGGComponentError(const System::UnicodeString Msg) : EGnuGettext(Msg) { }
	/* Exception.CreateFmt */ inline __fastcall EGGComponentError(const System::UnicodeString Msg, System::TVarRec const *Args, const int Args_Size) : EGnuGettext(Msg, Args, Args_Size) { }
	/* Exception.CreateRes */ inline __fastcall EGGComponentError(int Ident)/* overload */ : EGnuGettext(Ident) { }
	/* Exception.CreateResFmt */ inline __fastcall EGGComponentError(int Ident, System::TVarRec const *Args, const int Args_Size)/* overload */ : EGnuGettext(Ident, Args, Args_Size) { }
	/* Exception.CreateHelp */ inline __fastcall EGGComponentError(const System::UnicodeString Msg, int AHelpContext) : EGnuGettext(Msg, AHelpContext) { }
	/* Exception.CreateFmtHelp */ inline __fastcall EGGComponentError(const System::UnicodeString Msg, System::TVarRec const *Args, const int Args_Size, int AHelpContext) : EGnuGettext(Msg, Args, Args_Size, AHelpContext) { }
	/* Exception.CreateResHelp */ inline __fastcall EGGComponentError(int Ident, int AHelpContext)/* overload */ : EGnuGettext(Ident, AHelpContext) { }
	/* Exception.CreateResFmtHelp */ inline __fastcall EGGComponentError(System::PResStringRec ResStringRec, System::TVarRec const *Args, const int Args_Size, int AHelpContext)/* overload */ : EGnuGettext(ResStringRec, Args, Args_Size, AHelpContext) { }
	/* Exception.Destroy */ inline __fastcall virtual ~EGGComponentError(void) { }
	
};


class DELPHICLASS EGGIOError;
class PASCALIMPLEMENTATION EGGIOError : public EGnuGettext
{
	typedef EGnuGettext inherited;
	
public:
	/* Exception.Create */ inline __fastcall EGGIOError(const System::UnicodeString Msg) : EGnuGettext(Msg) { }
	/* Exception.CreateFmt */ inline __fastcall EGGIOError(const System::UnicodeString Msg, System::TVarRec const *Args, const int Args_Size) : EGnuGettext(Msg, Args, Args_Size) { }
	/* Exception.CreateRes */ inline __fastcall EGGIOError(int Ident)/* overload */ : EGnuGettext(Ident) { }
	/* Exception.CreateResFmt */ inline __fastcall EGGIOError(int Ident, System::TVarRec const *Args, const int Args_Size)/* overload */ : EGnuGettext(Ident, Args, Args_Size) { }
	/* Exception.CreateHelp */ inline __fastcall EGGIOError(const System::UnicodeString Msg, int AHelpContext) : EGnuGettext(Msg, AHelpContext) { }
	/* Exception.CreateFmtHelp */ inline __fastcall EGGIOError(const System::UnicodeString Msg, System::TVarRec const *Args, const int Args_Size, int AHelpContext) : EGnuGettext(Msg, Args, Args_Size, AHelpContext) { }
	/* Exception.CreateResHelp */ inline __fastcall EGGIOError(int Ident, int AHelpContext)/* overload */ : EGnuGettext(Ident, AHelpContext) { }
	/* Exception.CreateResFmtHelp */ inline __fastcall EGGIOError(System::PResStringRec ResStringRec, System::TVarRec const *Args, const int Args_Size, int AHelpContext)/* overload */ : EGnuGettext(ResStringRec, Args, Args_Size, AHelpContext) { }
	/* Exception.Destroy */ inline __fastcall virtual ~EGGIOError(void) { }
	
};


class DELPHICLASS EGGAnsi2WideConvError;
class PASCALIMPLEMENTATION EGGAnsi2WideConvError : public EGnuGettext
{
	typedef EGnuGettext inherited;
	
public:
	/* Exception.Create */ inline __fastcall EGGAnsi2WideConvError(const System::UnicodeString Msg) : EGnuGettext(Msg) { }
	/* Exception.CreateFmt */ inline __fastcall EGGAnsi2WideConvError(const System::UnicodeString Msg, System::TVarRec const *Args, const int Args_Size) : EGnuGettext(Msg, Args, Args_Size) { }
	/* Exception.CreateRes */ inline __fastcall EGGAnsi2WideConvError(int Ident)/* overload */ : EGnuGettext(Ident) { }
	/* Exception.CreateResFmt */ inline __fastcall EGGAnsi2WideConvError(int Ident, System::TVarRec const *Args, const int Args_Size)/* overload */ : EGnuGettext(Ident, Args, Args_Size) { }
	/* Exception.CreateHelp */ inline __fastcall EGGAnsi2WideConvError(const System::UnicodeString Msg, int AHelpContext) : EGnuGettext(Msg, AHelpContext) { }
	/* Exception.CreateFmtHelp */ inline __fastcall EGGAnsi2WideConvError(const System::UnicodeString Msg, System::TVarRec const *Args, const int Args_Size, int AHelpContext) : EGnuGettext(Msg, Args, Args_Size, AHelpContext) { }
	/* Exception.CreateResHelp */ inline __fastcall EGGAnsi2WideConvError(int Ident, int AHelpContext)/* overload */ : EGnuGettext(Ident, AHelpContext) { }
	/* Exception.CreateResFmtHelp */ inline __fastcall EGGAnsi2WideConvError(System::PResStringRec ResStringRec, System::TVarRec const *Args, const int Args_Size, int AHelpContext)/* overload */ : EGnuGettext(ResStringRec, Args, Args_Size, AHelpContext) { }
	/* Exception.Destroy */ inline __fastcall virtual ~EGGAnsi2WideConvError(void) { }
	
};


typedef void __fastcall (__closure *TOnDebugLine)(System::TObject* Sender, const System::UnicodeString Line, bool &Discard);

typedef int __fastcall (*TGetPluralForm)(int Number);

typedef void __fastcall (__closure *TDebugLogger)(System::AnsiString line);

class DELPHICLASS TMoFile;
class PASCALIMPLEMENTATION TMoFile : public System::TObject
{
	typedef System::TObject inherited;
	
private:
	bool doswap;
	
public:
	int Users;
	__fastcall TMoFile(const System::UnicodeString filename, const __int64 Offset, __int64 Size, const bool xUseMemoryMappedFiles, const System::UnicodeString ResName);
	__fastcall virtual ~TMoFile(void);
	System::RawByteString __fastcall gettext(const System::RawByteString msgid, bool &found);
	__property bool isSwappedArchitecture = {read=doswap, nodefault};
	
private:
	unsigned N;
	unsigned O;
	unsigned T;
	int startindex;
	int startstep;
	bool FUseMemoryMappedFiles;
	unsigned mo;
	unsigned momapping;
	char *momemoryHandle;
	char *momemory;
	unsigned __fastcall autoswap32(unsigned i);
	unsigned __fastcall CardinalInMem(char * baseptr, unsigned Offset);
};


class DELPHICLASS TDomain;
class PASCALIMPLEMENTATION TDomain : public System::TObject
{
	typedef System::TObject inherited;
	
private:
	bool Enabled;
	System::UnicodeString vDirectory;
	void __fastcall setDirectory(const System::UnicodeString dir);
	
public:
	TDebugLogger DebugLogger;
	System::UnicodeString Domain;
	__property System::UnicodeString Directory = {read=vDirectory, write=setDirectory};
	__fastcall TDomain(void);
	__fastcall virtual ~TDomain(void);
	void __fastcall SetLanguageCode(const System::UnicodeString langcode);
	void __fastcall SetFilename(const System::UnicodeString filename);
	void __fastcall GetListOfLanguages(Classes::TStrings* list);
	System::UnicodeString __fastcall GetTranslationProperty(System::UnicodeString Propertyname);
	System::RawByteString __fastcall gettext(const System::RawByteString msgid);
	
private:
	TMoFile* mofile;
	System::UnicodeString SpecificFilename;
	System::UnicodeString curlang;
	bool OpenHasFailedBefore;
	void __fastcall OpenMoFile(void);
	void __fastcall CloseMoFile(void);
};


class DELPHICLASS TExecutable;
class PASCALIMPLEMENTATION TExecutable : public System::TObject
{
	typedef System::TObject inherited;
	
public:
	virtual void __fastcall Execute(void) = 0 ;
public:
	/* TObject.Create */ inline __fastcall TExecutable(void) : System::TObject() { }
	/* TObject.Destroy */ inline __fastcall virtual ~TExecutable(void) { }
	
};


__interface IGnuGettextInstanceWhenNewLanguageListener;
typedef System::DelphiInterface<IGnuGettextInstanceWhenNewLanguageListener> _di_IGnuGettextInstanceWhenNewLanguageListener;
__interface IGnuGettextInstanceWhenNewLanguageListener  : public System::IInterface 
{
	
public:
	virtual void __fastcall WhenNewLanguage(const System::UnicodeString LanguageID) = 0 ;
};

class DELPHICLASS TGnuGettextInstance;
class PASCALIMPLEMENTATION TGnuGettextInstance : public System::TObject
{
	typedef System::TObject inherited;
	
private:
	TOnDebugLine fOnDebugLine;
	
public:
	bool Enabled;
	int DesignTimeCodePage;
	__fastcall TGnuGettextInstance(void);
	__fastcall virtual ~TGnuGettextInstance(void);
	void __fastcall UseLanguage(System::UnicodeString LanguageCode);
	void __fastcall GetListOfLanguages(const System::UnicodeString domain, Classes::TStrings* list);
	virtual System::UnicodeString __fastcall gettext(const System::UnicodeString szMsgId)/* overload */;
	System::UnicodeString __fastcall gettext_NoExtract(const System::UnicodeString szMsgId);
	System::UnicodeString __fastcall gettext_NoOp(const System::UnicodeString szMsgId);
	virtual System::UnicodeString __fastcall ngettext(const System::UnicodeString singular, const System::UnicodeString plural, int Number)/* overload */;
	System::UnicodeString __fastcall ngettext_NoExtract(const System::UnicodeString singular, const System::UnicodeString plural, int Number);
	System::UnicodeString __fastcall GetCurrentLanguage(void);
	System::UnicodeString __fastcall GetTranslationProperty(const System::UnicodeString Propertyname);
	System::UnicodeString __fastcall GetTranslatorNameAndEmail(void);
	void __fastcall TP_Ignore(System::TObject* AnObject, const System::UnicodeString name);
	void __fastcall TP_IgnoreClass(System::TClass IgnClass);
	void __fastcall TP_IgnoreClassProperty(System::TClass IgnClass, System::UnicodeString propertyname);
	bool __fastcall TP_TryGlobalIgnoreClass(System::TClass IgnClass);
	void __fastcall TP_GlobalIgnoreClass(System::TClass IgnClass);
	void __fastcall TP_GlobalIgnoreClassProperty(System::TClass IgnClass, System::UnicodeString propertyname);
	void __fastcall TP_GlobalHandleClass(System::TClass HClass, TTranslator Handler);
	void __fastcall TP_Remember(System::TObject* AnObject, System::UnicodeString PropName, System::UnicodeString OldValue);
	void __fastcall TranslateProperties(System::TObject* AnObject, System::UnicodeString textdomain = L"");
	void __fastcall TranslateComponent(Classes::TComponent* AnObject, const System::UnicodeString TextDomain = L"");
	void __fastcall RetranslateComponent(Classes::TComponent* AnObject, const System::UnicodeString TextDomain = L"");
	virtual System::UnicodeString __fastcall dgettext(const System::UnicodeString szDomain, const System::UnicodeString szMsgId)/* overload */;
	System::UnicodeString __fastcall dgettext_NoExtract(const System::UnicodeString szDomain, const System::UnicodeString szMsgId);
	virtual System::UnicodeString __fastcall dngettext(const System::UnicodeString szDomain, const System::UnicodeString singular, const System::UnicodeString plural, int Number)/* overload */;
	System::UnicodeString __fastcall dngettext_NoExtract(const System::UnicodeString szDomain, const System::UnicodeString singular, const System::UnicodeString plural, int Number);
	void __fastcall textdomain(const System::UnicodeString szDomain);
	System::UnicodeString __fastcall getcurrenttextdomain(void);
	void __fastcall bindtextdomain(const System::UnicodeString szDomain, const System::UnicodeString szDirectory);
	void __fastcall bindtextdomainToFile(const System::UnicodeString szDomain, const System::UnicodeString filename);
	System::UnicodeString __fastcall LoadResString(System::PResStringRec ResStringRec);
	void __fastcall DebugLogToFile(const System::UnicodeString filename, bool append = false);
	void __fastcall DebugLogPause(bool PauseEnabled);
	__property TOnDebugLine OnDebugLine = {read=fOnDebugLine, write=fOnDebugLine};
	void __fastcall RegisterWhenNewLanguageListener(_di_IGnuGettextInstanceWhenNewLanguageListener Listener);
	void __fastcall UnregisterWhenNewLanguageListener(_di_IGnuGettextInstanceWhenNewLanguageListener Listener);
	
protected:
	void __fastcall TranslateStrings(Classes::TStrings* _sl, const System::UnicodeString TextDomain);
	void __fastcall TranslateWideStrings(Widestrings::TWideStrings* _sl, const System::UnicodeString TextDomain);
	virtual void __fastcall WhenNewLanguage(const System::UnicodeString LanguageID);
	virtual void __fastcall WhenNewDomain(const System::UnicodeString TextDomain);
	virtual void __fastcall WhenNewDomainDirectory(const System::UnicodeString TextDomain, const System::UnicodeString Directory);
	
private:
	System::UnicodeString curlang;
	TGetPluralForm curGetPluralForm;
	System::UnicodeString curmsgdomain;
	Sysutils::TMultiReadExclusiveWriteSynchronizer* savefileCS;
	System::TextFile savefile;
	Classes::TStringList* savememory;
	System::UnicodeString DefaultDomainDirectory;
	Classes::TStringList* domainlist;
	Classes::TStringList* TP_IgnoreList;
	Classes::TList* TP_ClassHandling;
	Classes::TList* TP_GlobalClassHandling;
	TExecutable* TP_Retranslator;
	Classes::TInterfaceList* fWhenNewLanguageListeners;
	TExecutable* __fastcall TP_CreateRetranslator(void);
	void __fastcall FreeTP_ClassHandlingItems(void);
	bool __fastcall ClassIsIgnored(System::TClass AClass);
	void __fastcall TranslateProperty(System::TObject* AnObject, Typinfo::PPropInfo PropInfo, Classes::TStrings* TodoList, const System::UnicodeString TextDomain);
	TDomain* __fastcall Getdomain(const System::UnicodeString domain, const System::UnicodeString DefaultDomainDirectory, const System::UnicodeString CurLang);
};


//-- var, const, procedure ---------------------------------------------------
#define DefaultTextDomain L"default"
extern PACKAGE System::UnicodeString ExecutableFilename;
static const bool PreferExternal = false;
static const bool UseMemoryMappedFiles = true;
static const bool ReReadMoFileOnSameLanguage = false;
#define VCSVersion L"$LastChangedRevision: 220 $"
static const bool AutoCreateHooks = true;
extern PACKAGE TGnuGettextInstance* DefaultInstance;
extern PACKAGE System::UnicodeString __fastcall gettext(const System::UnicodeString szMsgId);
extern PACKAGE System::UnicodeString __fastcall gettext_NoExtract(const System::UnicodeString szMsgId);
extern PACKAGE System::UnicodeString __fastcall gettext_NoOp(const System::UnicodeString szMsgId);
extern PACKAGE System::UnicodeString __fastcall _(const System::UnicodeString szMsgId);
extern PACKAGE System::UnicodeString __fastcall dgettext(const System::UnicodeString szDomain, const System::UnicodeString szMsgId);
extern PACKAGE System::UnicodeString __fastcall dgettext_NoExtract(const System::UnicodeString szDomain, const System::UnicodeString szMsgId);
extern PACKAGE System::UnicodeString __fastcall dngettext(const System::UnicodeString szDomain, const System::UnicodeString singular, const System::UnicodeString plural, int Number);
extern PACKAGE System::UnicodeString __fastcall ngettext(const System::UnicodeString singular, const System::UnicodeString plural, int Number);
extern PACKAGE System::UnicodeString __fastcall ngettext_NoExtract(const System::UnicodeString singular, const System::UnicodeString plural, int Number);
extern PACKAGE void __fastcall textdomain(const System::UnicodeString szDomain);
extern PACKAGE System::UnicodeString __fastcall getcurrenttextdomain(void);
extern PACKAGE void __fastcall bindtextdomain(const System::UnicodeString szDomain, const System::UnicodeString szDirectory);
extern PACKAGE void __fastcall TP_Ignore(System::TObject* AnObject, const System::UnicodeString name);
extern PACKAGE void __fastcall TP_GlobalIgnoreClass(System::TClass IgnClass);
extern PACKAGE bool __fastcall TP_TryGlobalIgnoreClass(System::TClass IgnClass);
extern PACKAGE void __fastcall TP_IgnoreClass(System::TClass IgnClass);
extern PACKAGE void __fastcall TP_IgnoreClassProperty(System::TClass IgnClass, const System::UnicodeString propertyname);
extern PACKAGE void __fastcall TP_GlobalIgnoreClassProperty(System::TClass IgnClass, const System::UnicodeString propertyname);
extern PACKAGE void __fastcall TP_GlobalHandleClass(System::TClass HClass, TTranslator Handler);
extern PACKAGE void __fastcall TP_Remember(System::TObject* AnObject, System::UnicodeString PropName, System::UnicodeString OldValue);
extern PACKAGE void __fastcall TranslateComponent(Classes::TComponent* AnObject, const System::UnicodeString TextDomain = L"");
extern PACKAGE void __fastcall RetranslateComponent(Classes::TComponent* AnObject, const System::UnicodeString TextDomain = L"");
extern PACKAGE System::UnicodeString __fastcall GetTranslatorNameAndEmail(void);
extern PACKAGE void __fastcall UseLanguage(System::UnicodeString LanguageCode);
extern PACKAGE System::WideString __fastcall LoadResString(System::PResStringRec ResStringRec);
extern PACKAGE System::UnicodeString __fastcall LoadResStringW(System::PResStringRec ResStringRec);
extern PACKAGE System::UnicodeString __fastcall GetCurrentLanguage(void);
extern PACKAGE void __fastcall AddDomainForResourceString(const System::UnicodeString domain);
extern PACKAGE void __fastcall RemoveDomainForResourceString(const System::UnicodeString domain);
extern PACKAGE void __fastcall AddDomainForComponent(const System::UnicodeString domain);
extern PACKAGE void __fastcall RemoveDomainForComponent(const System::UnicodeString domain);
extern PACKAGE void __fastcall HookIntoResourceStrings(bool enabled = true, bool SupportPackages = false);

}	/* namespace Gnugettext */
using namespace Gnugettext;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// GnugettextHPP
