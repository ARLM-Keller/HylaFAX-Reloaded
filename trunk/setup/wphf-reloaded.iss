; Winprint HylaFAX Reloaded
; Copyright (C) 2011-2013 Monti Lorenzo
;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#define SrcApp "..\wphfgui\Release\wphfgui.exe"
#define FileVerStr GetFileVersion(SrcApp)
#define StripBuild(str VerStr) Copy(VerStr, 1, RPos(".", VerStr) - 1)
#define AppVerStr StripBuild(FileVerStr)
#define AppName "Winprint HylaFAX Reloaded"

[Setup]
AppId={{F64330DD-1138-4CB4-BF45-87F9168933F6}
AppName={#AppName}
AppVersion={#AppVerStr}
AppVerName={#AppName} {#AppVerStr}
AppPublisher=Monti Lorenzo
AppPublisherURL=http://wphf-reloaded.sourceforge.net/
AppSupportURL=http://wphf-reloaded.sourceforge.net/
AppUpdatesURL=http://wphf-reloaded.sourceforge.net/
UninstallDisplayName={#AppName} {#AppVerStr}
UninstallDisplayIcon={app}\wphfgui.exe
VersionInfoCompany=Monti Lorenzo
VersionInfoCopyright=Copyright © 2011-2013 Monti Lorenzo
VersionInfoDescription={#AppName} setup program
VersionInfoProductName={#AppName}
VersionInfoVersion={#FileVerStr}
WizardImageFile=..\images\setup.bmp

CreateAppDir=yes
DefaultDirName={pf32}\Winprint HylaFAX Reloaded
DefaultGroupName=Winprint HylaFAX Reloaded

; we take care of these on our own
CloseApplications=no
RestartApplications=no

OutputBaseFilename=wphf-reloaded-setup
Compression=lzma
SolidCompression=yes
ArchitecturesAllowed=x86 x64
ArchitecturesInstallIn64BitMode=x64
MinVersion=0,5.0

LicenseFile=gpl-3.0.rtf

SignTool=lorenzomonti /d "{#AppName}"

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "it"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "fr"; MessagesFile: "compiler:Languages\French.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"

[CustomMessages]
;en
en.errRegister=Error in port monitor registration!
en.errDeregister=Error in port monitor deregistration!
en.RemPrinter=Removing existing HylaFAX printer...
en.AddPrinter=Adding HylaFAX printer...
en.StartSpooler=Starting Spooler...
en.RegisteringMonitor=Registering port monitor...
en.InstallVirtualPrinter=Install HylaFAX virtual printer
;it
it.errRegister=Errore nella registrazione del port monitor!
it.errDeregister=Errore nella deregistrazione del port monitor!
it.RemPrinter=Rimozione di stampanti HylaFAX esistenti...
it.AddPrinter=Aggiunta della stampante HylaFAX...
it.StartSpooler=Avvio dello Spooler...
it.RegisteringMonitor=Registrazione del port monitor...
it.InstallVirtualPrinter=Installa stampante virtuale HylaFAX
;fr
fr.InstallVirtualPrinter=Installez l'imprimante virtuelle HylaFAX
;de
de.errRegister=Fehler bei der Registrierung des Port-Monitors!
de.errDeregister=Fehler beim Austragen des Port-Monitors!
de.RemPrinter=Entferne existierenden HylaFAX-Drucker...
de.AddPrinter=Füge HylaFAX-Drucker hinzu...
de.StartSpooler=Starte Druckerwarteschlange ...
de.RegisteringMonitor=Registriere Port-Monitor...
de.InstallVirtualPrinter=Virtuellen HylaFAX-Drucker installieren
;es
es.InstallVirtualPrinter=Instale la impresora virtual HylaFAX
;earthquake
;2013-11-26: it's time to get back some reward for my work.
;de.earthquake=Spenden Sie 10 Euro für Emilia Romagna
;de.appeal=Ist Winprint HylaFAX Reloaded für Ihre Organisation 10 Euro wert? Der Autor möchte kein Geld für sich selbst, aber er bittet um eine Spende für die Region Emilia Romagna, die vom Erdbeben zerstört wurde.
;de.donate=JETZT SPENDEN
;de.url=http://www.protezionecivile.gov.it/jcms/en/donazioni.wp
;en.earthquake=Donate 10 euros for Emilia Romagna
;en.appeal=Is Winprint HylaFAX Reloaded worth 10 euros for your organization? The author doesn't ask money for himself, but asks you to donate for Emilia Romagna, devastated by the earthquake.
;en.donate=DONATE NOW!
;en.url=http://www.protezionecivile.gov.it/jcms/en/donazioni.wp
;it.earthquake=Dona 10 euro per l'Emilia Romagna
;it.appeal=Winprint HylaFAX Reloaded vale 10 euro per la tua organizzazione? L'autore del software non li chiede per sé, ma ti chiede di donarli per la ricostruzione dell'Emilia Romagna, distrutta dal terremoto.
;it.donate=DONA ORA!
;it.url=http://www.protezionecivile.gov.it/jcms/it/donazioni.wp
en.donatetitle=Do you like Winprint HylaFAX Reloaded?
en.appeal=If you like Winprint HylaFAX Reloaded you can consider making a donation to help keeping the good work up. Developer tools cost money, and developing software requires much time. Alternatively, you can show your appreciation by choosing a gift for me or my children from my wish list on Amazon. Thank you!
en.donate=Donate
en.gift=Choose a gift
en.donateurl=http://sourceforge.net/p/wphf-reloaded/donate/
en.gifturl=http://www.amazon.it/registry/wishlist/2L11VD62U0NZT
it.donatetitle=Apprezzi Winprint HylaFAX Reloaded?
it.appeal=Se apprezzi Winprint HylaFAX Reloaded potresti fare una donazione per aiutarmi a portare avanti il progetto. I tool per programmare costano, e la programmazione richiede molto tempo. In alternativa, puoi mostrare il tuo gradimento scegliendo un regalo per me o per le mie bimbe dalla mia lista desideri su Amazon. Grazie!
it.donate=Dona ora
it.gift=Scegli un regalo
it.donateurl=http://sourceforge.net/p/wphf-reloaded/donate/
it.gifturl=http://www.amazon.it/registry/wishlist/2L11VD62U0NZT

[Files]
; x64 files
Source: ..\x64\release\wphfmon.dll; DestDir: "{sys}"; Flags: promptifolder replacesameversion; Check: Is_x64
Source: "..\x64\release\wphfmonUI.dll"; DestDir: "{sys}"; Flags: promptifolder replacesameversion; Check: Is_x64
; x86 files
Source: "..\win32\release\wphfmon.dll"; DestDir: "{sys}"; Flags: promptifolder replacesameversion; Check: Is_x86
Source: "..\win32\release\wphfmonUI.dll"; DestDir: "{sys}"; Flags: promptifolder replacesameversion; Check: Is_x86
; files common to both architectures
Source: "..\wphfgui\Release\wphfgui.exe"; DestDir: "{app}"; Flags: promptifolder replacesameversion
Source: "..\wphfgui\Release\traynotif.exe"; DestDir: "{app}"; Flags: promptifolder replacesameversion
Source: "..\win32\release\regmon.exe"; DestDir: "{app}"; Flags: promptifolder replacesameversion
; Dell printer drivers
Source: "..\Dell\x86\*"; DestDir: "{app}\Dell"; MinVersion: 0.0,6.0; Flags: promptifolder replacesameversion recursesubdirs; Check: Is_x86
Source: "..\Dell\x64\*"; DestDir: "{app}\Dell"; MinVersion: 0.0,6.0; Flags: promptifolder replacesameversion recursesubdirs; Check: Is_x64
; language files
Source: "..\wphfgui\locale\*.mo"; DestDir: "{app}\locale"; Flags: recursesubdirs
; Ghostscript
Source: "..\ghostscript\bin\gsdll32.dll"; DestDir: "{app}\gs\bin"; Flags: promptifolder replacesameversion
Source: "..\ghostscript\bin\gswin32c.exe"; DestDir: "{app}\gs\bin"; Flags: promptifolder replacesameversion
Source: "..\ghostscript\lib\*"; DestDir: "{app}\gs\lib"; Flags: promptifolder replacesameversion
; ICU
Source: "..\icu\icudt52.dll"; DestDir: "{app}"; Flags: promptifolder replacesameversion
Source: "..\icu\icuuc52.dll"; DestDir: "{app}"; Flags: promptifolder replacesameversion
Source: "..\icu\msvcr100.dll"; DestDir: "{app}"; Flags: promptifolder replacesameversion

[Tasks]
Name: printer; Description: "{cm:InstallVirtualPrinter}"; Flags: checkedonce

[Icons]
Name: "{group}\Winprint HylaFAX GUI"; Filename: "{app}\wphfgui.exe"; WorkingDir: "{app}"

[Registry]
Root: HKLM; SubKey: "Software\Winprint HylaFAX Reloaded"; ValueType: string; ValueName: "InstallDir"; ValueData: "{app}"

[Run]
Filename: "{sys}\net.exe"; Parameters: "start spooler"; Flags: RunHidden; StatusMsg: {cm:StartSpooler}
Filename: "{app}\regmon.exe"; Parameters: "-r"; Flags: RunHidden; StatusMsg: {cm:RegisteringMonitor}
Filename: "{sys}\rundll32.exe"; Parameters: "PrintUI.dll,PrintUIEntry /dl /n ""HylaFAX"" /q"; StatusMsg: {cm:RemPrinter}; Tasks: printer
Filename: "{sys}\rundll32.exe"; Parameters: "PrintUI.dll,PrintUIEntry /if /b ""HylaFAX"" /f ""{win}\inf\ntprint.inf"" /r ""WPHF:"" /m ""Apple LaserWriter 16/600 PS"" /z /u /q"; OnlyBelowVersion: 0.0,6.0; StatusMsg: {cm:AddPrinter}; Tasks: printer
Filename: "{sys}\rundll32.exe"; Parameters: "PrintUI.dll,PrintUIEntry /if /b ""HylaFAX"" /f ""{app}\Dell\prndl001.inf"" /r ""WPHF:"" /m ""Dell 3100cn PS"" /z /u /q"; MinVersion: 0.0,6.0; StatusMsg: {cm:AddPrinter}; Tasks: printer

[UninstallRun]
Filename: "{sys}\net.exe"; Parameters: "start spooler"; Flags: RunHidden; StatusMsg: {cm:StartSpooler}

[UninstallDelete]
Type: filesandordirs; Name: "{commonappdata}\Winprint HylaFAX Reloaded\faxtmp"

[InstallDelete]
Type: files; Name: "{app}\gsdll32.dll"
Type: filesandordirs; Name: "{app}\lib"

[Code]
var
  bIsAnUpdate: Boolean;
  DonatePage: TWizardPage;
  lblMessage: TLabel;
  btnDonate: TNewButton;
  btnGift: TNewButton;

{----------------------------------------------------------------------------------------}
procedure btnDonateOnClick(Sender: TObject);
var
  err: Integer;
begin
  ShellExecAsOriginalUser('', CustomMessage('donateurl'), '', '',
    SW_SHOW, ewNoWait, err);
end;

{----------------------------------------------------------------------------------------}
procedure btnGiftOnClick(Sender: TObject);
var
  err: Integer;
begin
  ShellExecAsOriginalUser('', CustomMessage('gifturl'), '', '',
    SW_SHOW, ewNoWait, err);
end;

{----------------------------------------------------------------------------------------}
procedure CreateWizardPages;
begin
  //Donate please!
  DonatePage := CreateCustomPage(wpInstalling, CustomMessage('donatetitle'), '');
  
  lblMessage := TLabel.Create(DonatePage);
  lblMessage.Font.Size := 10;
  lblMessage.AutoSize := False;
  lblMessage.WordWrap := True;
  lblMessage.Width := DonatePage.SurfaceWidth;
  lblMessage.Height := ScaleY(120);
  lblMessage.Parent := DonatePage.Surface;
  lblMessage.Caption := CustomMessage('appeal');

  btnDonate := TNewButton.Create(DonatePage);
  btnDonate.Font.Size := 10;
  btnDonate.Font.Style := [fsBold];
  btnDonate.Top := lblMessage.Top + lblMessage.Height + ScaleY(4);
  btnDonate.Left := (DonatePage.SurfaceWidth div 3) * 1 - ScaleX(64);
  btnDonate.Width := ScaleX(128);
  btnDonate.Height := ScaleY(32);
  btnDonate.Caption := CustomMessage('donate');
  btnDonate.Parent := DonatePage.Surface;
  btnDonate.OnClick := @btnDonateOnClick;
	
  btnGift := TNewButton.Create(DonatePage);
  btnGift.Font.Size := 10;
  btnGift.Font.Style := [fsBold];
  btnGift.Top := lblMessage.Top + lblMessage.Height + ScaleY(4);
  btnGift.Left := (DonatePage.SurfaceWidth div 3) * 2 - ScaleX(64);
  btnGift.Width := ScaleX(128);
  btnGift.Height := ScaleY(32);
  btnGift.Caption := CustomMessage('gift');
  btnGift.Parent := DonatePage.Surface;
  btnGift.OnClick := @btnGiftOnClick;
end;

{----------------------------------------------------------------------------------------}
function Is_x86: Boolean;
begin
  Result := (ProcessorArchitecture = paX86);
end;

{----------------------------------------------------------------------------------------}
function Is_x64: Boolean;
begin
  Result := (ProcessorArchitecture = paX64);
end;

{----------------------------------------------------------------------------------------}
function DestinationFilesExist: Boolean;
begin
  Result := FileExists(ExpandConstant('{sys}\wphfmon.dll')) and
            FileExists(ExpandConstant('{sys}\wphfmonUI.dll'));
end;

{----------------------------------------------------------------------------------------}
function InitializeSetup: Boolean;
begin
  bIsAnUpdate := DestinationFilesExist;
  Result := True;
end;

{----------------------------------------------------------------------------------------}
procedure InitializeWizard;
begin
  CreateWizardPages;
end;

{----------------------------------------------------------------------------------------}
procedure CurStepChanged(CurStep: TSetupStep);
var
  rc: Integer;
begin
  case CurStep of
    ssInstall:
      begin
        //stop spooler since we're going to overwrite DLLs in use
        if bIsAnUpdate then
          Exec(ExpandConstant('{sys}\net.exe'), 'stop spooler', '', SW_SHOW, ewWaitUntilTerminated, rc);
      end;
  end;
end;

{----------------------------------------------------------------------------------------}
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  rc: Integer;
begin
  case CurUninstallStep of
    usUninstall:
      begin
        //make sure spooler is running
        Exec(ExpandConstant('{sys}\net.exe'), 'start spooler', '', SW_SHOW, ewWaitUntilTerminated, rc);
        //delete printer
        Exec(ExpandConstant('{sys}\rundll32.exe'), 'PrintUI.dll,PrintUIEntry /dl /n "HylaFAX" /q', '', SW_SHOW, ewWaitUntilTerminated, rc);
        //deregister monitor
        Exec(ExpandConstant('{app}\regmon.exe'), '-d', '', SW_SHOW, ewWaitUntilTerminated, rc);
        //stop spooler
        Exec(ExpandConstant('{sys}\net.exe'), 'stop spooler', '', SW_SHOW, ewWaitUntilTerminated, rc);
      end;
  end;
end;

