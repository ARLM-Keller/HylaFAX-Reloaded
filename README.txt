=========================
Winprint HylaFAX Reloaded
=========================

1. Sources

Sources have been moved to SVN repository.
Please go to http://wphf-reloaded.svn.sourceforge.net/viewvc/wphf-reloaded/ if you want to browse files.
To check out: svn co https://wphf-reloaded.svn.sourceforge.net/svnroot/wphf-reloaded wphf-reloaded


2. Installation

Download and execute wphf-reloaded-setup.exe. The installer takes care of all the details.
Some notes:
- The installer optionally creates a new printer named "HylaFAX". Any printer with the same name is removed automatically during setup.
- On earlier systems (XP or lower) the "Apple LaserWriter 16/600 PS" is used (driver already included in the system). On newer systems (Vista or higher) the "Dell 3100cn PS" is used. A copy of the driver is included in the installer itself. I don't know if this breaks any licenses. I downloaded the drivers from the Microsoft website (http://catalog.update.microsoft.com/v7/site/Home.aspx). The driver is freely downloadable by anyone and it comes with no legal notices except one from Adobe that states that the files can be redistributed as long as they are not altered. So I thought I could embed all this stuff into my installer. This should make people happy.
- Working files (configuration, temporary files) are placed into a subdirectory under the common application data folder (which by default is "C:\Documents and Settings\All Users\Application Data" under XP and "C:\ProgramData" under Vista / 7). The subdirectory is named "Winprint HylaFAX Reloaded".
- The address book is now in CSV format, with delimiter=comma (,) and quote character=double quotes ("). The user can configure the path to the address book. The file name is fixed: addressbook.csv. If you don't like it, you can complain on Sourceforge :-) . If you had the old Winprint HylaFAX previously installed and you filled up the old "names.txt" and "numbers.txt", just copy them to the new address book location: the GUI will convert them automatically. The address book is placed in the same directory as the ini file, if not configured otherwise.
- The configuration now takes place in the GUI. The first time a fax is sent, the GUI prompts for the configuration. Parameters are written to an ini file.


3. Usage

Print your documents to "HylaFAX" printer. The GUI appears; you can send the document immediately, or append other documents. All collected documents can be sent as a single fax. So you can, for example, print a FAX cover with Word, then print something from Excel, and finally send all the pages in one fax call. You can arrange the order of the documents using the up/down arrows or dragging items inside the list box.
Starting with version 0.3.7, the GUI supports a rudimental API through the command line. You can pass various parameters to a new or running instance by passing options on the command line. The syntax is as follows:
wphfgui.exe [<option> | <file> <title>] ...
where:
<file> = full path to the file you want to send (PostScript, PDF, TIFF and PCL5 are supported, unless you have a server side script that can translate other types of document)
<title> = mandatory after <file> (not to break existing code): this is the title displayed in the GUI for the preceding <file>
available options:
-to <number>                         add number to recipient list (this option can be specified multiple times)
-at <datetime | time>                postpone send at given date / time - parsing follows current locale settings. Time is optional; if a time is specified, seconds are optional.
-notify <email>                      success/failure notification address
-retry <days,hh:mm | days | hh:mm>   keep retrying for the specified number of days / hours
-dials <max>                         maximum number of dials
-send                                send data to the server
-autoclose                           close after sending data to the server
-regex <expression | "default">      override the "Number caption enabled" configuration option setting it to TRUE, either using the provided expression or the default
-noregex                             override the "Number caption enabled" configuration option setting it to FALSE
-firstpage                           override the "Look at the first page only" configuration option setting it to TRUE
-wholedoc                            override the "Look at the first page only" configuration option setting it to FALSE
Parameters that contain spaces MUST be enclosed in double quotes.
The -retry param is interpreted this way: if a comma is found, it is treated as days,hours; otherwise, if a colon is found, it is treated as hours (days=0); otherwise it is treated as days (hours=0).
Example:
-retry 3        retry for 3 days
-retry 1,12:00  retry for 1 day and 12 hours
-retry 0:30     retry for 30 minutes
A double dash means "stop parsing parameters, only files follow". This can be handy in the unusual case where files exist that are named after options (eg. you want to send a file named "-send").

Example1: send 2 files to 2 recipients, on feb 17, 2012 at 23:00; notify failure/success to me@mycompany.com; retry for 6 hours; send immediately (-send), then exit the program (-autoclose).
wphfgui.exe "C:\some folder\file1.pdf" "title1" C:\anotherfolder\file2.tiff "title2" -to 0123456789 -to "0123 9876543" -at "02/17/2012 23:00" -notify me@mycompany.com -retry 6:00 -send -autoclose

Example2: send a file named "-send" to one recipient; send immediately, then exit the program.
wphfgui.exe -send -autoclose -to 12345678 -- -send "title for a strange file named -send"

Example3: append two files; then append two recipients; then send alltogether and exit program.
wphfgui.exe C:\folder\file1.pdf "first file"
wphfgui.exe C:\folder\file2.pdf "second file"
wphfgui.exe -to 0123456 -to 0987654
wphfgui.exe -send -autoclose


4. Auto number capture

The automatic number capture feature uses Ghostscript to convert the submitted fax into text, then matches the text against the Regular Expression specified in the configuration to find fax numbers embedded into the page.
The default Regular Expression is #{3}((?:[^#]|#(?!##))+)#{3}, which matches numbers like:
###012/3456###
###recipient@0123-45678###
###recipient@0123-45678#12###
In other words, put your fax number onto the page, enclose it between the special characters ###...### and the GUI will recognize it.
For the whole process to succeed, the submitted document must be convertible to text. Only PostScript and PDF files containing text can be processed; images (or PostScript/PDF made of images) cannot be converted back to text. To help the PostScript processor keeping the fax number as text, use regular fonts (such as Times New Roman, Courier New and so on), otherwise PostScript will render the text as a picture. The font can be any size, so you can put it in a corner and make it very small; the GUI will find it anyway.


5. Technical details

This software is derived from the popular HylaFAX client for Windows, Winprint HylaFAX, by Michael Stowe (http://sourceforge.net/projects/winprinthylafax/). The original version was modified so as to work under new operating systems (Vista, 7), both 32 and 64 bit. To accomplish this, the software has been splitted into two components:
- a port monitor, derived from Mfilemon - Multi File Port Monitor, by Lorenzo Monti (aka lomo74, myself. http://sourceforge.net/projects/mfilemon/);
- a GUI, for user interaction.
The port monitor receives data (Postscript) from the printer driver and writes it to a temporary file. Then it tries to send the name of the file to the GUI, through a pipe. If the port monitor does not find the pipe (the GUI is not running), it launches the GUI on the user desktop. This is the key of the transition to Vista / 7: the GUI has moved to a separate process, thus allowing it to appear on the user's desktop rather than on a protected desktop. Moreover, the port monitor is provided in two flavours (32 and 64 bit), thus allowing execution on 64 bit systems.


6. License

This software is released under the terms of the GNU GPL license. See http://www.gnu.org/licenses/gpl.html for details.


7. Bugs

This software is in the beta stage. Many improvements must be done yet. If you find any bug or have suggestions, please use SourceForge support facilities to submit your report. Thank you. If you want to contribute, drop me a line.
November 23, 2011 update: terminal services support, added in rel. 0.3.5, is not perfect yet. The big issue is: given a print job, from which desktop did that print job come from? The software uses a trick: enumerate all desktops (console plus all RDP connections) until one is found belonging to the same user that submitted the print job; then the GUI is executed on that desktop. The drawback is that if there are two or more sessions owned by the same user account, the GUI will appear on the first one it finds.
