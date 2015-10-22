if %PROCESSOR_ARCHITECTURE%==AMD64 goto AMD64
set PF=%PROGRAMFILES%
goto WORK

:AMD64
set PF=%PROGRAMFILES(X86)%

:WORK
set ALIAS=WinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE
set DCC="%PF%\Embarcadero\RAD Studio\7.0\bin\dcc32.exe"
set TL="%PF%\Embarcadero\RAD Studio\7.0\bin\tlib.exe"

REM
REM DCU release version, C++ headers and object files
REM
set INCLUDE="%PF%\Embarcadero\RAD Studio\7.0\lib";"%PF%\Embarcadero\RAD Studio\7.0\lib\obj";"%PF%\Embarcadero\RAD Studio\7.0\include"
set FLAGS=-$A8 -$B- -$C- -$D- -$G+ -$H+ -$I+ -$J- -$L+ -$M- -$O+ -$P+ -$Q- -$R- -$T- -$U- -$V+ -$W- -$X+ -$Y+ -$Z1
%DCC% %FLAGS% --no-config -DRELEASE -JPHNE -N0lib -NOobj -NHinclude -Q -A%ALIAS% -I%INCLUDE% -O%INCLUDE% -R%INCLUDE% -U%INCLUDE% gnugettext.pas
%TL% lib\gnugettext.lib /u obj\gnugettext.obj

REM
REM DCU debug version
REM
set INCLUDE="%PF%\Embarcadero\RAD Studio\7.0\lib\debug";"%PF%\Embarcadero\RAD Studio\7.0\lib\obj";"%PF%\Embarcadero\RAD Studio\7.0\include"
set FLAGS=-$A8 -$B- -$C+ -$D+ -$G+ -$H+ -$I+ -$J- -$L+ -$M- -$O- -$P+ -$Q- -$R- -$T- -$U- -$V+ -$W- -$X+ -$Y+ -$Z1
%DCC% %FLAGS% --no-config -DDEBUG -JPHNE -N0lib\debug -NOobj\debug -NHinclude -GD -Q -A%ALIAS% -I%INCLUDE% -O%INCLUDE% -R%INCLUDE% -U%INCLUDE% gnugettext.pas
%TL% lib\debug\gnugettext.lib /u obj\debug\gnugettext.obj
